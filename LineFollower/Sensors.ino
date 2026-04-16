void setupSensors() {
  for (int i = 0; i < 12; i++) {
    pinMode(sensorPins[i], INPUT);
  }
}

void calculateError() {
  long weightedSum = 0;
  long sum = 0;
  int rawValues[12];
  
  for (int i = 0; i < 12; i++) {
    rawValues[i] = analogRead(sensorPins[i]);
    
    // فلترة اللون الأسود
    if (rawValues[i] > 1700) {
      weightedSum += (long)rawValues[i] * sensorWeights[i];
      sum += rawValues[i];
    }
  }

  if (sum > 0) {
    currentError = (float)weightedSum / (float)sum;
    lastError = currentError;
  } else {
    // ذاكرة الروبوت إذا فقد الخط
    currentError = (lastError > 0) ? 60 : -60; 
  }
}