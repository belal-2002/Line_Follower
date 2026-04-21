void calculateError() {
  rightSensor = 0;
  leftSensor = 0;
  allSensor = 0;
  weightedSum = 0;
  sum = 0;

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


