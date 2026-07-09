// ====================================================================
// ملف قراءة الحساسات، فلترتها، وتحديث متغيرات الرادارات (Sensors.ino)
// ====================================================================

void loopSensors() {
  
  // =========================================================
  // 1. مرحلة قراءة الحساسات والفلترة (Oversampling Filter)
  // =========================================================
  for (int i = 0; i < 12; i++) {
    long tempSum = 0;
    for (int j = 0; j < 3; j++) {
      tempSum += analogRead(sensorPins[i]);
    }
    sensorValue[i] = tempSum / 3;
  }
  //sensorValue[6] = 0;
  
  // =========================================================
  // 2. مرحلة المعايرة الخطية وتقييد القيم (Linear Mapping)
  // =========================================================
  for (int i = 0; i < 12; i++) {
    sensorValue[i] = map(sensorValue[i], S_White[i], S_Black[i], target_White, target_Black);
    sensorValue[i] = constrain(sensorValue[i], 0, 4095);
  }
  
  // =========================================================
  // 3. مرحلة اكتشاف التبديل الشفاف للألوان (Inversion Zone)
  // =========================================================
  /*
  int currentBlackCount = 0;
  // حساب عدد الحساسات التي ترى لوناً "أسود فيزيائي" حالياً
  for (int i = 0; i < 12; i++) {
    if (sensorValue[i] > lineThreshold) {
      currentBlackCount++;
    }
  }

  // أ. اكتشاف الدخول في المنطقة المعكوسة (الخلفية أصبحت سوداء والخط أبيض)
  // الشرط != 12 هو لحماية الروبوت من التقاطعات العادية (+) التي تعطي 12 أسود مؤقتاً
  if (!isInverted && currentBlackCount >= 9 && currentBlackCount != 12) {
    inversionCounterBlack++; // زيادة عداد التأكيد
    
    if (inversionCounterBlack > INVERSION_THRESH) {
      isInverted = true; // تأكيد الدخول في المنطقة المعكوسة
      inversionCounterBlack = 0; // تصفير العداد
      tone(buzzerPin, 3000, 90); // إطلاق نغمة سريعة لتأكيد التبديل
    }
  } 
  
  // ب. اكتشاف الخروج من المنطقة المعكوسة (العودة للوضع الطبيعي)
  // إذا كنا في وضع معكوس ورأينا 3 حساسات أو أقل باللون الأسود الفيزيائي
  else if (isInverted && currentBlackCount <= 3) {
    inversionCounterWhite++; // زيادة عداد التأكيد للعودة
    
    if (inversionCounterWhite > (INVERSION_THRESH / 3)) { // استجابة العودة أسرع بـ 3 مرات
      isInverted = false; // تأكيد العودة للوضع الطبيعي
      inversionCounterWhite = 0; // تصفير العداد
      tone(buzzerPin, 3000, 90); // إطلاق نغمة التبديل
    }
  } 
  
  // ج. تصفير العدادات إذا كانت الحالات مؤقتة (مثل المرور فوق أوساخ أو تقاطع)
  else {
    inversionCounterBlack = 0;
    inversionCounterWhite = 0;
  }

  // د. الخدعة الرياضية: عكس القيم فعلياً إذا تم تأكيد وضع التبديل
  if (isInverted) {
    for (int i = 0; i < 12; i++) {
      // معادلة العكس السحرية: تحويل الأسود لأبيض والأبيض لأسود رياضياً
      sensorValue[i] = (target_Black + target_White) - sensorValue[i];
    }
  }
  */
  

  // =========================================================
  // 4. مرحلة التشفير الثنائي (Bitmasking) للحساسات
  // =========================================================
  for (int i = 0; i < 12; i++) {
    if (sensorValue[i] > lineThreshold) {
      bitSet(sensorBit, 11 - i); 
    } else {
      bitClear(sensorBit, 11 - i);
    }
  }

  // =========================================================
  // 5. استخراج البيانات وعدّ الحساسات النشطة (Popcount Logic)
  // =========================================================
  
  // leftSensor = S1 إلى S6 (أول 6 حساسات يسار)
  leftSensor = __builtin_popcount((sensorBit >> 6) & 0x3F);
  // rightSensor = S7 إلى S12 (أول 6 حساسات يمين)
  rightSensor = __builtin_popcount(sensorBit & 0x3F);
  
  // midSensor = S3 إلى S10 (البتات من 9 إلى 2) (8 حساسات)
  midSensor = __builtin_popcount((sensorBit >> 2) & 0xFF);
  
  // midMidSensor = S4 إلى S9 (البتات من 8 إلى 3) (6 حساسات)
  midMidSensor = __builtin_popcount((sensorBit >> 3) & 0x3F);
  
  // midMidMidSensor = S5 إلى S8 (البتات من 7 إلى 4) (4 حساسات مركزية)
  midMidMidSensor = __builtin_popcount((sensorBit >> 4) & 0x0F);
  //midMidMidSensor = bitRead(sensorBit, 4) + bitRead(sensorBit, 6) + bitRead(sensorBit, 7);

  // إجمالي الحساسات النشطة (12 بت)
  allSensor = __builtin_popcount(sensorBit & 0xFFF);

  // =========================================================
  // 6. تخصيص قراءات الرادارات الموضعية
  // =========================================================
  leftOutRadar   = bitRead(sensorBit, 11); // S1
  leftRadar    = bitRead(sensorBit, 10); // S2
  leftMidRadar   = bitRead(sensorBit, 9);  // S3 
  rightMidRadar  = bitRead(sensorBit, 2);  // S10 
  rightRadar   = bitRead(sensorBit, 1);  // S11
  rightOutRadar  = bitRead(sensorBit, 0); // S12

  // =========================================================
  // 7. خوارزمية الذاكرة الزمنية للرادارات (Radar Time Memory) - مطورة
 // =========================================================
  
  // --- رادار أقصى اليسار (S1) ---
  if (leftOutRadar) { //
    leftOutRadarOn = true; //
    leftOutRadarStartTime = millis(); //
  } else if (leftOutRadarOn) { //
    unsigned long elapsedTime = millis() - leftOutRadarStartTime; //
    if (elapsedTime >= radarMaxTime) { //
      leftOutRadarOn = false; //
    } 
    else if (elapsedTime >= radarMinTime) { //
      if (rightOutRadarOn == true) { //
        leftOutRadarOn = false; //
      }
    }
  } 
  
  // --- رادار أقصى اليمين (S12) ---
  if (rightOutRadar) { //
    rightOutRadarOn = true; //
    rightOutRadarStartTime = millis(); //
  } else if (rightOutRadarOn) { //
    unsigned long elapsedTime = millis() - rightOutRadarStartTime; //
    if (elapsedTime >= radarMaxTime) { //
      rightOutRadarOn = false; //
    } 
    else if (elapsedTime >= radarMinTime) { //
      if (leftOutRadarOn == true) { //
        rightOutRadarOn = false; //
      }
    }
  }

  // --- رادار أقصى اليسار (S2) ---
  if (leftRadar) {
    leftRadarOn = true;
    leftRadarStartTime = millis(); // تحديث نقطة البداية طالما الرادار يرى الخط
  } else if (leftRadarOn) { 
    // إذا لم يعد يرى الخط، نحسب الوقت المنقضي
    unsigned long elapsedTime = millis() - leftRadarStartTime;
    
    if (elapsedTime >= radarMaxTime) {
      // إذا وصلنا للحد الأقصى المطلق (650)، نطفئه فوراً
      leftRadarOn = false;
    } 
    else if (elapsedTime >= radarMinTime) {
      // إذا تجاوزنا الحد الأدنى (200)، نتحقق من الرادار المعاكس (أقصى اليمين)
      if (rightRadarOn == true) {
        leftRadarOn = false; // الرادار الآخر يعمل، إذن نطفئ هذا الرادار
      }
      // إذا كان الرادار الآخر لا يعمل، سيبقى هذا الرادار يعمل حتى يصل لـ 650
    }
  } 
  
  // --- رادار أقصى اليمين (S11) ---
  if (rightRadar) {
    rightRadarOn = true;
    rightRadarStartTime = millis(); 
  } else if (rightRadarOn) {
    unsigned long elapsedTime = millis() - rightRadarStartTime;
    
    if (elapsedTime >= radarMaxTime) {
      rightRadarOn = false;
    } 
    else if (elapsedTime >= radarMinTime) {
      if (leftRadarOn == true) {
        rightRadarOn = false;
      }
    }
  }

  // --- رادار اليسار الداخلي (S3) ---
  if (leftMidRadar) { //
    leftMidRadarOn = true; //
    leftMidRadarStartTime = millis(); //
  } else if (leftMidRadarOn) { //
    unsigned long elapsedTime = millis() - leftMidRadarStartTime; //
    if (elapsedTime >= radarMaxTime) { //
      leftMidRadarOn = false; //
    } 
    else if (elapsedTime >= radarMinTime) { //
      if (rightMidRadarOn == true) { //
        leftMidRadarOn = false; //
      }
    }
  } 
  
  // --- رادار اليمين الداخلي (S10) ---
  if (rightMidRadar) { //
    rightMidRadarOn = true; //
    rightMidRadarStartTime = millis(); //
  } else if (rightMidRadarOn) { //
    unsigned long elapsedTime = millis() - rightMidRadarStartTime; //
    if (elapsedTime >= radarMaxTime) { //
      rightMidRadarOn = false; //
    } 
    else if (elapsedTime >= radarMinTime) { // //
      if (leftMidRadarOn == true) { //
        rightMidRadarOn = false; //
      }
    }
  }

 // =========================================================
  // 8. خوارزمية الذاكرة للشرط المخصص (Special Condition Memory)
 // =========================================================
  if ( (leftMidRadar == 1) && (bitRead(sensorBit, 8) == 1) && (bitRead(sensorBit, 7) == 1) &&  (bitRead(sensorBit, 6) == 1) &&
       (bitRead(sensorBit, 3) == 0) && (rightMidRadar == 0) && (rightOutRadar == 0) ){ //
    specialMemory = true; //
    specialMemoryStartTime = millis(); //
  } else { 
    if (specialMemory && ((millis() - specialMemoryStartTime) > 20)) { //
        specialMemory = false; //
    }
  }

  // --- رادار أقصى اليسار (S1) ---
  if (leftOutRadar) { //
    leftOutRadarOn2 = true; //
    leftOutRadarStartTime2 = millis(); //
  } else { 
    if (((millis() - leftOutRadarStartTime2) > 50) && leftOutRadarOn2) { //
        leftOutRadarOn2 = false; //
    }
  }
  checkStateChanges();
}




