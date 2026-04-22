void calculateError() {
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
    caseMotor = 0; // العودة للمسار الطبيعي

    // --- حماية الـ PD عند استرجاع الخط ---
    if (!lineWasFound) {
      lastTime = micros();       
      lastError = currentError;
      lineWasFound = true; // تم الاستشفاء بنجاح
    }

    calculatePD();
  } else {
    // فقدنا الخط بالكامل (خط دفاع أخير إذا لم تتصرف الاستراتيجية)
    lineWasFound = false;
  }
}


