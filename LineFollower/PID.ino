void calculatePID() {
  P = currentError;
  I = I + currentError;
  D = currentError - lastError;
  
  float PID_Value = (Kp * P) + (Ki * I) + (Kd * D);

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