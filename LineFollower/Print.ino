void loopPrint() {
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 500) {

      for (int i = 0; i < 12; i++) {
        TelnetStream.print(sensorValue[i]);  
        TelnetStream.print("\t");               
      }

      for (int i = 0; i < 12; i++) {
        if (sensorValue[i] > lineThreshold) {
          TelnetStream.print(1);  
          TelnetStream.print("\t");
        } else {
          TelnetStream.print(0);  
          TelnetStream.print("\t");
        }
      }

      TelnetStream.print("\t");
      TelnetStream.printf("Kp:%.1f  Kd:%.1f  Speed:%d  TSpeed:%d", Kp, Kd, baseSpeed, turnSpeed);
      TelnetStream.print("\t");
      //TelnetStream.printf("Err:%4.1f | ML:%d MR:%d", currentError, leftMotorSpeed, rightMotorSpeed);

      TelnetStream.println();
    lastPrintTime = millis();
  }

  ArduinoOTA.handle(); // استقبال أكواد البرمجة عبر الهواء
  // معالجة أوامر الكيبورد لتعديل PID
  if (TelnetStream.available()) {
    char c = TelnetStream.read();
    if (c == 'q') Kp += 1;
    if (c == 'a') Kp -= 1;
    if (c == 'w') Kd += 1;
    if (c == 's') Kd -= 1;
    if (c == 'e') baseSpeed += 50;
    if (c == 'd') baseSpeed -= 50;
    if (c == 'r') turnSpeed += 50;
    if (c == 'f') turnSpeed -= 50;

    if (Kp < 0) Kp = 0;
    if (Kd < 0) Kd = 0;
    if (baseSpeed < 0) baseSpeed = 0;
    if (turnSpeed < 0) turnSpeed = 0;
  }
}