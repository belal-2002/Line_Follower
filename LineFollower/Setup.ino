void setupMotors() {
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(LIMIT_SWITCH, INPUT_PULLUP);

  // --- التعديل الاحترافي للتردد والدقة ---
  ledcAttach(PWMA, 10000, 10); 
  ledcAttach(PWMB, 10000, 10);

  digitalWrite(STBY, LOW); // الإيقاف التام في البداية
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, LOW); 
  digitalWrite(BIN1, LOW);  digitalWrite(BIN2, LOW);  
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
}

void setupSensors() {
  for (int i = 0; i < 12; i++) {
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
    serviceStarted = true; 
  }
}
