#include <WiFi.h>
  #include <ESPmDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <TelnetStream.h>

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
  #define STBY 39   

// ---  تعريف مفتاح التشغيل ودبابيس الاستراتيجيات  ---
  #define limitSwitch 17  
  #define switchPin1 18
  #define switchPin2 47
  #define switchPin3 48

// --- تعريف دبابيس الحساسات الـ 12 ---
  const int sensorPins[12] = {14, 13, 10, 9, 8, 7, 6, 5, 4, 2, 12, 11};

// --- المتغيرات العامة (Global Variables) لتتشاركها جميع الملفات ---
  static unsigned long lastPrintTime = 0;
  static unsigned long rightRadarTime = 0;
  static unsigned long leftRadarTime = 0;
  static unsigned long allRadarTime = 0;
  unsigned long turnStartTime = 0;
  unsigned long lastButtonPress = 0;
  const unsigned long debounceDelay = 400;
  const unsigned long RadarTime = 50;
  bool bit1 = false;
  bool bit2 = false;
  bool bit3 = false;
  byte strategy = 2;
  byte caseMotor = 0;
  int sensorBit = 0;
  byte rightSensor = 0;
  byte leftSensor = 0;
  byte midSensor = 0; 
  byte allSensor = 0;
  byte radar = 0;
  byte rightRadar = 0;
  byte leftRadar = 0;
  bool rightRadarOn = false;
  bool leftRadarOn = false;
  bool allRadarOn = false;
  bool goRight = false;
  bool goLeft = false;

//PD
  bool isRunning = false;

  float Kp = 1.15;
  float Kd = 15.0;
  int maximumSpeed = 625;  //1023;   
  int baseSpeed = 275;  //800;  
  int turnSpeed = 600;    

  float P = 0;
  float D = 0;
  float PD_Value =0;
  float lastError = 0;
  float currentError = 0;

  unsigned long currentTime = 0;
  static unsigned long lastTime = 0;
  float dt;

//Error
  const int sensorWeights[12] = {-698, -444, -267, -190, -114, -38, 38, 114, 190, 267, 444, 698};
  int sensorValue[12]; 
  long weightedSum = 0;
  long sum = 0;
  bool lineAvailable = true;
  bool lineWasFound = true;
  

  int lineThreshold = 1333;


int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

bool serviceStarted = false;

void setup() {
  //Serial.begin(115200);
  setupMotors();
  setupSwitch();
  setupSensors();
  setupNetwork();
}

void loop() {
  loopSwitch();
  loopSensors();
  if (isRunning) {
    loopStrategy();
    loopMotor();
    } //else {  
    if (serviceStarted) {
      loopPrint();
    } else { 
      turnOnService();
    }
  //}
}