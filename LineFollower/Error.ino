void calculateError() {
  weightedSum = 0;
  sum = 0;

  for (int i = 0; i < 12; i++) {
    if (sensorValue[i] > lineThreshold) {
      weightedSum += (long)sensorValue[i] * sensorWeights[i];
      sum += sensorValue[i];
    }
  }

  if (sum > 0) {
    currentError = (float)weightedSum / (float)sum;
    lineAvailable = true;

    if (!lineWasFound) {
      lastTime = micros();       
      lastError = currentError;  
    }

    if (currentError > 0){
      goRight = true;
      //goLeft = false;
    } else if (currentError < 0) {
      goRight = false;
      //goLeft = true;
    }
  
  } else {
    lineAvailable = false; // الروبوت فقد الخط كلياً في المسار الطبيعي
  }
  lineWasFound = lineAvailable;
}






