void loopMotor() {
  switch (caseMotor) {
    case 0:
      // إرجاع المحركات للاتجاه الأمامي الطبيعي
      digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
      digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
      // تقييد السرعة وإرسالها للمحركات (10-bit)
      leftMotorSpeed  = constrain(baseSpeed + PD_Value, 0, maximumSpeed);
      rightMotorSpeed = constrain(baseSpeed - PD_Value, 0, maximumSpeed);

      ledcWrite(PWMA, leftMotorSpeed);
      ledcWrite(PWMB, rightMotorSpeed);
    break;
    case 1:
      // الدوران لليسار
      digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); 
      digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); 
    
      ledcWrite(PWMA, turnSpeed);
      ledcWrite(PWMB, turnSpeed);
    break;
    case 2:
      // الدوران لليمين
      digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
      digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); 
    
      ledcWrite(PWMA, turnSpeed);
      ledcWrite(PWMB, turnSpeed);
    break;
    case 3:
      // السير في خط مستقيم
      digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
      digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);

      ledcWrite(PWMA, turnSpeed);
      ledcWrite(PWMB, turnSpeed);
    break;
    case 4:

    break;
    case 5:

    break;
    case 6:

    break;
    case 7:

    break;
  }
}