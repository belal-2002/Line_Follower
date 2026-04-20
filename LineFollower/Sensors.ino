void loopSensors() {
  // 1. قراءة جميع الحساسات
  for (int i = 0; i < 12; i++) {
    sensorValue[i] = analogRead(sensorPins[i]);
  }
  sensorValue[1] -= 70;
  sensorValue[10] -= 170;

  if (sensorValue[0] > lineThreshold) {
    bitSet(sensorBit, 11);
  } else bitClear(sensorBit, 11);

  if (sensorValue[1] > lineThreshold) {
    bitSet(sensorBit, 10);
  } else bitClear(sensorBit, 10);

  if (sensorValue[2] > lineThreshold) {
    bitSet(sensorBit, 9);
  } else bitClear(sensorBit, 9);

  if (sensorValue[3] > lineThreshold) {
    bitSet(sensorBit, 8);
  } else bitClear(sensorBit, 8);

  if (sensorValue[4] > lineThreshold) {
    bitSet(sensorBit, 7);
  } else bitClear(sensorBit, 7);

  if (sensorValue[5] > lineThreshold) {
    bitSet(sensorBit, 6);
  } else bitClear(sensorBit, 6);

  if (sensorValue[6] > lineThreshold) {
    bitSet(sensorBit, 5);
  } else bitClear(sensorBit, 5);

  if (sensorValue[7] > lineThreshold) {
    bitSet(sensorBit, 4);
  } else bitClear(sensorBit, 4);

  if (sensorValue[8] > lineThreshold) {
    bitSet(sensorBit, 3);
  } else bitClear(sensorBit, 3);

  if (sensorValue[9] > lineThreshold) {
    bitSet(sensorBit, 2);
  } else bitClear(sensorBit, 2);

  if (sensorValue[10] > lineThreshold) {
    bitSet(sensorBit, 1);
  } else bitClear(sensorBit, 1);

  if (sensorValue[11] > lineThreshold) {
    bitSet(sensorBit, 0);
  } else bitClear(sensorBit, 0);
}

