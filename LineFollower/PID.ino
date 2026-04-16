void calculatePID() {
  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0; 
  lastTime = currentTime;

  // حماية من قسمة الصفر أو بطء اللوب
  if (dt <= 0.0) dt = 0.001;

  P = currentError;
  D = (currentError - lastError) / dt;
  
  float PID_Value = (Kp * P) + (Kd * D);

  leftMotorSpeed  = baseSpeed + PID_Value;
  rightMotorSpeed = baseSpeed - PID_Value;

  // تقييد السرعة حتى لا تتجاوز 1023 أو تنزل عن الصفر
  if (leftMotorSpeed > 1023) leftMotorSpeed = 1023;
  if (leftMotorSpeed < 0) leftMotorSpeed = 0;
  if (rightMotorSpeed > 1023) rightMotorSpeed = 1023;
  if (rightMotorSpeed < 0) rightMotorSpeed = 0;

  // إرسال السرعة المحسوبة للمحركات
  analogWrite(PWMA, leftMotorSpeed);
  analogWrite(PWMB, rightMotorSpeed);
}