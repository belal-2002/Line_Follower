void loopSwitch() {
  // 1. نقرأ حالة الزر 
  if (digitalRead(LIMIT_SWITCH) == LOW) {
    
    // 2. نتحقق هل مر وقت كافٍ منذ آخر مرة تم فيها تفعيل الزر بنجاح؟
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
      
      // 3. تحديث توقيت آخر ضغطة لتفعيل مؤقت التجاهل
      lastButtonPress = millis();
    }
  }
}