void loopSwitch() {

  if (!isRunning) {
    if (digitalRead(switchPin1) == LOW) {
      switch1 = true;
    } else switch1 = false;
    if (digitalRead(switchPin2) == LOW) {
      switch2 = true;
    } else switch2 = false;
    if (digitalRead(switchPin3) == LOW) {
      switch3 = true;
    } else switch3 = false;
  }

  if (digitalRead(limitSwitch) == LOW) {
    
    if (millis() - lastButtonPress > debounceDelay) {
      
      isRunning = !isRunning; 
      
      if (isRunning) {
        digitalWrite(STBY, HIGH);
      } else {
        digitalWrite(STBY, LOW);  
        digitalWrite(AIN1, LOW);  digitalWrite(AIN2, LOW); 
        digitalWrite(BIN1, LOW);  digitalWrite(BIN2, LOW);  
        ledcWrite(PWMA, 0);
        ledcWrite(PWMB, 0);
      }
      
      lastButtonPress = millis();
    }
  }
}