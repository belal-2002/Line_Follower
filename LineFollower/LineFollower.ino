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

// --- المتغيرات العامة (Global Variables) لتتشاركها جميع الملفات ---
// --- متغيرات حساب المسافة الافتراضية للرادار ---
  float RadarDistanceThreshold = 2.4;   // المسافة المطلوبة بالسنتيمتر (يمكنك تعديلها في أي وقت)
  float gapDistance = 7.0;   // مسافة تخطي الفجوات بالسنتيمتر
  float distanceNow = 0.0; // المسافة التراكمية الإجمالية التي قطعها الروبوت
  float absDistanceNow = 0.0;
  float leftRadarStartDistance = 0.0;   // المسافة المسجلة لحظة التقاط رادار اليسار
  float rightRadarStartDistance = 0.0;  // المسافة المسجلة لحظة التقاط رادار اليمين
  float leftMidRadarStartDistance = 0.0;   
  float rightMidRadarStartDistance = 0.0; 
  float lostLineDistance = 0.0;
  bool Turn180now = false; 
  bool sweep180Done = false; // يمنع الروبوت من الدوران 180 درجة مرتين متتاليتين

  static unsigned long lastPrintTime = 0;
  unsigned long turnStartTime = 0;
  unsigned long LineNotFoundTime = 0;
  unsigned long lastButtonPress = 0;
  const unsigned long debounceDelay = 400;
  bool bit1 = false;
  bool bit2 = false;
  bool bit3 = false;
  byte strategy = 1;
  byte buzzer = 0;
  int sensorBit = 0;
  byte rightSensor = 0;
  byte leftSensor = 0;
  byte midSensor = 0; 
  byte midMidSensor = 0;
  byte allSensor = 0;
  byte radar = 0;
  byte leftRadar = 0;
  byte rightRadar = 0;
  byte leftMidRadar = 0;
  byte rightMidRadar = 0;
  bool leftRadarOn = false;
  bool rightRadarOn = false;
  bool leftMidRadarOn = false;
  bool rightMidRadarOn = false;
  bool allRadarOn = false;
  bool goRight = false;
  bool goLeft = false;
  bool turnLeft = false;
  bool turnRight = false;

//PD
  bool isRunning = false;
  bool serviceStarted = false;

  float Kp = 1.8;   // تم رفعه لزيادة شراسة الانعطاف نحو المنتصف
  float Kd = 15.0;  // سيعمل الآن بشكل صحيح وناعم بعد إزالة الـ dt
  int originalMaximumSpeed = 400;     
  int originalBaseSpeed = 200;    
  int originalTurnSpeed = 250;  
  int leftMotorSpeed = 0;
  int rightMotorSpeed = 0;

  int maximumSpeed = originalMaximumSpeed;     
  int baseSpeed = originalBaseSpeed; 
  int turnSpeed = originalTurnSpeed; 
  int innerTurnSpeed = turnSpeed / 5*4; 

  float P = 0;
  float D = 0;
  float PD_Value =0;
  float lastError = 0;
  float currentError = 0;




// --- متغيرات الـ Odometry ---
  // استخدام volatile ضروري جداً للمتغيرات التي يتم تعديلها داخل المقاطعات (Interrupts)
  float currentAngleZ = 0.0;
  bool zeroAngleZ = false;
  volatile long leftTicks = 0;
  volatile long rightTicks = 0;

  const float distancePerTick = 0.8639; // المسافة لكل نبضة بالسنتيمتر
  const float trackWidth = 8.75; // المسافة بين العجلتين بالسنتيمتر
  float angleOffset = 0.0;       // لحفظ نقطة الصفر عند كل دوران
  float totalOdometer = 0.0;     // عداد المسافة التراكمي (دائماً يزداد)
  long lastLeftTicks_odo = 0;    // لحفظ النبضات السابقة للعجل الأيسر
  long lastRightTicks_odo = 0;   // لحفظ النبضات السابقة للعجل الأيمن

  // --- متغيرات كشف المنحدر ---
  float pitchAngle = 0.0;        // الزاوية الرأسية النهائية المعدلة
  float pitchOffset = 0.0;       // <--- (جديد) لحفظ زاوية الميلان عند لحظة الانطلاق


//Error
  const int sensorWeights[10] = {-444, -267, -190, -114, -38, 38, 114, 190, 267, 444};
  int sensorValue[10]; 
  long weightedSum = 0;
  long sum = 0;
  bool lineAvailable = true;
  bool lineWasFound = true;
  




int S_White[10] = {173, 178, 172, 215, 182, 197, 169, 175, 233, 906};
int S_Black[10] = {2468, 2804, 2721, 3377, 3118, 3720, 3020, 2962, 3801, 3942};
int target_White = 152;
int target_Black = 2552;
int lineThreshold = 1352;




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
