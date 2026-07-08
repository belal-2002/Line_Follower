#include <WiFi.h>
  #include <ESPmDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <TelnetStream.h>
  #include <Wire.h>
  const int MPU_ADDR = 0x68;
  #define SDA_PIN 15  // إضافة رقم الدبوس
  #define SCL_PIN 16  // إضافة رقم الدبوس

// --- إعدادات شبكة الواي فاي ---
  const char* ssid = "Zain_B530_A013";      
  const char* password = "F8BLmiFRedB"; 
  //192.168.8.28

  //const char* ssid = "Galaxy S20+2db9";      
  //const char* password = "55555555";
  //10.189.201.71

// --- تعريف دبابيس المحركات ---
  #define PWMA 42   
  #define AIN1 41   
  #define AIN2 40   
  #define PWMB 19   
  #define BIN1 21   
  #define BIN2 20   
  #define STBY 48   

// ---  تعريف مفتاح التشغيل ودبابيس الاستراتيجيات  ---
  #define limitSwitch 17  
  #define switchPin1 47
  #define switchPin2 39
  #define switchPin3 18
  #define buzzerPin 1

// --- تعريف دبابيس الحساسات الـ 12 ---
  const int sensorPins[12] = {14, 13, 10, 9, 8, 7, 6, 5, 4, 2, 12, 11};

// ====================================================================
// --- المتغيرات العامة (Global Variables) ---
// ====================================================================

// ---------------------------------------------------------
// 1. متغيرات حالة الروبوت والنظام (System & State)
// ---------------------------------------------------------
  bool isRunning = false;        // حالة الروبوت (يعمل أم متوقف)
  bool serviceStarted = false;   // للتأكد من تشغيل خدمات الواي فاي و OTA
  byte strategy = 1;             // رقم الاستراتيجية الحالية (يتم تحديده من المفاتيح)

// ---------------------------------------------------------
// 2. متغيرات إعدادات السرعة (Speed Configuration)
// ---------------------------------------------------------
  int originalMaximumSpeed = 450; // السرعة القصوى المرجعية
  int originalBaseSpeed = 225;    // السرعة الأساسية المرجعية في الخط المستقيم
  int originalTurnSpeed = 250;    // سرعة الانعطاف المرجعية

  int maximumSpeed = originalMaximumSpeed; // السرعة القصوى الفعّالة (تتغير حسب المنحدر)
  int baseSpeed = originalBaseSpeed;       // السرعة الأساسية الفعّالة
  int turnSpeed = originalTurnSpeed;       // سرعة الانعطاف الفعّالة للعجل الخارجي
  int innerTurnSpeed = turnSpeed * 0.73;  // سرعة الانعطاف التفاضلية للعجل الداخلي
  //int innerTurnSpeed = turnSpeed * 1.73;

  int leftMotorSpeed = 0;         // السرعة اللحظية المُرسلة للمحرك الأيسر
  int rightMotorSpeed = 0;        // السرعة اللحظية المُرسلة للمحرك الأيمن

// ---------------------------------------------------------
// 3. متغيرات التحكم وتصحيح المسار (PID Control)
// ---------------------------------------------------------
  float Kp = 0.50;                // معامل التصحيح التناسبي (شراسة العودة للمنتصف)
  float Kd = 5.0;               // معامل التصحيح التفاضلي (نعومة الحركة وإخماد الاهتزاز)
  float currentError = 0;         // نسبة الخطأ اللحظية عن مركز الخط
  float lastError = 0;            // نسبة الخطأ السابقة (لحساب المعامل التفاضلي D)
  float PD_Value = 0;

// ---------------------------------------------------------
// 4. متغيرات الحساسات وقراءاتها (Sensors & Arrays)
// ---------------------------------------------------------
  const int sensorWeights[12] = {0, 0, -267, -190, -114, -38, 38, 114, 190, 267, 0, 0}; // أوزان الحساسات هندسياً
  int sensorValue[12];            // القيم التناظرية للحساسات بعد المعايرة
  int sensorBit = 0;              // تجميع حالة الحساسات (أبيض/أسود) في متغير واحد (Bitmask)

  byte rightSensor = 0;           // عدد حساسات الجهة اليمنى التي ترى الخط
  byte leftSensor = 0;            // عدد حساسات الجهة اليسرى التي ترى الخط
  byte midSensor = 0;             // عدد حساسات المنتصف التي ترى الخط
  byte midMidSensor = 0;          // عدد حساسات عمق المنتصف (للتأكد من التمركز التام)
  byte midMidMidSensor = 0;
  byte allSensor = 0;             // إجمالي عدد الحساسات التي ترى الخط

// ---------------------------------------------------------
// 5. متغيرات الرادار والاستكشاف الجانبي (Radars & Discovery)
// ---------------------------------------------------------
  bool leftOutRadar = 0;          // حالة الحساس الجانبي الأيسر المتطرف (S1)
  bool rightOutRadar = 0;         // حالة الحساس الجانبي الأيمن المتطرف (S12)
  bool leftRadar = 0;             // حالة الحساس الجانبي الأيسر المتطرف (S)
  bool rightRadar = 0;            // حالة الحساس الجانبي الأيمن المتطرف (S)
  bool leftMidRadar = 0;          // حالة الحساس الجانبي الأيسر الداخلي (S)
  bool rightMidRadar = 0;         // حالة الحساس الجانبي الأيمن الداخلي (S)
  
  bool leftOutRadarOn = false;       
  bool leftOutRadarOn2 = false;
  bool rightOutRadarOn = false;
  bool leftRadarOn = false;       
  bool rightRadarOn = false;      
  bool leftMidRadarOn = false;    // راية (Flag) لتفعيل حدث التقاط رادار اليسار الداخلي
  bool rightMidRadarOn = false;   // راية (Flag) لتفعيل حدث التقاط رادار اليمين الداخلي

  unsigned long radarMinTime = 30; // الحد الأدنى للذاكرة: بعدها يبدأ الرادار بسؤال الرادار المقابل
  unsigned long radarMaxTime = 600; // الحد الأقصى المطلق: المدة التي بعدها ينطفئ الرادار إجبارياً
  //200
  //700
  unsigned long leftOutRadarStartTime = 0;   
  unsigned long leftOutRadarStartTime2 = 0;
  unsigned long rightOutRadarStartTime = 0;
  unsigned long leftRadarStartTime = 0;   
  unsigned long rightRadarStartTime = 0;  
  unsigned long leftMidRadarStartTime = 0;// الوقت المسجل لحظة التقاط الرادار الأيسر الداخلي
  unsigned long rightMidRadarStartTime = 0;// الوقت المسجل لحظة التقاط الرادار الأيمن الداخلي

  bool specialMemory = false;       // راية (Flag) لمعرفة هل الشرط الخاص نشط أم لا
  unsigned long specialMemoryStartTime = 0; // لتسجيل لحظة تحقق الشرط بالضبط

// ---------------------------------------------------------
// 6. متغيرات التوقيت الزمني (Timers & Delays)
// ---------------------------------------------------------
  unsigned long turnStartTime = 0;     // توقيت بدء عملية الانعطاف
  unsigned long LineNotFoundTime = 0;  // توقيت لحظة فقدان الخط بالكامل
  unsigned long lastButtonPress = 0;   // توقيت آخر ضغطة زر (لمنع التداخل Debounce)
  const unsigned long debounceDelay = 400; // زمن الفلترة لضغطة الزر (ملي ثانية)

// ---------------------------------------------------------
// 7. متغيرات الفقدان والاستشفاء (Recovery & Movement Flags)
// ---------------------------------------------------------
  bool lineWasFound = true;        // راية تؤكد أن الروبوت يرى الخط حالياً

  bool Turn180now = false;         // تفعيل وضع الدوران 180 درجة للبحث عن الخط
  bool sweep180Done = false;       // راية تمنع الروبوت من الدوران 180 درجة مرتين متتاليتين

  bool goRight = false;            // أمر توجيه الروبوت لليمين
  bool goLeft = false;             // أمر توجيه الروبوت لليسار
  bool turnLeft = false;           // أمر تفعيل دوران اليسار الحاد
  bool turnRight = false;          // أمر تفعيل دوران اليمين الحاد

// ---------------------------------------------------------
// 8. متغيرات الانحدار والتوازن (MPU6050 Slope)
// ---------------------------------------------------------
  float pitchAngle = 0.0;          // زاوية ميل الروبوت الرأسية (موجب للصعود، سالب للنزول)
  float pitchOffset = 0.0;         // زاوية الميل المرجعية (تؤخذ عند معايرة نقطة الانطلاق)

// ---------------------------------------------------------
// 9. متغيرات منطقة التبديل الشفاف للألوان (Inversion Zone)
// ---------------------------------------------------------
  const int INVERSION_THRESH = 30; // عدد دورات القراءة المتتالية المطلوبة لتأكيد تبديل الألوان
  bool isInverted = false;         // هل الروبوت حالياً في منطقة ألوان معكوسة؟
  int inversionCounterBlack = 0;   // عداد مرشح (Filter) لاكتشاف الدخول في المنطقة المعكوسة
  int inversionCounterWhite = 0;   // عداد مرشح (Filter) لاكتشاف الخروج والعودة للمنطقة البيضاء

// ---------------------------------------------------------


int S_White[12] = {180, 166, 177, 172, 208, 182, 186, 165, 168, 208, 282, 199};
int S_Black[12] = {2214, 2372, 2719, 2430, 3271, 3087, 3000, 2525, 2251, 3070, 3666, 2545};
int target_White = 183;
int target_Black = 2794;
int lineThreshold = 1489;




void setup() {
  //Serial.begin(115200);
  setupMotors();
  setupSwitch();
  setupBuzzer();
  setupSensors();
  setupNetwork();
  setupMPU();
}

void loop() {
  loopSwitch();
  updateMPU();
  loopSensors();
  if (isRunning || strategy == 0) {
    
    loopStrategy();
    } //else {  
    if (serviceStarted) {
      loopPrint();
    } else { 
      turnOnService();
    }
  //}
}
