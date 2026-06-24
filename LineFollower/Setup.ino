// إضافة متغيرات لحفظ زمن آخر نبضة لتطبيق الفلتر (توضع في الأعلى)
  const unsigned long debounceDelayMicros = ((3000000 / 175) / 1.5); // فلتر زمني: 11.5 ملي ثانية

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
    playToneWiFiConnected(); // --> إطلاق نغمة النجاح الجديدة هنا <--
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

// دالة مقاطعة العجل الأيسر المعدلة
void IRAM_ATTR leftEncoderISR() {
  static volatile unsigned long lastLeftPulseTime = 0;
  unsigned long currentTime = micros();
  // التأكد من مرور وقت كافٍ لتجاهل التشويش الوهمي
  if (currentTime - lastLeftPulseTime > debounceDelayMicros) {
    if (digitalRead(AIN1) == LOW && digitalRead(AIN2) == HIGH) {
      leftTicks++;
    } 
    else if (digitalRead(AIN1) == HIGH && digitalRead(AIN2) == LOW) {
      leftTicks--;
    } 
    else {
      leftTicks++;
    }
    lastLeftPulseTime = currentTime;
  }
}

// دالة مقاطعة العجل الأيمن المعدلة
void IRAM_ATTR rightEncoderISR() {
  static volatile unsigned long lastRightPulseTime = 0;
  unsigned long currentTime = micros();
  if (currentTime - lastRightPulseTime > debounceDelayMicros) {
    if (digitalRead(BIN1) == HIGH && digitalRead(BIN2) == LOW) {
      rightTicks++;
    } 
    else if (digitalRead(BIN1) == LOW && digitalRead(BIN2) == HIGH) {
      rightTicks--;
    } 
    else {
      rightTicks++;
    }
    lastRightPulseTime = currentTime;
  }
}

void setupEncoder() {
  // إعداد دبابيس حساسات السرعة
  pinMode(LEFT_ENCODER, INPUT_PULLUP);
  pinMode(RIGHT_ENCODER, INPUT_PULLUP);

  // ربط المقاطعات - تعمل فور استشعار ثقب في القرص
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER), leftEncoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER), rightEncoderISR, RISING);
}

