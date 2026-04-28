void loopMotor() {
  switch (caseMotor) {
    case 0:
      // 1. حساب السرعة مع السماح بالقيم السالبة (للدوران العكسي) وتقييدها بالسرعة القصوى
      leftMotorSpeed  = constrain(baseSpeed + PD_Value, -maximumSpeed, maximumSpeed);
      rightMotorSpeed = constrain(baseSpeed - PD_Value, -maximumSpeed, maximumSpeed);

      // 2. التحكم في المحرك الأيسر (اتجاه وسرعة)
      if (leftMotorSpeed >= 0) {
        // الاتجاه للأمام
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
      } else {
        // الاتجاه للخلف (تفعيل الدوران العكسي)
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
      }
      // إرسال السرعة كقيمة موجبة دائماً (القيمة المطلقة)
      ledcWrite(PWMA, abs(leftMotorSpeed));


      // 3. التحكم في المحرك الأيمن (اتجاه وسرعة)
      if (rightMotorSpeed >= 0) {
        // الاتجاه للأمام
        digitalWrite(BIN1, HIGH); 
        digitalWrite(BIN2, LOW);
      } else {
        // الاتجاه للخلف (تفعيل الدوران العكسي)
        digitalWrite(BIN1, LOW); 
        digitalWrite(BIN2, HIGH);
      }
      // إرسال السرعة كقيمة موجبة دائماً (القيمة المطلقة)
      ledcWrite(PWMB, abs(rightMotorSpeed));
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