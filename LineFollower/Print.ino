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
    if (c == 'w') Kd += 1.0;
    if (c == 's') Kd -= 1.0;
    
    // تعديل السرعات
    if (c == 'e') maximumSpeed += 25;
    if (c == 'd') maximumSpeed -= 25;
    if (c == 'r') baseSpeed += 25;
    if (c == 'f') baseSpeed -= 25;
    if (c == 't') { turnSpeed += 25; innerTurnSpeed = (turnSpeed / 5) * 4; }
    if (c == 'g') { turnSpeed -= 25; innerTurnSpeed = (turnSpeed / 5) * 4; }
    
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
      
      TelnetStream.println("\n-------------------------------------------------");
      
      // [القسم الأول]: طباعة متغيرات التحكم والسرعة والاستراتيجية
      TelnetStream.printf("⚙️ Control : Kp: %.2f  | Kd: %.1f  | Strategy: %d\n", Kp, Kd, strategy);
      TelnetStream.printf("🚀 Speeds  : Max: %d   | Base: %d  | Turn: %d\n", maximumSpeed, baseSpeed, turnSpeed);
      
      // [القسم الثاني]: طباعة متغيرات الملاحة والمسافات
      TelnetStream.printf("🧭 Nav     : Angle: %.1f | Dist: %.2f cm\n", pitchAngle, distanceNow);
      TelnetStream.printf("🔄 Encoders: Left: %ld   | Right: %ld\n", currentLeftTicks, currentRightTicks);

      // 💡 (قالب جاهز): إذا أردت يوماً رؤية الحساسات الـ 10 كرقم ثنائي، أزل علامة التعليق عن السطرين التاليين:
      // TelnetStream.print("👀 Sensors : ");
      // for(int i=0; i<10; i++) { TelnetStream.print(bitRead(sensorBit, 9-i)); } TelnetStream.println();
      
      TelnetStream.println("-------------------------------------------------");

      lastPrintTime = millis(); // تصفير العداد الزمني للطباعة
    }
    
  } 
  else {
    // إذا كنا في وضع المعايرة (الاستراتيجية 0)، نطبع تذكير بسيط كل 3.5 ثانية
    if (millis() - lastPrintTime > 3500) {
      TelnetStream.println("⚠️ الروبوت في وضع المعايرة (Strategy 0) - راجع التعليمات بالأعلى...");
      lastPrintTime = millis();
    }
  }
}