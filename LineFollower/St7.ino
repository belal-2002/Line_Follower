void loopStrategy7() {
  // فحص الكبسة
  if (digitalRead(limitSwitch) == LOW) {
    if (millis() - lastButtonPress > debounceDelay) {
      isRunning = !isRunning;
      
      if (isRunning) {
        if (calibState == 0) {
          runCalibration(false); // معايرة الأبيض
          isRunning = false;     // إطفاء الروبوت برمجياً لانتظار الأسود
          calibState = 1;
        } 
        else if (calibState == 1) {
          runCalibration(true);  // معايرة الأسود
          generateFinalCode();   // طباعة الكود النهائي لجميع الحساسات
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
    
    TelnetStream.print("S"); TelnetStream.print(s+1); 
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

  TelnetStream.println("\n=======================================================");
  TelnetStream.println("🎉 اكتملت المعايرة الشاملة بنجاح! انسخ الكود التالي:");
  TelnetStream.println("=======================================================\n");
  
  TelnetStream.println("// 1. ضع هذه المصفوفات في ملف Sensors.ino (خارج الدوال بالبداية، أو داخل دالة loopSensors):");
  
  // طباعة مصفوفة الأبيض
  TelnetStream.print("int S_White[12] = {");
  for (int i = 0; i < 12; i++) {
    TelnetStream.print(round(whiteAvg[i]));
    if (i < 11) TelnetStream.print(", ");
  }
  TelnetStream.println("};");

  // طباعة مصفوفة الأسود
  TelnetStream.print("int S_Black[12] = {");
  for (int i = 0; i < 12; i++) {
    TelnetStream.print(round(blackAvg[i]));
    if (i < 11) TelnetStream.print(", ");
  }
  TelnetStream.println("};");

  TelnetStream.printf("int target_White = %d, target_Black = %d;\n\n", target_W, target_B);
  
  TelnetStream.println("// 2. استبدل كود القراءة والمعايرة في دالة loopSensors بالكامل بهذا الكود:");
  TelnetStream.println("  for (int i = 0; i < 12; i++) {");
  TelnetStream.println("    // أخذ القراءة الخام");
  TelnetStream.println("    sensorValue[i] = analogRead(sensorPins[i]);");
  TelnetStream.println("    ");
  TelnetStream.println("    // المعايرة الخطية الفردية لكل حساس ليطابق المرجع المثالي");
  TelnetStream.println("    sensorValue[i] = map(sensorValue[i], S_White[i], S_Black[i], target_White, target_Black);");
  TelnetStream.println("    ");
  TelnetStream.println("    // حماية القيم (استخدام 50 كحد أدنى حسب طلبك)");
  TelnetStream.println("    sensorValue[i] = constrain(sensorValue[i], 50, 4095);");
  TelnetStream.println("  }");

  TelnetStream.println("\n-------------------------------------------------------");
  TelnetStream.println("// 3. القيمة المثالية لمتغير lineThreshold في ملف LineFollower.ino هي:");
  TelnetStream.printf("int lineThreshold = %d;\n", calculatedThreshold);
  TelnetStream.println("=======================================================");
}