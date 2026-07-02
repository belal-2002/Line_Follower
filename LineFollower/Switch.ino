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
        
        // 1. تصفير الملاحة والزوايا
        resetPitchOffset(); // <--- لتتم المعايرة مرة واحدة قبل الانطلاق
        resetAngleZ();
        totalOdometer = 0.0;
        lostLineDistance = 0.0;

        // 2. تصفير رادارات الاستكشاف 
        leftRadarOn = false;
        leftRadarOn2 = false;
        rightRadarOn = false;
        leftMidRadarOn = false;
        rightMidRadarOn = false;
        specialMemory = false;

        // 3. تصفير أوامر التوجيه
        goLeft = false;
        goRight = false;
        turnLeft = false;
        turnRight = false;

        // 4. تصفير ذاكرة الـ PID (لضمان بداية ناعمة)
        currentError = 0;
        lastError = 0;
        PD_Value = 0;

        // 5. تصفير أعلام الاستشفاء والبحث (Recovery)
        lineWasFound = true;
        Turn180now = false;
        sweep180Done = false;
        turnCooldownTime = 0;
        
        // 6. تصفير حالة الألوان المعكوسة
        isInverted = false;
        inversionCounterBlack = 0;
        inversionCounterWhite = 0;

      } else {
        stopMotor();  
      }
      
      lastButtonPress = millis();
    }
  }
}