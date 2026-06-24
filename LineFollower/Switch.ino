void loopSwitch() {
  bool bit1 = false;
  bool bit2 = false;
  bool bit3 = false;
  // فحص الأزرار وتحديد رقم الاستراتيجية (فقط عندما يكون الروبوت متوقفاً)
  if (!isRunning) {
    bit1 = (digitalRead(switchPin1) == LOW) ? 1 : 0; // يمثل الرقم 1
    bit2 = (digitalRead(switchPin2) == LOW) ? 1 : 0; // يمثل الرقم 2
    bit3 = (digitalRead(switchPin3) == LOW) ? 1 : 0; // يمثل الرقم 4
    
    // المجموع سيعطينا رقماً دقيقاً من 0 إلى 7
    strategy = (bit1 * 1) + (bit2 * 2) + (bit3 * 4);
  }

  if (digitalRead(limitSwitch) == LOW) {
    
    if (millis() - lastButtonPress > debounceDelay) {
      
      isRunning = !isRunning; 
      
      if (isRunning) {
        digitalWrite(STBY, HIGH);
        resetPitchOffset(); // <--- تمت إضافتها هنا لتتم المعايرة مرة واحدة قبل الانطلاق
        resetAngleZ();
        leftRadarOn = false;
        rightRadarOn = false;
        leftMidRadarOn = false;
        rightMidRadarOn = false;
        goLeft = false;
        goRight = false;
        turnLeft = false;
      } else {
        stopMotor();  
      }
      
      lastButtonPress = millis();
    }
  }
}