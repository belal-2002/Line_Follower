void loopSensors() {
  // 1. قراءة جميع الحساسات
  for (int i = 0; i < 12; i++) {
    sensorValue[i] = analogRead(sensorPins[i]);
  }
  sensorValue[1] -= 70;
  sensorValue[10] -= 170;
   
  for (int i = 0; i < 12; i++) {
    if (sensorValue[i] > lineThreshold) {
      TelnetStream.print(1);  
      TelnetStream.print("\t");
    } else {
      TelnetStream.print(0);  
      TelnetStream.print("\t");
    }
  }
}

