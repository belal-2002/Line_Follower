void calculateError() {
  long weightedSum = 0;
  long sum = 0;
  sensorValue[0] = 0;
  sensorValue[9] = 0;
  for (int i = 0; i < 10; i++) {
    if (sensorValue[i] > lineThreshold) { 
      weightedSum += (long)sensorValue[i] * sensorWeights[i];
      sum += sensorValue[i];
    }
  }

  if (sum > 0) {
    currentError = (float)weightedSum / (float)sum;

    // --- حماية الـ PD عند استرجاع الخط ---
    if (!lineWasFound) {      
      lastError = currentError;
      lineWasFound = true; // تم الاستشفاء بنجاح
    }

    calculatePD();
  } else {
    // فقدنا الخط بالكامل (خط دفاع أخير إذا لم تتصرف الاستراتيجية)
    lineWasFound = false;
    forwardMotor();
  }
}


