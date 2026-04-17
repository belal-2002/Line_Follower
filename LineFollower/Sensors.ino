void loopSensors() {
  // 1. قراءة جميع الحساسات
  for (int i = 0; i < 12; i++) {
    sensorValue[i] = analogRead(sensorPins[i]);
  }
  //sensorValue[1] -= 270;
  sensorValue[10] -= 270;
}

