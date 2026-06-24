void loopSensors() {
// 1. قراءة جميع الحساسات باستخدام فلتر (متوسط 3 قراءات)
  for (int i = 0; i < 10; i++) {
    long tempSum = 0; // متغير مؤقت لتخزين مجموع القراءات
    // أخذ 3 قراءات متتالية لنفس الحساس
    for (int j = 0; j < 3; j++) {
      tempSum += analogRead(sensorPins[i]);
    }
    // حساب المتوسط وتخزينه كقراءة نهائية للحساس
    sensorValue[i] = tempSum / 3;
  }
  
  for (int i = 0; i < 10; i++) {
    // المعايرة الخطية الفردية لكل حساس ليطابق المرجع المثالي
    sensorValue[i] = map(sensorValue[i], S_White[i], S_Black[i], target_White, target_Black);
    
    // حماية القيم
    sensorValue[i] = constrain(sensorValue[i], 0, 4095);
  }
  
  // =========================================================
  // --- [بداية كود اكتشاف التبديل الشفاف للألوان] ---
  // =========================================================
  /*
  int currentBlackCount = 0;
  for (int i = 0; i < 10; i++) {
    // نعد الحساسات التي ترى اللون الأسود "الفيزيائي" على الأرضية
    if (sensorValue[i] > lineThreshold) {
      currentBlackCount++;
    }
  }

  // 1. اكتشاف الدخول في المنطقة المعكوسة (الخلفية تصبح سوداء فجأة)
  // إذا رأينا 8 حساسات أو أكثر تقرأ لوناً أسوداً
  if (!isInverted && currentBlackCount >= 7 && currentBlackCount != 10) {
    inversionCounterBlack++;
    if (inversionCounterBlack > INVERSION_THRESH) {
      isInverted = true;
      inversionCounterBlack = 0; 
      // اختياري: إطلاق نغمة سريعة للتأكيد أثناء السباق
      tone(buzzerPin, 3000, 90); 
    }
  } 
  // 2. اكتشاف الخروج والعودة للوضع الطبيعي (الخلفية تعود بيضاء)
  // إذا رأينا حساسين أو أقل يقرأون اللون الأسود (وهو الخط الطبيعي)
  else if (isInverted && currentBlackCount <= 2) {
    inversionCounterWhite++;
    if (inversionCounterWhite > (INVERSION_THRESH / 3)) {
      isInverted = false;
      inversionCounterWhite = 0;
      tone(buzzerPin, 3000, 90);
    }
  } 
  // 3. تصفير العداد إذا كانت الحالة مؤقتة
  // هذا السطر يحمي الروبوت من التقاطعات (+) العادية، حيث سيقرأ 10 حساسات أسود
  // ولكن لفترة قصيرة جداً لا تتجاوز الـ INVERSION_THRESH فيصفر العداد.
  else {
    inversionCounterBlack = 0;
    inversionCounterWhite = 0;
  }

  // 4. الخدعة الرياضية: عكس القيم إذا كنا في وضع التبديل
  if (isInverted) {
    for (int i = 0; i < 10; i++) {
      // معادلة العكس السحرية
      sensorValue[i] = (target_Black + target_White) - sensorValue[i];
    }
  }
  */
  // =========================================================
  // --- [نهاية كود التبديل] ---
  // =========================================================

  for (int i = 0; i < 10; i++) {
    if (sensorValue[i] > lineThreshold) {
      bitSet(sensorBit, 9 - i);
    } else {
      bitClear(sensorBit, 9 - i);
    }
  }

  // إزاحة البتات 6 خطوات لليمين لاستخراج بتات الحساسات اليسرى (من 6 إلى 11) وعدّها
  leftSensor = __builtin_popcount((sensorBit >> 5) & 0x1F); 

  // استخراج أول 6 بتات (من 0 إلى 5) الخاصة بالحساسات اليمنى وعدّها بسرعة
  rightSensor = __builtin_popcount(sensorBit & 0x1F); // 0x3F تعادل 000000111111 ثنائياً

  // حساسات المنتصف
  midSensor = __builtin_popcount((sensorBit >> 1) & 0xFF);

  // حساسات المنتصف
  midMidSensor = __builtin_popcount((sensorBit >> 2) & 0x3F);

  // إجمالي الحساسات
  allSensor = __builtin_popcount(sensorBit & 0x3FF);

  leftRadar = bitRead(sensorBit, 9);
  rightRadar = bitRead(sensorBit, 0);
  //radar = leftRadar + rightRadar;

  leftMidRadar = bitRead(sensorBit, 8);
  rightMidRadar = bitRead(sensorBit, 1);

if (leftRadar){
    leftRadarOn = true;
    leftRadarStartDistance = totalOdometer; // تسجيل المسافة الحالية كنقطة بداية
  } else { 
    // إذا اختفى الخط عن الرادار، نتحقق مما إذا كان الروبوت قد قطع المسافة المحددة (5 سم)
    if ((totalOdometer - leftRadarStartDistance) > RadarDistanceThreshold) {
        leftRadarOn = false;
    }
  } 
  
  if (rightRadar){
    rightRadarOn = true;
    rightRadarStartDistance = totalOdometer; // تسجيل المسافة الحالية كنقطة بداية
  } else { 
    if ((totalOdometer - rightRadarStartDistance) > RadarDistanceThreshold) {
        rightRadarOn = false;
    }
  }

  if (leftMidRadar){
    leftMidRadarOn = true;
    leftMidRadarStartDistance = totalOdometer;
  } else { 
    if ((totalOdometer - leftMidRadarStartDistance) > RadarDistanceThreshold) {
        leftMidRadarOn = false;
    }
  } 
  
  if (rightMidRadar){
    rightMidRadarOn = true;
    rightMidRadarStartDistance = totalOdometer;
  } else { 
    if ((totalOdometer - rightMidRadarStartDistance) > RadarDistanceThreshold) {
        rightMidRadarOn = false;
    }
  }

}


