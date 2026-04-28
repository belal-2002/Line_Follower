void loopSensors() {
  // 1. قراءة جميع الحساسات
  for (int i = 0; i < 12; i++) {
    sensorValue[i] = analogRead(sensorPins[i]);
  }


  // 1. ضع هذه المتغيرات في ملف Sensors.ino (خارج الدوال أو داخل دالة loopSensors):
  int S2_White = 220, S2_Black = 3258;
  int S11_White = 258, S11_Black = 3617;
  int target_White = 174, target_Black = 2588;
  // 2. استبدل كود المعايرة القديم بهذا الكود:
  // معايرة الحساس رقم 2
  sensorValue[1] = map(sensorValue[1], S2_White, S2_Black, target_White, target_Black);
  sensorValue[1] = constrain(sensorValue[1], 50, 4095);

  // معايرة الحساس رقم 11
  sensorValue[10] = map(sensorValue[10], S11_White, S11_Black, target_White, target_Black);
  sensorValue[10] = constrain(sensorValue[10], 50, 4095);


  /*
  // معايرة الحساس رقم 2
  sensorValue[1] = map(sensorValue[1], 223, 3261, 171, 2378);
  sensorValue[1] = constrain(sensorValue[1], 127, 4095); // حماية القراءة من النزول تحت الصفر

  // معايرة الحساس رقم 11
  sensorValue[10] = map(sensorValue[10], 240, 3546, 171, 2378);
  sensorValue[10] = constrain(sensorValue[10], 127, 4095); // حماية القراءة من النزول تحت الصفر
  */

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

