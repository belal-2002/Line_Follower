// تعريف المتغيرات محلياً ولكن بشكل (static) لتحتفظ بقيمتها
static int calibState = 0; 
static bool wasButtonPressed = false;

// متغيرات التخزين المؤقت للمعايرة
static float whiteAvg[12];
static float blackAvg[12];
static float overallWhiteAvg = 0;
static float overallBlackAvg = 0;

void loopStrategy7() {
  bool currentButtonState = (digitalRead(limitSwitch) == LOW);

  // اكتشاف لحظة الضغط على الزر
  if (currentButtonState && !wasButtonPressed) {
    delay(50); // Debounce
    
    if (digitalRead(limitSwitch) == LOW) {
      if (calibState == 0) {
        TelnetStream.println("\n=== بدء معايرة الخط الأبيض ===");
        runCalibrationPhase(false); 
        
        // --- نغمة انتهاء الأبيض (سريعة ومتقطعة) ---
        for(int b=0; b<3; b++){
          buzzerOn(); delay(100);
          buzzerOff(); delay(100);
        }

        calibState = 1;
        TelnetStream.println("\n--> الروبوت متوقف. ضعه على الأسود واضغط الزر...");
      } 
      else if (calibState == 1) {
        TelnetStream.println("\n=== بدء معايرة الخط الأسود ===");
        runCalibrationPhase(true);  
        finalizeCalibration();      
        
        // --- نغمة انتهاء الأسود والمعايرة (طويلة ثم حادة) ---
        buzzerOn(); delay(600);    // نغمة طويلة
        buzzerOff(); delay(150);
        // نغير التردد لنغمة مختلفة (استدعاء الدالة الأصلية في الأردوينو مؤقتاً)
        tone(buzzerPin, 3000, 400); // نغمة حادة جداً لتأكيد النهاية
        delay(400);

        calibState = 2;
        TelnetStream.println("\n--- تمت المعايرة. لتكرارها اضغط الزر، أو غير الاستراتيجية للانطلاق ---");
      } 
      else if (calibState == 2) {
        TelnetStream.println("\n--- إعادة تعيين النظام. ضع الروبوت على الأبيض واضغط الزر ---");
        // نغمة التصفير
        buzzerOn(); delay(100); buzzerOff();
        calibState = 0;
      }
    }
  }
  
  wasButtonPressed = currentButtonState;

  if (TelnetStream.available()) { TelnetStream.read(); }
}

void runCalibrationPhase(bool isBlack) {
  long sums[12] = {0};
  
  TelnetStream.println("جاري أخذ 100 قراءة (وطباعة 10 عينات منها):");
  
  for (int i = 0; i < 100; i++) {
    for (int s = 0; s < 12; s++) {
      int val = analogRead(sensorPins[s]);
      sums[s] += val;
      
      if (i % 10 == 0) {
        TelnetStream.print(val);
        TelnetStream.print("\t");
      }
    }
    
    if (i % 10 == 0) {
      TelnetStream.println(); 
    }
    
    delay(10); 
  }

  TelnetStream.println("\n--- متوسط الحساسات ---");
  float overallSum = 0;
  
  for (int s = 0; s < 12; s++) {
    float avg = (float)sums[s] / 100.0;
    
    if (isBlack) {
      blackAvg[s] = avg;
    } else {
      whiteAvg[s] = avg;
    }
    
    TelnetStream.print("S"); TelnetStream.print(s+1); TelnetStream.print(":"); TelnetStream.print(avg, 1); TelnetStream.print("\t");
    
    if (s != 1 && s != 10) { 
      overallSum += avg;
    }
  }
  TelnetStream.println();
  
  float overall = overallSum / 10.0;
  if (isBlack) {
    overallBlackAvg = overall;
  } else {
    overallWhiteAvg = overall;
  }
}

void finalizeCalibration() {
  int calculatedThreshold = round((overallWhiteAvg + overallBlackAvg) / 2.0);
  
  target_White = round(overallWhiteAvg);
  target_Black = round(overallBlackAvg);
  lineThreshold = calculatedThreshold;
  
  for (int i = 0; i < 12; i++) {
    S_White[i] = round(whiteAvg[i]);
    S_Black[i] = round(blackAvg[i]);
  }

  TelnetStream.println("\n=======================================================");
  TelnetStream.println("🎉 اكتملت المعايرة بنجاح! تم تحديث الذاكرة.");
  TelnetStream.println("يمكنك نسخ الكود التالي ولصقه في ملف LineFollower.ino:");
  TelnetStream.println("=======================================================\n");
  
  TelnetStream.print("int S_White[12] = {");
  for (int i = 0; i < 12; i++) {
    TelnetStream.print(S_White[i]);
    if (i < 11) TelnetStream.print(", ");
  }
  TelnetStream.println("};");

  TelnetStream.print("int S_Black[12] = {");
  for (int i = 0; i < 12; i++) {
    TelnetStream.print(S_Black[i]);
    if (i < 11) TelnetStream.print(", ");
  }
  TelnetStream.println("};");

  TelnetStream.print("int target_White = "); TelnetStream.print(target_White); TelnetStream.println(";");
  TelnetStream.print("int target_Black = "); TelnetStream.print(target_Black); TelnetStream.println(";");
  TelnetStream.print("int lineThreshold = "); TelnetStream.print(lineThreshold); TelnetStream.println(";");
  
  TelnetStream.println("\n=======================================================");
}