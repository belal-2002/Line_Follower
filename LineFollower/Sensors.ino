void calculateError() {
  weightedSum = 0;
  sum = 0;

  // 1. قراءة جميع الحساسات
  for (int i = 0; i < 12; i++) {
    rawValues[i] = analogRead(sensorPins[i]);
  }
  rawValues[1] -= 270;
  rawValues[10] -= 270;

  for (int i = 0; i < 12; i++) {
    if (rawValues[i] > 1200) {
      weightedSum += (long)rawValues[i] * sensorWeights[i];
      sum += rawValues[i];
    }
  }

  if (sum > 0) {
    currentError = (float)weightedSum / (float)sum;
    isLineLost = false;
  } else {
    isLineLost = true; // الروبوت فقد الخط كلياً في المسار الطبيعي
  }
}