void calculatePID() {
  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0; 
  static unsigned long lastTime = currentTime;

// حماية من القسمة على صفر أو التوقف المفاجئ
  if (dt <= 0.0) dt = 0.001;

  P = currentError;
  D = (currentError - lastError) / dt;
  
  float PID_Value = (Kp * P) + (Kd * D);

  leftMotorSpeed  = baseSpeed + PID_Value;
  rightMotorSpeed = baseSpeed - PID_Value;

  // تقييد السرعة حتى لا تتجاوز 1023 أو تنزل عن الصفر
  leftMotorSpeed = constrain(leftMotorSpeed, 0, 1023);
  rightMotorSpeed = constrain(rightMotorSpeed, 0, 1023);

  // حل صراع الـ PWM: إرسال السرعة باستخدام ledcWrite
  ledcWrite(PWMA, leftMotorSpeed);
  ledcWrite(PWMB, rightMotorSpeed);

  // 6. تحديث الخطأ السابق في نهاية الدورة فقط! (وهذا يضمن عمل الـ D بشكل صحيح)
  if (!lineLost) {
    lastError = currentError;
  }
}