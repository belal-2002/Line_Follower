void moveMotor() {
  // إرجاع المحركات للاتجاه الأمامي الطبيعي
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
      
  // تقييد السرعة وإرسالها للمحركات (10-bit)
  leftMotorSpeed  = constrain(baseSpeed + PD_Value, 0, maximumSpeed);
  rightMotorSpeed = constrain(baseSpeed - PD_Value, 0, maximumSpeed);

  ledcWrite(PWMA, leftMotorSpeed);
  ledcWrite(PWMB, rightMotorSpeed);
}

void leftMotor() {
  // الدوران لليسار (Left Turn)
  // 1. المحرك الأيسر (الداخلي) يعمل كمرساة ويدور للخلف بسرعة منخفضة
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
  ledcWrite(PWMA, innerTurnSpeed); // <-- تم التعديل للسرعة التفاضلية
      
  // 2. المحرك الأيمن (الخارجي) يستمر بالدفع للأمام بالسرعة العالية المحددة
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); 
  ledcWrite(PWMB, turnSpeed);      // 600
}

void rightMotor() {
  // الدوران لليمين (Right Turn)
  // 1. المحرك الأيسر (الخارجي) يستمر بالدفع للأمام بالسرعة العالية المحددة
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
  ledcWrite(PWMA, turnSpeed);      // 600
      
  // 2. المحرك الأيمن (الداخلي) يعمل كمرساة ويدور للخلف بسرعة منخفضة
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); 
  ledcWrite(PWMB, innerTurnSpeed); // <-- تم التعديل للسرعة التفاضلية
}

void stopMotor() {
  // توقف تام
  digitalWrite(STBY, LOW);  
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, LOW); 
  digitalWrite(BIN1, LOW);  digitalWrite(BIN2, LOW);  
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
}

void forwardMotor() {
  // السير في خط مستقيم
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);

  ledcWrite(PWMA, baseSpeed);
  ledcWrite(PWMB, baseSpeed);
}

void rightMotor_2() {
  // السير في خط مستقيم
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);

  ledcWrite(PWMA, baseSpeed);
  ledcWrite(PWMB, baseSpeed - 25);
}









