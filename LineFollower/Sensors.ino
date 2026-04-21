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

  // استخراج أول 6 بتات (من 0 إلى 5) الخاصة بالحساسات اليمنى وعدّها بسرعة
  rightSensor = __builtin_popcount(sensorBit & 0x3F); // 0x3F تعادل 000000111111 ثنائياً

  // إزاحة البتات 6 خطوات لليمين لاستخراج بتات الحساسات اليسرى (من 6 إلى 11) وعدّها
  leftSensor = __builtin_popcount((sensorBit >> 6) & 0x3F); 
  midSensor = __builtin_popcount((sensorBit >> 2) & 0xFF);

  // إجمالي الحساسات
  allSensor = __builtin_popcount(sensorBit & 0xFFF);

  leftRadar = bitRead(sensorBit, 0) + bitRead(sensorBit, 1);
  rightRadar = bitRead(sensorBit, 10) + bitRead(sensorBit, 11);
  radar = leftRadar + rightRadar;  
}

