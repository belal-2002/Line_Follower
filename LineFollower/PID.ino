void calculatePD() { 
  // تم تغيير الاسم إلى PD ليعكس الواقع العلمي
  unsigned long currentTime = micros();
  static unsigned long lastTime = 0;
  float dt = (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  if (dt <= 0.0) dt = 0.001; 

  float P = currentError;
  float D = 0; // تصفير افتراضي

  // هندسة فقدان الخط 
  if (lineLost) {
    D = 0; // إبطال التفاضل تماماً لحظة الخروج
  } else {
    D = (currentError - lastError) / dt; // حساب معدل التغير الرياضي السليم
  }
  // تحديث الخطأ السابق
  if (!lineLost) {
    lastError = currentError;
  }
  // حساب معادلة الـ PD الخالصة والسريعة
  float PD_Value = (Kp * P) + (Kd * D);

  // تقييد السرعة (أقصى قيمة لـ 10-bit هي 1023)
  int leftMotorSpeed  = constrain(baseSpeed + PD_Value, 0, 1023);
  int rightMotorSpeed = constrain(baseSpeed - PD_Value, 0, 1023);

  // إرسال السرعة للمحركات
  ledcWrite(PWMA, leftMotorSpeed);
  ledcWrite(PWMB, rightMotorSpeed);
}