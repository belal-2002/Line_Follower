void loopMotor() {
  if (lineAvailable) {
  // إرجاع المحركات للاتجاه الأمامي الطبيعي
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
  // تقييد السرعة وإرسالها للمحركات (10-bit)
  leftMotorSpeed  = constrain(baseSpeed + PD_Value, 0, maximumSpeed);
  rightMotorSpeed = constrain(baseSpeed - PD_Value, 0, maximumSpeed);

  ledcWrite(PWMA, leftMotorSpeed);
  ledcWrite(PWMB, rightMotorSpeed);
  } else {
    if (goRight) {
      // دفع المحرك الأيسر للأمام، وعكس الأيمن للخلف
      digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
      digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); 
    
      ledcWrite(PWMA, turnSpeed);
      ledcWrite(PWMB, turnSpeed);

      //lastError = 750;
      //lastError = 500;
      lastError = -750; 
    } else {
      // عكس المحرك الأيسر للخلف، ودفع الأيمن للأمام
      digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); 
      digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); 
    
      ledcWrite(PWMA, turnSpeed);
      ledcWrite(PWMB, turnSpeed);
      
      //lastError = -750;
      //lastError = -500;
      lastError = 750;
    }
  }
}