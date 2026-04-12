#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>

// --- إعدادات شبكة الواي فاي الخاصة بك ---
const char* ssid = "Zain_B530_A013";      
const char* password = "F8BLmiFRedB"; 

// --- تعريف دبابيس المحركات ---
#define PWMA 42   
#define AIN1 41   
#define AIN2 40   
#define PWMB 19   
#define BIN1 21   
#define BIN2 20   
#define STBY 39   

// --- تعريف دبابيس مفاتيح التحكم ---
#define LIMIT_SWITCH 17  
#define DIP_SWITCH_1 18  

// --- متغيرات حالة النظام الموحدة ---
bool isRunning = false;  // حالة الروبوت الموحدة
bool rampDone = false;   // لضمان تنفيذ التسارع مرة واحدة

void startWiFiAndOTA() {
  // استخدام السيريال العادي هنا لأن Telnet لم يعمل بعد
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart(); 
  }

  // الآن فقط يمكننا تشغيل TelnetStream
  TelnetStream.begin(); 
  
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.onStart([]() {
    digitalWrite(STBY, LOW);
    TelnetStream.println("OTA Update Started. Motors Disabled.");
  });
  
  ArduinoOTA.onEnd([]() { TelnetStream.println("\nOTA Update Finished!"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    // استخدمنا السيريال هنا لتخفيف الضغط اللاسلكي
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    TelnetStream.printf("Error[%u]: ", error);
  });

  ArduinoOTA.begin();
}

void setup() {
  Serial.begin(115200);

  pinMode(PWMA, OUTPUT); pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT); pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  
  pinMode(LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(DIP_SWITCH_1, INPUT_PULLUP);
  digitalWrite(STBY, LOW);

  // تشغيل الواي فاي وتجهيز الـ Telnet
  startWiFiAndOTA();
}

void loop() {
  // 1. مراقبة التحديثات اللاسلكية دائماً
  ArduinoOTA.handle();

  // 2. قراءة كبسة التشغيل والإيقاف (Limit Switch)
  if (digitalRead(LIMIT_SWITCH) == LOW) {
    delay(50); // Debounce
    if (digitalRead(LIMIT_SWITCH) == LOW) {
      
      isRunning = !isRunning; // عكس الحالة
      
      if (isRunning) {
        TelnetStream.println("-------------------------");
        TelnetStream.println("Robot STARTING!");
        digitalWrite(STBY, HIGH); // تفعيل المحركات
        rampDone = false; // تصفير التسارع ليبدأ من جديد
      } else {
        digitalWrite(STBY, LOW);  // إيقاف المحركات
        TelnetStream.println("Robot FORCE STOPPED by user!");
        TelnetStream.println("-------------------------");
      }
      
      while(digitalRead(LIMIT_SWITCH) == LOW); // انتظار رفع الإصبع
    }
  }

  // 3. خوارزمية التسارع والمحركات
  if (isRunning) {
    int dir = digitalRead(DIP_SWITCH_1);
    
    // تحديد الاتجاه 
    if (dir == HIGH) { // للأمام
      digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
      digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);  
    } else { // للخلف
      digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);  
      digitalWrite(BIN1, LOW);  digitalWrite(BIN2, HIGH); 
    }

    // تنفيذ تسلسل التسارع والتباطؤ مرة واحدة عند التشغيل
    if (!rampDone) {
      TelnetStream.println("Starting Acceleration Ramp (0 to 255)...");
      
      // التسارع
      for (int speed = 0; speed <= 255; speed++) {
        analogWrite(PWMA, speed);
        analogWrite(PWMB, speed);
        delay(10); 
      }
      
      TelnetStream.println("Max Speed Reached! Holding for 0.5s...");
      delay(500); 

      TelnetStream.println("Starting Deceleration Ramp (255 to 0)...");
      // التباطؤ
      for (int speed = 255; speed >= 0; speed--) {
        analogWrite(PWMA, speed);
        analogWrite(PWMB, speed);
        delay(10);
      }
      
      rampDone = true;
      isRunning = false; // إيقاف النظام تلقائياً بعد انتهاء الاختبار
      digitalWrite(STBY, LOW);
      TelnetStream.println("Ramp Test Completed Successfully.");
      TelnetStream.println("Waiting for OTA or Limit Switch press...");
      TelnetStream.println("-------------------------");
    }
  }
}