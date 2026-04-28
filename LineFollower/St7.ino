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

  // اكتشاف لحظة الضغط على الزر (انتقال من غير مضغوط إلى مضغوط)
  if (currentButtonState && !wasButtonPressed) {
    delay(50); // Debounce بسيط لمنع القراءات المتكررة للضغطة الواحدة
    
    // إذا كان الزر لا يزال مضغوطاً بعد التأخير
    if (digitalRead(limitSwitch) == LOW) {
      
      if (calibState == 0) {
        TelnetStream.println("\n=== بدء معايرة الخط الأبيض ===");
        runCalibrationPhase(false); 
        calibState = 1;
        TelnetStream.println("--> ضع الروبوت على الأسود واضغط الزر...");
      } 
      else if (calibState == 1) {
        TelnetStream.println("\n=== بدء معايرة الخط الأسود ===");
        runCalibrationPhase(true);  
        finalizeCalibration();      
        calibState = 2;
        TelnetStream.println("\n--- تمت المعايرة. لتكرارها اضغط الزر، أو قم بتغيير الاستراتيجية للانطلاق ---");
      } 
      else if (calibState == 2) {
        TelnetStream.println("\n--- إعادة تعيين النظام. ضع الروبوت على الأبيض واضغط الزر ---");
        calibState = 0;
      }
    }
  }
  
  wasButtonPressed = currentButtonState;

  // إبقاء الاتصال نشطاً لمنع انقطاع الـ Telnet
  if (TelnetStream.available()) { TelnetStream.read(); }
}

void runCalibrationPhase(bool isBlack) {
  long sums[12] = {0};
  
  TelnetStream.println("جاري أخذ 100 قراءة...");
  
  for (int i = 0; i < 100; i++) {
    for (int s = 0; s < 12; s++) {
      int val = analogRead(sensorPins[s]);
      sums[s] += val;
    }
    delay(10); // ديلي 10 ملي ثانية (إجمالي 1 ثانية تقريباً)
  }

  TelnetStream.println("--- متوسط الحساسات ---");
  float overallSum = 0;
  
  for (int s = 0; s < 12; s++) {
    float avg = (float)sums[s] / 100.0;
    
    if (isBlack) {
      blackAvg[s] = avg;
    } else {
      whiteAvg[s] = avg;
    }
    
    TelnetStream.print("S"); TelnetStream.print(s+1); TelnetStream.print(":"); TelnetStream.print(avg, 1); TelnetStream.print("\t");
    
    // استثناء S2 (index 1) و S11 (index 10) من المتوسط الكلي
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
  
  TelnetStream.print("المتوسط الكلي للمرجع: ");
  TelnetStream.println(overall);
}

void finalizeCalibration() {
  int calculatedThreshold = round((overallWhiteAvg + overallBlackAvg) / 2.0);
  
  // تحديث المتغيرات العامة (الموجودة في LineFollower.ino أو Sensors.ino)
  target_White = round(overallWhiteAvg);
  target_Black = round(overallBlackAvg);
  lineThreshold = calculatedThreshold;
  
  for (int i = 0; i < 12; i++) {
    S_White[i] = round(whiteAvg[i]);
    S_Black[i] = round(blackAvg[i]);
  }

  TelnetStream.println("\n=======================================================");
  TelnetStream.println("🎉 تمت المعايرة! تم تحديث المتغيرات بنجاح.");
  TelnetStream.printf("الهدف الأبيض: %d | الهدف الأسود: %d\n", target_White, target_Black);
  TelnetStream.printf("حد الخط (Threshold) الجديد: %d\n", lineThreshold);
  TelnetStream.println("=======================================================");
}