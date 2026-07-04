// ====================================================================
// ملف الطباعة اللاسلكية والتعديل الحي (Print.ino)
// ====================================================================

void loopPrint() {
  static unsigned long lastPrintTime = 0;

  // 1. الاستماع الدائم لتحديثات البرمجة عبر الهواء (OTA)
  ArduinoOTA.handle(); 

  // =================================================================
  // 2. معالجة أوامر لوحة المفاتيح (التعديل الحي للـ PID والسرعات)
  // =================================================================
  if (TelnetStream.available()) {
    char c = TelnetStream.read();
    
    // تعديل قيم التحكم (PID)
    if (c == 'q') Kp += 0.1;
    if (c == 'a') Kp -= 0.1;
    if (c == 'w') Kd += 0.1;
    if (c == 's') Kd -= 0.1;
    
    // تعديل السرعات
    if (c == 'e') maximumSpeed += 25;
    if (c == 'd') maximumSpeed -= 25;
    if (c == 'r') baseSpeed += 25;
    if (c == 'f') baseSpeed -= 25;
    if (c == 't') { turnSpeed += 25; innerTurnSpeed = turnSpeed * 0.73; }
    if (c == 'g') { turnSpeed -= 25; innerTurnSpeed = turnSpeed * 0.73; }
    
    // تغيير الاستراتيجية يدوياً (تحويل الحرف المدخل مباشرة إلى رقم الاستراتيجية)
    if (c >= '0' && c <= '7') {
      strategy = c - '0'; 
    }

    // حماية القيم من الانخفاض تحت الصفر (حتى لا تعكس المحركات اتجاهها بالخطأ)
    if (Kp < 0) Kp = 0;
    if (Kd < 0) Kd = 0;
    if (baseSpeed < 0) baseSpeed = 0;
    if (turnSpeed < 0) turnSpeed = 0;
    if (innerTurnSpeed < 0) innerTurnSpeed = 0;
    if (maximumSpeed < 0) maximumSpeed = 0;
  }

  // =================================================================
  // 3. طباعة البيانات (لوحة المراقبة - Dashboard)
  // =================================================================
  
  // إذا لم نكن في وضع المعايرة (لأن الاستراتيجية 0 تتولى طباعة بياناتها بنفسها)
  if (strategy != 0) {
    if (millis() - lastPrintTime > 1500) { // الطباعة كل ثانية ونصف
      
      /*TelnetStream.println("\n-------------------------------------------------");
      TelnetStream.println();
      
      TelnetStream.printf("Kp: %.2f  | Kd: %.1f  | Strategy: %d", Kp, Kd, strategy);
      TelnetStream.printf("  | Max: %d   | Base: %d  | Turn: %d", maximumSpeed, baseSpeed, turnSpeed);
      TelnetStream.printf("  | Dist: %.2f cm", distanceNow);
      TelnetStream.printf("  | Left: %ld   | Right: %ld", currentLeftTicks, currentRightTicks);*/

      for (int i = 0; i < 10; i++) {
        TelnetStream.print(sensorValue[i]);  
        TelnetStream.print("\t");               
      }
      TelnetStream.print("midMidMidSensor: ");
      TelnetStream.println(midMidMidSensor);



      lastPrintTime = millis(); // تصفير العداد الزمني للطباعة
    }  



    /*static bool LeftRadar = false;
    static bool RightRadar = false;
    static unsigned long lastLeftChange = 0;
    static unsigned long lastRightChange = 0;
    
    unsigned long currentTime = millis();
    
    if (leftMidRadarOn != LeftRadar) {
      unsigned long duration = currentTime - lastLeftChange;
      TelnetStream.print("LeftRadar: ");
      TelnetStream.print(leftMidRadarOn ? "ON" : "OFF");
      TelnetStream.print(" | Time elapsed: ");
      TelnetStream.print(duration);
      TelnetStream.println(" ms");
      LeftRadar = leftMidRadarOn;
      lastLeftChange = currentTime;
    }
    if (rightMidRadarOn != RightRadar) {
      unsigned long duration = currentTime - lastRightChange;
      TelnetStream.print("RightRadar: ");
      TelnetStream.print(rightMidRadarOn ? "ON" : "OFF"); 
      TelnetStream.print(" | Time elapsed: ");
      TelnetStream.print(duration);
      TelnetStream.println(" ms");
      RightRadar = rightMidRadarOn;
      lastRightChange = currentTime;
    }*/


  } 
  else {
    // إذا كنا في وضع المعايرة (الاستراتيجية 0)، نطبع تذكير بسيط كل 3.5 ثانية
    if (millis() - lastPrintTime > 3500) {
      TelnetStream.println("⚠️ الروبوت في وضع المعايرة (Strategy 0) - راجع التعليمات بالأعلى...");
      lastPrintTime = millis();
    }
  }
}

// =================================================================
// دالة مخصصة لمراقبة وتتبع تغيرات متغيرات الرادار والتوجيه
// =================================================================
void checkStateChanges() {
  // 1. تعريف الذاكرة (استخدام static يجعل المتغيرات تحتفظ بقيمتها بين كل دورة)
  static bool prevleftRadarOn = 0;
  static bool prevrightRadarOn = 0;
  static bool prevleftMidRadarOn = 0;
  static bool prevrightMidRadarOn = 0;
  static bool prevGoLeft = false;
  static bool prevGoRight = false;
  
  // متغير لحفظ وقت آخر رسالة تم طباعتها من هذه الدالة فقط
  static unsigned long lastStateChangePrintTime = millis(); 

  // 2. التحقق مما إذا كان أي من المتغيرات الأربعة قد تغيرت قيمته
  if (leftRadarOn != prevleftRadarOn ||
      rightRadarOn != prevrightRadarOn ||
      leftMidRadarOn != prevleftMidRadarOn ||
      rightMidRadarOn != prevrightMidRadarOn ||
      goLeft != prevGoLeft ||
      goRight != prevGoRight) {

      // 4. فحص أي المتغيرات تغير بالضبط وطباعته
      if (leftRadarOn != prevleftRadarOn) {
        TelnetStream.print(" - leftRadarOn: "); 
        TelnetStream.print(leftRadarOn);
      }
      if (rightRadarOn != prevrightRadarOn) {
        TelnetStream.print(" - rightRadarOn: "); 
        TelnetStream.print(rightRadarOn);
      }
      //////////////////////////////////
      if (leftMidRadarOn != prevleftMidRadarOn) {
        TelnetStream.print(" - leftMidRadarOn: "); 
        TelnetStream.print(leftMidRadarOn);
      }
      if (rightMidRadarOn != prevrightMidRadarOn) {
        TelnetStream.print(" - rightMidRadarOn: "); 
        TelnetStream.print(rightMidRadarOn);
      }
      ////////////////////////////////
      if (goLeft != prevGoLeft) {
        TelnetStream.print(" - goLeft: "); 
        TelnetStream.print(goLeft ? "True (1)" : "False (0)");
      }
      if (goRight != prevGoRight) {
        TelnetStream.print(" - goRight: "); 
        TelnetStream.print(goRight ? "True (1)" : "False (0)");
      }
      
      // حساب الوقت الحالي وفرق الوقت عن آخر رسالة
      unsigned long currentTime = millis();
      unsigned long timeDiff = currentTime - lastStateChangePrintTime;

      // 5. طباعة قيمة midMidMidSensor مع كل رسالة تغيير كما طلبت
      TelnetStream.print(" | midMidMidSensor: ");
      TelnetStream.print(midMidMidSensor);
      TelnetStream.print(" | Time elapsed: ");
      TelnetStream.println(timeDiff);

      // 6. تحديث الذاكرة بالقيم الجديدة لانتظار التغيير القادم
      prevleftRadarOn = leftRadarOn;
      prevrightRadarOn = rightRadarOn;
      prevleftMidRadarOn = leftMidRadarOn;
      prevrightMidRadarOn = rightMidRadarOn;
      prevGoLeft = goLeft;
      prevGoRight = goRight;
      
      // تحديث وقت آخر طباعة
      lastStateChangePrintTime = currentTime;
  }
}





