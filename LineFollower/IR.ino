// ====================================================================
// ملف قراءة الحساسات، فلترتها، وتحديث متغيرات الرادارات (Sensors.ino)
// ====================================================================

void loopSensors() {
  
  // =========================================================
  // 1. مرحلة قراءة الحساسات والفلترة (Oversampling Filter)
  // =========================================================
  for (int i = 0; i < 10; i++) {
    long tempSum = 0; // متغير مؤقت لتجميع القراءات
    
    // أخذ 3 قراءات متتالية لنفس الحساس في نفس اللحظة
    for (int j = 0; j < 3; j++) {
      tempSum += analogRead(sensorPins[i]);
    }
    
    // حساب المتوسط وتخزينه كقراءة نهائية لتقليل التشويش (Noise)
    sensorValue[i] = tempSum / 3;
  }
  
  // =========================================================
  // 2. مرحلة المعايرة الخطية وتقييد القيم (Linear Mapping)
  // =========================================================
  for (int i = 0; i < 10; i++) {
    // توحيد القراءات: تحويل قراءة كل حساس بناءً على مرجعه الخاص ليطابق المرجع المثالي
    sensorValue[i] = map(sensorValue[i], S_White[i], S_Black[i], target_White, target_Black);
    
    // حماية النظام: تقييد القيم بين 0 و 4095 (دقة 12-بت لـ ESP32) لمنع القيم الشاذة
    sensorValue[i] = constrain(sensorValue[i], 0, 4095);
  }
  
  // =========================================================
  // 3. مرحلة اكتشاف التبديل الشفاف للألوان (Inversion Zone)
  // =========================================================
  /*
  int currentBlackCount = 0;
  
  // حساب عدد الحساسات التي ترى لوناً "أسود فيزيائي" حالياً
  for (int i = 0; i < 10; i++) {
    if (sensorValue[i] > lineThreshold) {
      currentBlackCount++;
    }
  }

  // أ. اكتشاف الدخول في المنطقة المعكوسة (الخلفية أصبحت سوداء والخط أبيض)
  // الشرط != 10 هو لحماية الروبوت من التقاطعات العادية (+) التي تعطي 10 أسود مؤقتاً
  if (!isInverted && currentBlackCount >= 7 && currentBlackCount != 10) {
    inversionCounterBlack++; // زيادة عداد التأكيد
    
    if (inversionCounterBlack > INVERSION_THRESH) {
      isInverted = true;           // تأكيد الدخول في المنطقة المعكوسة
      inversionCounterBlack = 0;   // تصفير العداد
      tone(buzzerPin, 3000, 90);   // إطلاق نغمة سريعة لتأكيد التبديل
    }
  } 
  
  // ب. اكتشاف الخروج من المنطقة المعكوسة (العودة للوضع الطبيعي)
  // إذا كنا في وضع معكوس ورأينا حساسين أو أقل باللون الأسود الفيزيائي
  else if (isInverted && currentBlackCount <= 2) {
    inversionCounterWhite++; // زيادة عداد التأكيد للعودة
    
    if (inversionCounterWhite > (INVERSION_THRESH / 3)) { // استجابة العودة أسرع بـ 3 مرات
      isInverted = false;          // تأكيد العودة للوضع الطبيعي
      inversionCounterWhite = 0;   // تصفير العداد
      tone(buzzerPin, 3000, 90);   // إطلاق نغمة التبديل
    }
  } 
  
  // ج. تصفير العدادات إذا كانت الحالات مؤقتة (مثل المرور فوق أوساخ أو تقاطع)
  else {
    inversionCounterBlack = 0;
    inversionCounterWhite = 0;
  }

  // د. الخدعة الرياضية: عكس القيم فعلياً إذا تم تأكيد وضع التبديل
  if (isInverted) {
    for (int i = 0; i < 10; i++) {
      // معادلة العكس السحرية: تحويل الأسود لأبيض والأبيض لأسود رياضياً
      sensorValue[i] = (target_Black + target_White) - sensorValue[i];
    }
  }
  */

  // =========================================================
  // 4. مرحلة التشفير الثنائي (Bitmasking) للحساسات
  // =========================================================
  for (int i = 0; i < 10; i++) {
    if (sensorValue[i] > lineThreshold) {
      bitSet(sensorBit, 9 - i);  // تحويل القراءة إلى 1 إذا كانت فوق العتبة
    } else {
      bitClear(sensorBit, 9 - i); // تحويل القراءة إلى 0 إذا كانت تحت العتبة
    }
  }

  // =========================================================
  // 5. استخراج البيانات وعدّ الحساسات النشطة (Popcount Logic)
  // =========================================================
  
  // إزاحة البتات 5 خطوات لليمين لاستخراج بتات الحساسات اليسرى الـ 5 وعدّها
  leftSensor = __builtin_popcount((sensorBit >> 5) & 0x1F); 

  // استخراج أول 5 بتات للحساسات اليمنى وعدّها بسرعة (0x1F تعادل 11111 ثنائياً)
  rightSensor = __builtin_popcount(sensorBit & 0x1F); 

  // حساسات المنتصف الـ 8 (تجاهل S1 و S10)
  midSensor = __builtin_popcount((sensorBit >> 1) & 0xFF);

  // حساسات عمق المنتصف الـ 6 (تجاهل S1, S2 و S9, S10) للتأكد من التمركز
  midMidSensor = __builtin_popcount((sensorBit >> 2) & 0x3F);

  // مركز المنتصف الكثيف جداً (البتات 3 و 4 و 5 و 6)
  midMidMidSensor = __builtin_popcount((sensorBit >> 3) & 0x0F);

  // إجمالي الحساسات النشطة
  allSensor = __builtin_popcount(sensorBit & 0x3FF);

  // =========================================================
  // 6. تخصيص قراءات الرادارات الموضعية
  // =========================================================
  leftRadar     = bitRead(sensorBit, 9); // S1
  leftMidRadar  = bitRead(sensorBit, 8); // S2
  rightMidRadar = bitRead(sensorBit, 1); // S9
  rightRadar    = bitRead(sensorBit, 0); // S10

  // =========================================================
  // 7. خوارزمية الذاكرة الزمنية للرادارات (Radar Time Memory)
  // =========================================================
  
  // --- رادار أقصى اليسار (S1) ---
  if (leftRadar) {
    leftRadarOn = true;
    leftRadarStartTime = millis(); // تحديث نقطة البداية الزمنية طالما الرادار يرى الخط
  } else { 
    // إذا اختفى الخط، نبقي الرادار مفعلاً كـ (ذاكرة) حتى ينقضي الزمن المسموح (RadarTimeThreshold)
    if ((millis() - leftRadarStartTime) > RadarTimeThreshold) {
        leftRadarOn = false;
    }
  } 
  
  // --- رادار أقصى اليمين (S10) ---
  if (rightRadar) {
    rightRadarOn = true;
    rightRadarStartTime = millis(); 
  } else { 
    if ((millis() - rightRadarStartTime) > RadarTimeThreshold) {
        rightRadarOn = false;
    }
  }

  // --- رادار اليسار الداخلي (S2) ---
  if (leftMidRadar) {
    leftMidRadarOn = true;
    leftMidRadarStartTime = millis();
  } else { 
    if ((millis() - leftMidRadarStartTime) > RadarTimeThreshold) {
        leftMidRadarOn = false;
    }
  } 
  
  // --- رادار اليمين الداخلي (S9) ---
  if (rightMidRadar) {
    rightMidRadarOn = true;
    rightMidRadarStartTime = millis();
  } else { 
    if ((millis() - rightMidRadarStartTime) > RadarTimeThreshold) {
        rightMidRadarOn = false;
    }
  }

  // =========================================================
  // 8. خوارزمية الذاكرة للشرط المخصص (Special Condition Memory)
  // =========================================================
  
  if ((leftMidRadar == 1) && (bitRead(sensorBit, 7) == 1) && 
      (rightMidRadar == 0) && (rightRadar == 0)) {
    specialMemory = true;
    specialMemoryStartTime = millis(); 
  } else { 
    if (specialMemory && ((millis() - specialMemoryStartTime) > 100)) {
        specialMemory = false;
    }
  }

  // --- رادار أقصى اليسار (S1) ---
  if (leftRadar) {
    //leftRadarOn2 = true;
    leftRadarStartTime2 = millis();
  } else { 
    if ((millis() - leftRadarStartTime2) > 100) {
        leftRadarOn2 = false;
    }
  }

}