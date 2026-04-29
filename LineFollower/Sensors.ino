void loopSensors() {
// 1. قراءة جميع الحساسات باستخدام فلتر (متوسط 3 قراءات)
  for (int i = 0; i < 12; i++) {
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
    
    // حماية القيم (استخدام 50 كحد أدنى كما طلبت)
    sensorValue[i] = constrain(sensorValue[i], 50, 4095);
  }

  for (int i = 0; i < 12; i++) {
    if (sensorValue[i] > lineThreshold) {
      bitSet(sensorBit, 11 - i);
    } else {
      bitClear(sensorBit, 11 - i);
    }
  }

  // إزاحة البتات 6 خطوات لليمين لاستخراج بتات الحساسات اليسرى (من 6 إلى 11) وعدّها
  leftSensor = __builtin_popcount((sensorBit >> 6) & 0x3F); 

  // استخراج أول 6 بتات (من 0 إلى 5) الخاصة بالحساسات اليمنى وعدّها بسرعة
  rightSensor = __builtin_popcount(sensorBit & 0x3F); // 0x3F تعادل 000000111111 ثنائياً

  // حساسات المنتصف
  midSensor = __builtin_popcount((sensorBit >> 2) & 0xFF);

  // إجمالي الحساسات
  allSensor = __builtin_popcount(sensorBit & 0xFFF);

  // حساسات اليسار: إزاحة 10 خطوات لليمين لاقتناص بتات اليسار
  leftRadar = __builtin_popcount((sensorBit >> 10) & 0x03);

  // حساسات اليمين: 0x03 تعادل (11) ثنائياً لاقتناص أول بتين من اليمين
  rightRadar = __builtin_popcount(sensorBit & 0x03);

  radar = leftRadar + rightRadar;

  if (leftRadar == 2){
    leftRadarOn = true;
    leftRadarTime = millis();
  } else { 
    if (millis() - leftRadarTime > RadarTime) leftRadarOn = false;
  } 
  if (rightRadar == 2){
    rightRadarOn = true;
    rightRadarTime = millis();
  } else { 
    if (millis() - rightRadarTime > RadarTime) rightRadarOn = false;
  } 
}

