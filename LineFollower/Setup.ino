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
  Wire.begin(SDA_PIN, SCL_PIN); // <-- الحل الجوهري هنا: تحديد مسار الاتصال
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // سجل إدارة الطاقة
  Wire.write(0);    // إيقاظ الحساس
  Wire.endTransmission(true);

  // حساب الـ Offset لمعايرة الحساس عند بدء التشغيل (يجب أن يكون الروبوت ثابتاً)
  long sumZ = 0;
  for(int i=0; i<200; i++){
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x47); // مسجل Gyro Z
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 2, true);
    sumZ += (int16_t)(Wire.read() << 8 | Wire.read());
    delay(3);
  }
  gyroZ_offset = sumZ / 200.0;
  lastMpuTime = micros();
}

// دالة مقاطعة العجل الأيسر
void IRAM_ATTR leftEncoderISR() {
  // نقرأ دبابيس المحرك لمعرفة الاتجاه
  // المحرك الأيسر: AIN1=LOW و AIN2=HIGH تعني للأمام
  if (digitalRead(AIN1) == LOW && digitalRead(AIN2) == HIGH) {
    leftTicks++;
  } 
  // AIN1=HIGH و AIN2=LOW تعني للخلف
  else if (digitalRead(AIN1) == HIGH && digitalRead(AIN2) == LOW) {
    leftTicks--;
  } 
  // في حالة الفرملة أو الانزلاق الحر (الافتراضي نزيد المسافة)
  else {
    leftTicks++; 
  }
}

// دالة مقاطعة العجل الأيمن
void IRAM_ATTR rightEncoderISR() {
  // المحرك الأيمن: BIN1=HIGH و BIN2=LOW تعني للأمام
  if (digitalRead(BIN1) == HIGH && digitalRead(BIN2) == LOW) {
    rightTicks++;
  } 
  // BIN1=LOW و BIN2=HIGH تعني للخلف
  else if (digitalRead(BIN1) == LOW && digitalRead(BIN2) == HIGH) {
    rightTicks--;
  } 
  else {
    rightTicks++;
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

