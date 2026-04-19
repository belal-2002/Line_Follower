void loopSwitch() {

  if (!isRunning) {
    if (digitalRead(STRATEGY_3_PIN) == LOW) {
      // الاستراتيجية 3: وضع الهجوم والسرعة القصوى
      baseSpeed = 500;       
      maximumSpeed = 1000;   // استغلال شبه كامل لقدرة المحركات
      turnSpeed = 800;       // انعطاف عنيف للتقاطعات
      Kp = 1.5;              // استجابة أسرع للخطأ
      Kd = 20.0;             // فرملة أقوى لمنع التذبذب على السرعات العالية
    
    } else if (digitalRead(STRATEGY_2_PIN) == LOW) {
      // الاستراتيجية 2: السرعة المتوسطة وتجاهل الفجوات
      baseSpeed = 275;
      maximumSpeed = 625;
      turnSpeed = 600;
      Kp = 1.15;
      Kd = 15.0;
    
    } else if (digitalRead(STRATEGY_1_PIN) == LOW) {
      // الاستراتيجية 1: الاستكشاف البطيء وحفظ المسار
      baseSpeed = 150;
      maximumSpeed = 350;
      turnSpeed = 400;
      Kp = 0.8;              // استجابة أنعم
      Kd = 10.0;
    
    } else {
      // الوضع الافتراضي (في حال كانت جميع الأزرار مرفوعة)
      
    }
  }


  if (digitalRead(LIMIT_SWITCH) == LOW) {
    
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