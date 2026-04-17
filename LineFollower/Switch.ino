void loopSwitch() {
  if (digitalRead(LIMIT_SWITCH) == LOW) {
    delay(50); // تأخير لمنع التذبذب
    if (digitalRead(LIMIT_SWITCH) == LOW) {
      
      isRunning = !isRunning; 
      
      if (isRunning) {
        digitalWrite(STBY, HIGH);
        // الدوران للأمام
        digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
        digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);  
      } else {
        digitalWrite(STBY, LOW);  
        ledcWrite(PWMA, 0); // التعديل هنا
        ledcWrite(PWMB, 0); // التعديل هنا
      }
      
      while(digitalRead(LIMIT_SWITCH) == LOW); 
    }
  }
}