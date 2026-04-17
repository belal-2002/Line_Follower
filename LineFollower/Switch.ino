void loopSwitch() {
  if (digitalRead(LIMIT_SWITCH) == LOW) {
    delay(50); // تأخير لمنع التذبذب
    if (digitalRead(LIMIT_SWITCH) == LOW) {
      
      isRunning = !isRunning; 
      
      if (isRunning) {
        digitalWrite(STBY, HIGH);
      } else {
        digitalWrite(STBY, LOW);  
        digitalWrite(AIN1, LOW);  digitalWrite(AIN2, LOW); 
        digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);  
        ledcWrite(PWMA, 0);
        ledcWrite(PWMB, 0);
      }
      
      while(digitalRead(LIMIT_SWITCH) == LOW) {
        yield(); // يسمح لمعالج ESP32 بإبقاء اتصال الواي فاي حياً
      }
    }
  }
}