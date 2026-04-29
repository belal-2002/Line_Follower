// إعداد الطنان
void setupBuzzer() {
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // التأكد من إطفائه في البداية
}

// دالة لتشغيل الطنان بشكل مستمر (مفيدة للأخطاء)
void buzzerOn() {
  digitalWrite(buzzerPin, HIGH);
}

// دالة لإطفاء الطنان
void buzzerOff() {
  digitalWrite(buzzerPin, LOW);
}

// ==========================================
// دوال النغمات المخصصة للمعايرة (وغيرها)
// ==========================================

// 1. نغمة "تم الأبيض" (3 صفارات قصيرة وسريعة)
void playToneWhiteDone() {
  for (int i = 0; i < 3; i++) {
    buzzerOn();
    delay(80);
    buzzerOff();
    delay(80);
  }
}

// 2. نغمة "تم الأسود / اكتملت المعايرة" (صفارة طويلة ثم نغمة حادة)
void playToneCalibrationComplete() {
  buzzerOn();
  delay(600);    // صفارة طويلة
  buzzerOff();
  delay(150);    // صمت قصير
  
  // نغمة حادة للتأكيد (تردد 3000 هرتز لمدة 400 مللي ثانية)
  tone(buzzerPin, 3000, 400);
  delay(400);    // انتظار انتهاء النغمة
  noTone(buzzerPin);
}

// 3. نغمة التصفير/إعادة الضبط (صفارة واحدة متوسطة)
void playToneReset() {
  buzzerOn();
  delay(150);
  buzzerOff();
}