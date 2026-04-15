void calculatePID() {
  P = currentError;
  I = I + currentError;
  D = currentError - lastError;
  
  float PID_Value = (Kp * P) + (Ki * I) + (Kd * D);

  leftMotorSpeed  = baseSpeed + PID_Value;
  rightMotorSpeed = baseSpeed - PID_Value;

  // تقييد السرعة حتى لا تتجاوز 255 أو تنزل عن الصفر
  if (leftMotorSpeed > 255) leftMotorSpeed = 255;
  if (leftMotorSpeed < 0) leftMotorSpeed = 0;
  if (rightMotorSpeed > 255) rightMotorSpeed = 255;
  if (rightMotorSpeed < 0) rightMotorSpeed = 0;

  // إرسال السرعة المحسوبة للمحركات
  analogWrite(PWMA, leftMotorSpeed);
  analogWrite(PWMB, rightMotorSpeed);
}