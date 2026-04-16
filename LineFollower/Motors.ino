void setupMotors() {
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(LIMIT_SWITCH, INPUT_PULLUP);

  // --- التعديل الاحترافي للتردد والدقة ---
  ledcAttach(PWMA, 10000, 10); 
  ledcAttach(PWMB, 10000, 10);

  digitalWrite(STBY, LOW); // الإيقاف التام في البداية

}

void checkLimitSwitch() {
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
        analogWrite(PWMA, 0);
        analogWrite(PWMB, 0);
        I = 0; // تصفير الخطأ التراكمي للـ Ki حتى لا يجن الروبوت عند التشغيل القادم
      }
      
      while(digitalRead(LIMIT_SWITCH) == LOW); 
    }
  }
}