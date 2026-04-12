#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>  // 1. تضمين المكتبة الجديدة

// --- إعدادات شبكة الواي فاي الخاصة بك ---
const char* ssid = "Zain_B530_A013";      // ضع اسم الشبكة هنا
const char* password = "F8BLmiFRedB"; // ضع كلمة المرور هنا

// --- تعريف دبابيس المحرك الأيسر [cite: 1505] ---
#define PWMA 42   
#define AIN1 41   
#define AIN2 40   

// --- تعريف دبابيس المحرك الأيمن [cite: 1506] ---
#define PWMB 19   
#define BIN1 21   
#define BIN2 20   

// --- تعريف دبوس تفعيل درايفر المحركات [cite: 1505] ---
#define STBY 39   

// --- تعريف دبابيس مفاتيح التحكم [cite: 1501, 1502] ---
#define LIMIT_SWITCH 17  
#define DIP_SWITCH_1 18  

bool systemRunning = false;
bool rampDone = false; // لضمان تنفيذ التسارع مرة واحدة عند البدء

// --- متغيرات حالة النظام ---
bool isRunning = false;   // حالة الروبوت (يعمل أو متوقف)
bool wifiEnabled = false; // حالة الواي فاي

// دالة مخصصة لتشغيل الواي فاي والـ OTA
void startWiFiAndOTA() {
  TelnetStream.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // ننتظر الاتصال (مؤقتاً)
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    TelnetStream.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart(); // إعادة تشغيل المعالج إذا فشل الاتصال
  }

  // 2. تشغيل بث السيريال عبر الواي فاي
  TelnetStream.begin();
  
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // إعداد أوامر الـ OTA الأساسية وحمايتها
  ArduinoOTA.onStart([]() {
    // كإجراء أمان قاهر: إذا بدأ التحديث، أوقف المحركات فوراً!
    digitalWrite(STBY, LOW);
    TelnetStream.println("OTA Update Started. Motors Disabled.");
  });
  
  ArduinoOTA.onEnd([]() { Serial.println("\nOTA Update Finished!"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
  });

  ArduinoOTA.begin();
  wifiEnabled = true;
  TelnetStream.println("OTA Ready! Waiting for code updates...");
delay(5000);
  // 3. يمكنك الآن الطباعة عبر الواي فاي هكذا:
  TelnetStream.println("OTA & Wireless Serial Ready!");
}

// دالة مخصصة لقتل الواي فاي تماماً
void stopWiFi() {
  ArduinoOTA.end();       // إيقاف خدمة التحديث
  ArduinoOTA.end();
  WiFi.disconnect(true);  // قطع الاتصال بالراوتر
  WiFi.mode(WIFI_OFF);    // إغلاق شريحة الراديو لتوفير الطاقة ومنع التشويش
  wifiEnabled = false;
  TelnetStream.println("WiFi OFF! CPU & Battery are 100% focused on PID.");
}

void setup() {
  Serial.begin(115200);

  pinMode(PWMA, OUTPUT); pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT); pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(DIP_SWITCH_1, INPUT_PULLUP);
  digitalWrite(STBY, LOW); 

  // تشغيل الواي فاي في البداية لتلقي التحديثات
  startWiFiAndOTA();
}

void loop() {
  // 1. مراقبة التحديثات اللاسلكية (فقط إذا كان الواي فاي يعمل)
  if (wifiEnabled) {
    ArduinoOTA.handle();
  }

  // 2. قراءة كبسة التشغيل والإيقاف (Limit Switch)
  if (digitalRead(LIMIT_SWITCH) == LOW) {
    delay(50); // Debounce
    if (digitalRead(LIMIT_SWITCH) == LOW) {
      
      isRunning = !isRunning; // عكس الحالة
      
      if (isRunning) {
        // --- مرحلة الانطلاق (الهجوم) ---
        TelnetStream.println("Robot STARTING!");
        stopWiFi(); // إعدام الواي فاي فوراً
        digitalWrite(STBY, HIGH); // تفعيل المحركات
        
      } else {
        // --- مرحلة التوقف (الصيانة) ---
        digitalWrite(STBY, LOW);  // إيقاف المحركات أولاً
        TelnetStream.println("Robot STOPPED!");
        startWiFiAndOTA(); // إعادة إحياء الواي فاي لاستقبال الكود الجديد
      }
      
      while(digitalRead(LIMIT_SWITCH) == LOW); // انتظار رفع الإصبع
    }
  }

  // 3. خوارزمية التتبع والمحركات
  if (isRunning) {
    int dir = digitalRead(DIP_SWITCH_1);

    // تحديد الاتجاه (مع تصحيح المحرك الأيسر ليعمل مع الأيمن للأمام)
    if (dir == HIGH) { // للأمام
      digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); // تم العكس هنا للإصلاح
      digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);  
    } else { // للخلف
      digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);  // تم العكس هنا للإصلاح
      digitalWrite(BIN1, LOW);  digitalWrite(BIN2, HIGH); 
    }

    // تنفيذ تسلسل التسارع والتباطؤ مرة واحدة عند التشغيل
    if (!rampDone) {
      TelnetStream.println("Starting Acceleration Ramp...");
      // 1. التسارع من 0 إلى 255
      for (int speed = 0; speed <= 255; speed++) {
        analogWrite(PWMA, speed);
        analogWrite(PWMB, speed);
        delay(10); // سرعة التسارع (إجمالي 2.5 ثانية للوصول للقمة)
      }
      
      delay(500); // الثبات على السرعة القصوى لنصف ثانية

      // 2. التباطؤ من 255 إلى 0
      for (int speed = 255; speed >= 0; speed--) {
        analogWrite(PWMA, speed);
        analogWrite(PWMB, speed);
        delay(10);
      }
      
      rampDone = true;
      systemRunning = false; // إيقاف النظام تلقائياً بعد انتهاء الاختبار
      digitalWrite(STBY, LOW);
      TelnetStream.println("Ramp Test Completed.");
    }
  }
}