void loopStrategy7() {
  // فحص الكبسة
  if (digitalRead(limitSwitch) == LOW) {
    if (millis() - lastButtonPress > debounceDelay) {
      isRunning = !isRunning;
      if (isRunning) {
        if (calibState == 0) {
          runCalibration(false); // معايرة الأبيض
          isRunning = false; // إطفاء الروبوت برمجياً لانتظار الأسود
          calibState = 1;
        } 
        else if (calibState == 1) {
          runCalibration(true); // معايرة الأسود
          generateFinalCode(); // تخزين القيم تلقائياً وطباعة الملخص
          isRunning = false;
          calibState = 2;
        } 
        else if (calibState == 2) {
          TelnetStream.println("\n--- تم تصفير النظام. ضع الروبوت على الأبيض واضغط الزر ---");
          calibState = 0;
          isRunning = false;
        }
      }
      lastButtonPress = millis();
    }
  }
  
  // إبقاء الاتصال نشطاً
  if (TelnetStream.available()) { TelnetStream.read(); }
}

// دالة مدمجة لعمل المعايرة
void runCalibration(bool isBlack) {
  long sums[12] = {0};
  if (isBlack) {
    TelnetStream.println("\n=== بدء معايرة الخط الأسود ===");
  } else {
    TelnetStream.println("\n=== بدء معايرة الخط الأبيض ===");
  }
  
  TelnetStream.println("طباعة 10 عينات من أصل 100:");
  for (int i = 0; i < 100; i++) {
    for (int s = 0; s < 12; s++) {
      sensorValue[s] = analogRead(sensorPins[s]);
      sums[s] += sensorValue[s];
    }
    
    if (i % 10 == 0) {
      for (int s = 0; s < 12; s++) {
        TelnetStream.print(sensorValue[s]);
        TelnetStream.print("\t");
      }
      TelnetStream.println();
    }
    delay(10); 
  }

  TelnetStream.println("--- متوسط كل حساس ---");
  float overallSum = 0;
  
  for (int s = 0; s < 12; s++) {
    float avg = (float)sums[s] / 100.0;
    if (isBlack) {
      blackAvg[s] = avg;
    } else {
      whiteAvg[s] = avg;
    }
    
    TelnetStream.print("S");
    TelnetStream.print(s+1); 
    TelnetStream.print(":"); TelnetStream.print(avg, 1); TelnetStream.print(" \t");
    
    // حساب المجموع الكلي مع استثناء الحساسين (الرابع عشر والثاني عشر) للحفاظ على المرجع الأساسي المستقر
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
  
  TelnetStream.print("\n>>> المتوسط الكلي للمرجع (باستثناء 2 و 11): ");
  TelnetStream.println(overall);
  TelnetStream.println("الروبوت الآن في وضع التوقف. جاهز للخطوة القادمة.");
}

// دالة لتوليد الكود النهائي بأسلوب المصفوفات (Arrays)
void generateFinalCode() {
  int calculatedThreshold = round((overallWhiteAvg + overallBlackAvg) / 2.0);
  int target_W = round(overallWhiteAvg);
  int target_B = round(overallBlackAvg);

  // 1. تحديث المتغيرات العامة (Global Variables) تلقائياً في ذاكرة الروبوت
  target_White = target_W;
  target_Black = target_B;
  lineThreshold = calculatedThreshold;
  
  for (int i = 0; i < 12; i++) {
    S_White[i] = round(whiteAvg[i]);
    S_Black[i] = round(blackAvg[i]);
  }

  // 2. طباعة ملخص للتأكيد فقط
  TelnetStream.println("\n=======================================================");
  TelnetStream.println("🎉 اكتملت المعايرة الشاملة بنجاح!");
  TelnetStream.println("✅ تم تحديث قيم الحساسات وقيمة lineThreshold تلقائياً في ذاكرة الروبوت وهو جاهز للانطلاق!");
  TelnetStream.println("=======================================================\n");
  
  TelnetStream.println("القيم التي تم تخزينها واعتمادها حالياً:");
  
  // طباعة مصفوفة الأبيض
  TelnetStream.print("S_White[12] = {");
  for (int i = 0; i < 12; i++) {
    TelnetStream.print(S_White[i]);
    if (i < 11) TelnetStream.print(", ");
  }
  TelnetStream.println("};");

  // طباعة مصفوفة الأسود
  TelnetStream.print("S_Black[12] = {");
  for (int i = 0; i < 12; i++) {
    TelnetStream.print(S_Black[i]);
    if (i < 11) TelnetStream.print(", ");
  }
  TelnetStream.println("};");

  TelnetStream.printf("target_White = %d, target_Black = %d\n", target_White, target_Black);
  TelnetStream.printf("lineThreshold = %d\n", lineThreshold);
  TelnetStream.println("=======================================================");
}