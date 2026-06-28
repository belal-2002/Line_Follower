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

// --- تعريف دبابيس حساسات السرعة (Encoders) ---
  #define LEFT_ENCODER 14  
  #define RIGHT_ENCODER 11

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
  const int sensorPins[10] = {13, 10, 9, 8, 7, 6, 5, 4, 2, 12};

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
  int originalMaximumSpeed = 500; // السرعة القصوى المرجعية
  int originalBaseSpeed = 250;    // السرعة الأساسية المرجعية في الخط المستقيم
  int originalTurnSpeed = 290;    // سرعة الانعطاف المرجعية

  int maximumSpeed = originalMaximumSpeed; // السرعة القصوى الفعّالة (تتغير حسب المنحدر)
  int baseSpeed = originalBaseSpeed;       // السرعة الأساسية الفعّالة
  int turnSpeed = originalTurnSpeed;       // سرعة الانعطاف الفعّالة للعجل الخارجي
  int innerTurnSpeed = turnSpeed / 5 * 4;  // سرعة الانعطاف التفاضلية للعجل الداخلي

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
  const int sensorWeights[10] = {-444, -267, -190, -114, -38, 38, 114, 190, 267, 444}; // أوزان الحساسات هندسياً
  int sensorValue[10];            // القيم التناظرية للحساسات بعد المعايرة
  int sensorBit = 0;              // تجميع حالة الحساسات (أبيض/أسود) في متغير واحد (Bitmask)

  byte rightSensor = 0;           // عدد حساسات الجهة اليمنى التي ترى الخط
  byte leftSensor = 0;            // عدد حساسات الجهة اليسرى التي ترى الخط
  byte midSensor = 0;             // عدد حساسات المنتصف التي ترى الخط
  byte midMidSensor = 0;          // عدد حساسات عمق المنتصف (للتأكد من التمركز التام)
  byte allSensor = 0;             // إجمالي عدد الحساسات التي ترى الخط

// ---------------------------------------------------------
// 5. متغيرات الرادار والاستكشاف الجانبي (Radars & Discovery)
// ---------------------------------------------------------
  byte leftRadar = 0;             // حالة الحساس الجانبي الأيسر المتطرف (S1)
  byte rightRadar = 0;            // حالة الحساس الجانبي الأيمن المتطرف (S10)
  byte leftMidRadar = 0;          // حالة الحساس الجانبي الأيسر الداخلي (S2)
  byte rightMidRadar = 0;         // حالة الحساس الجانبي الأيمن الداخلي (S9)

  bool leftRadarOn = false;       // راية (Flag) لتفعيل حدث التقاط رادار اليسار
  bool rightRadarOn = false;      // راية (Flag) لتفعيل حدث التقاط رادار اليمين
  bool leftMidRadarOn = false;    // راية (Flag) لتفعيل حدث التقاط رادار اليسار الداخلي
  bool rightMidRadarOn = false;   // راية (Flag) لتفعيل حدث التقاط رادار اليمين الداخلي

  float RadarDistanceThreshold = 4.0;         // المسافة المسموح للرادار بتذكر الخط خلالها (سم)
  float leftRadarStartDistance = 0.0;         // المسافة المسجلة لحظة التقاط رادار اليسار
  float rightRadarStartDistance = 0.0;        // المسافة المسجلة لحظة التقاط رادار اليمين
  float leftMidRadarStartDistance = 0.0;      // المسافة المسجلة لحظة التقاط الرادار الأيسر الداخلي
  float rightMidRadarStartDistance = 0.0;     // المسافة المسجلة لحظة التقاط الرادار الأيمن الداخلي

// ---------------------------------------------------------
// 6. متغيرات التوقيت الزمني (Timers & Delays)
// ---------------------------------------------------------
  unsigned long turnStartTime = 0;     // توقيت بدء عملية الانعطاف
  unsigned long LineNotFoundTime = 0;  // توقيت لحظة فقدان الخط بالكامل
  unsigned long lastButtonPress = 0;   // توقيت آخر ضغطة زر (لمنع التداخل Debounce)
  const unsigned long debounceDelay = 400; // زمن الفلترة لضغطة الزر (ملي ثانية)

// ---------------------------------------------------------
// 7. متغيرات الملاحة والإنكودر (Odometry & Navigation)
// ---------------------------------------------------------
  const float distancePerTick = 0.51836; // المسافة المقطوعة لكل نبضة من الإنكودر (سم)
  const float trackWidth = 9.15;         // المسافة بين مركزي العجلتين الخلفيتين (قاعدة العجلات) (سم)

  volatile long leftTicks = 0;     // العداد الحي لنبضات العجل الأيسر (يُعدل داخل المقاطعة)
  volatile long rightTicks = 0;    // العداد الحي لنبضات العجل الأيمن (يُعدل داخل المقاطعة)
  long currentLeftTicks = 0;       // نبضات العجل الأيسر المستنسخة (للاستخدام الآمن داخل الكود)
  long currentRightTicks = 0;      // نبضات العجل الأيمن المستنسخة (للاستخدام الآمن داخل الكود)
  long lastLeftTicks_odo = 0;      // النبضات السابقة للعجل الأيسر (لحساب الإزاحة الجديدة)
  long lastRightTicks_odo = 0;     // النبضات السابقة للعجل الأيمن (لحساب الإزاحة الجديدة)

  float distanceNow = 0.0;         // المسافة الصافية اللحظية (تتأثر بالدوران للخلف)
  float totalOdometer = 0.0;       // المسافة التراكمية الإجمالية (تزداد دائماً)
  float currentAngleZ = 0.0;       // زاوية الروبوت الحالية مقارنة بنقطة الصفر الديكارتية
  float angleOffset = 0.0;         // زاوية الإزاحة لتصفير الحسابات عند كل منعطف

// ---------------------------------------------------------
// 8. متغيرات الفقدان والاستشفاء (Recovery & Movement Flags)
// ---------------------------------------------------------
  bool lineWasFound = true;        // راية تؤكد أن الروبوت يرى الخط حالياً
  float lostLineDistance = 0.0;    // المسافة المسجلة لحظة فقدان الخط
  float gapDistance = 9.0;         // المسافة المسموح بقطعها أعمى لتجاوز الفجوات المقطوعة (سم)

  bool Turn180now = false;         // تفعيل وضع الدوران 180 درجة للبحث عن الخط
  bool sweep180Done = false;       // راية تمنع الروبوت من الدوران 180 درجة مرتين متتاليتين

  bool goRight = false;            // أمر توجيه الروبوت لليمين
  bool goLeft = false;             // أمر توجيه الروبوت لليسار
  bool turnLeft = false;           // أمر تفعيل دوران اليسار الحاد
  bool turnRight = false;          // أمر تفعيل دوران اليمين الحاد

// ---------------------------------------------------------
// 9. متغيرات الانحدار والتوازن (MPU6050 Slope)
// ---------------------------------------------------------
  float pitchAngle = 0.0;          // زاوية ميل الروبوت الرأسية (موجب للصعود، سالب للنزول)
  float pitchOffset = 0.0;         // زاوية الميل المرجعية (تؤخذ عند معايرة نقطة الانطلاق)

// ---------------------------------------------------------
// 10. متغيرات منطقة التبديل الشفاف للألوان (Inversion Zone)
// ---------------------------------------------------------
  const int INVERSION_THRESH = 30; // عدد دورات القراءة المتتالية المطلوبة لتأكيد تبديل الألوان
  bool isInverted = false;         // هل الروبوت حالياً في منطقة ألوان معكوسة؟
  int inversionCounterBlack = 0;   // عداد مرشح (Filter) لاكتشاف الدخول في المنطقة المعكوسة
  int inversionCounterWhite = 0;   // عداد مرشح (Filter) لاكتشاف الخروج والعودة للمنطقة البيضاء

// ---------------------------------------------------------


int S_White[10] = {189, 183, 182, 214, 198, 198, 178, 179, 225, 395};
int S_Black[10] = {2652, 2431, 2332, 2988, 2961, 2712, 2268, 1940, 2931, 3707};
int target_White = 194;
int target_Black = 2570;
int lineThreshold = 1382;



void setup() {
  //Serial.begin(115200);
  setupMotors();
  setupSwitch();
  setupBuzzer();
  setupSensors();
  setupNetwork();
  setupMPU();
  setupEncoder();
}

void loop() {
  loopSwitch();
  updateDistance();
  //updateMPU();
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
