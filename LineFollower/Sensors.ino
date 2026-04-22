void loopSensors() {
  // 1. قراءة جميع الحساسات
  for (int i = 0; i < 12; i++) {
    sensorValue[i] = analogRead(sensorPins[i]);
  }
  sensorValue[1] -= 70;
  sensorValue[10] -= 170;

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

  if (leftRadar){
    leftRadarOn = true;
    leftRadarTime = millis();
  } else { 
    if (millis() - leftRadarTime > RadarTime) leftRadarOn = false;
  } 
  if (rightRadar){
    rightRadarOn = true;
    rightRadarTime = millis();
  } else { 
    if (millis() - rightRadarTime > RadarTime) rightRadarOn = false;
  } 
}

