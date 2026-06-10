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
  
  for (int i = 0; i < 12; i++) {
    // المعايرة الخطية الفردية لكل حساس ليطابق المرجع المثالي
    sensorValue[i] = map(sensorValue[i], S_White[i], S_Black[i], target_White, target_Black);
    
    // حماية القيم
    sensorValue[i] = constrain(sensorValue[i], 0, 4095);
  }
  

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
  radar = leftRadar + rightRadar;

  leftMidRadar = bitRead(sensorBit, 8);
  rightMidRadar = bitRead(sensorBit, 1);

if (leftRadar){
    leftRadarOn = true;
    leftRadarStartDistance = distanceNow; // تسجيل المسافة الحالية كنقطة بداية
  } else { 
    // إذا اختفى الخط عن الرادار، نتحقق مما إذا كان الروبوت قد قطع المسافة المحددة (5 سم)
    if ((distanceNow - leftRadarStartDistance) > RadarDistanceThreshold) {
        leftRadarOn = false;
    }
  } 
  
  if (rightRadar){
    rightRadarOn = true;
    rightRadarStartDistance = distanceNow; // تسجيل المسافة الحالية كنقطة بداية
  } else { 
    if ((distanceNow - rightRadarStartDistance) > RadarDistanceThreshold) {
        rightRadarOn = false;
    }
  }

  if (leftMidRadar){
    leftMidRadarOn = true;
    leftMidRadarStartDistance = distanceNow;
  } else { 
    if ((distanceNow - leftMidRadarStartDistance) > RadarDistanceThreshold) {
        leftMidRadarOn = false;
    }
  } 
  
  if (rightMidRadar){
    rightMidRadarOn = true;
    rightMidRadarStartDistance = distanceNow;
  } else { 
    if ((distanceNow - rightMidRadarStartDistance) > RadarDistanceThreshold) {
        rightMidRadarOn = false;
    }
  }

}


