void calculateError() {
  rightSensor = 0;
  leftSensor = 0;
  allSensor = 0;
  weightedSum = 0;
  sum = 0;

  // استخراج أول 6 بتات (من 0 إلى 5) الخاصة بالحساسات اليمنى وعدّها بسرعة
  rightSensor = __builtin_popcount(sensorBit & 0x3F); // 0x3F تعادل 000000111111 ثنائياً

  // إزاحة البتات 6 خطوات لليمين لاستخراج بتات الحساسات اليسرى (من 6 إلى 11) وعدّها
  leftSensor = __builtin_popcount((sensorBit >> 6) & 0x3F); 

  // إجمالي الحساسات
  allSensor = __builtin_popcount(sensorBit & 0xFFF);

  if (allSensor > 9) {  // السير في خط مستقيم لتجاوز التقاطع
    caseMotor = 3;
    lineWasFound = false;
    lineAvailable = false;
    return;
  }
  
  // لازم افحص بت معين
  if (sensorBit == 0b000000000000) 





  for (int i = 0; i < 12; i++) {
    if (bitRead(sensorBit, 11 - i)) {
      weightedSum += (long)sensorValue[i] * sensorWeights[i];
      sum += sensorValue[i];
    }
  }

  if (sum > 0) {
    currentError = (float)weightedSum / (float)sum;
    lineAvailable = true;
    caseMotor = 0;

    if (!lineWasFound) {
      lastTime = micros();       
      lastError = currentError;  
    }

    if (currentError > 0){
      goRight = true;
    } else if (currentError < 0) {
      goRight = false;
    }
  
  } else {
    lineAvailable = false; // الروبوت فقد الخط كلياً في المسار الطبيعي
    if (goRight){ // الدوران لليمين
      caseMotor = 1;
    } else { // الدوران لليسار
      caseMotor = 2;
    }
  }
  lineWasFound = lineAvailable;

  if (lineAvailable) calculatePD();
}


