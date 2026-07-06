void setupMotors() {
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  // --- التعديل الاحترافي للتردد والدقة ---
  ledcAttach(PWMA, 10000, 10); 
  ledcAttach(PWMB, 10000, 10);

  stopMotor(); // الإيقاف التام في البداية
}

void setupSwitch() {
  pinMode(limitSwitch, INPUT_PULLUP);
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(switchPin2, INPUT_PULLUP);
  pinMode(switchPin3, INPUT_PULLUP);
}

void setupBuzzer() {
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // التأكد من إطفائه في البداية
}

void setupSensors() {
  for (int i = 0; i < 10; i++) {
    pinMode(sensorPins[i], INPUT);
  }
}

void setupNetwork() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //ArduinoOTA.begin();
  //TelnetStream.begin();
}

void turnOnService() {
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.setHostname("Belal_Line_Follower");     // اسم الروبوت على الشبكة
    ArduinoOTA.setPassword("789");      // كلمة المرور السرية

    // بدء الخدمات بعد ضبط الحماية
    ArduinoOTA.begin();
    TelnetStream.begin();

    // الحل السريع: تشغيل النغمة فقط إذا كان الروبوت متوقفاً
    if (!isRunning) {
      playToneWiFiConnected(); 
    }
    
    serviceStarted = true; 
  }
}

void setupMPU() {
  Wire.begin(SDA_PIN, SCL_PIN); // تحديد مسار الاتصال
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // سجل إدارة الطاقة
  Wire.write(0);    // إيقاظ الحساس
  Wire.endTransmission(true);
}



