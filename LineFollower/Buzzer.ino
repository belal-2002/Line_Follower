// دالة لتشغيل الطنان بشكل مستمر (مفيدة للأخطاء)
void buzzerOn() {
  digitalWrite(buzzerPin, HIGH);
}

// دالة لإطفاء الطنان
void buzzerOff() {
  digitalWrite(buzzerPin, LOW);
}

// ==========================================
// دوال النغمات المخصصة للمعايرة والنظام
// ==========================================

// 1. نغمة "تم الأبيض" (3 صفارات قصيرة وسريعة)
void playToneWhiteDone() {
  for (int i = 0; i < 3; i++) {
    buzzerOn();
    delay(100);
    buzzerOff();
    delay(100);
  }
}

// 2. نغمة "تم الأسود / اكتملت المعايرة" (صفارة طويلة ثم نغمة حادة)
void playToneCalibrationComplete() {
  buzzerOn();
  delay(600); // صفارة طويلة 
  buzzerOff(); 
  delay(150); // صمت قصير
  
  // نغمة حادة للتأكيد (تردد 3000 هرتز لمدة 400 مللي ثانية)
  tone(buzzerPin, 3000, 400);
  delay(400); // انتظار انتهاء النغمة
  noTone(buzzerPin);
}

// 3. النغمة الجديدة: "تم الاتصال بالواي فاي بنجاح"
// (نغمة مزدوجة بتردد متوسط ثم عالي لتعطي إيحاء بنجاح الاتصال)
void playToneWiFiConnected() {
  tone(buzzerPin, 1500, 150); // نغمة متوسطة
  delay(150);
  tone(buzzerPin, 2500, 200); // نغمة حادة وأعلى
  delay(200);
  noTone(buzzerPin);
}