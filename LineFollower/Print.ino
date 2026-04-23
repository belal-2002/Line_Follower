void loopPrint() {
  if (millis() - lastPrintTime > 2000) {

    for (int i = 0; i < 12; i++) {
      TelnetStream.print(sensorValue[i]);  
      TelnetStream.print("\t");               
    }
    TelnetStream.println();

  String output = "";
  for (int i = 0; i < 12; i++) {
    output += bitRead(sensorBit, 11 - i);
    output += "\t";
  }
  TelnetStream.print(output);

      TelnetStream.print("\t");
      TelnetStream.printf("Kp:%.2f  Kd:%.1f  maxSpeed:%d  Speed:%d  TSpeed:%d  strategy:%d", Kp, Kd, maximumSpeed, baseSpeed, turnSpeed, strategy);
      TelnetStream.print("\t");
      //TelnetStream.printf("Err:%4.1f | ML:%d MR:%d", currentError, leftMotorSpeed, rightMotorSpeed);

      TelnetStream.println();
    lastPrintTime = millis();
  }

  ArduinoOTA.handle(); // استقبال أكواد البرمجة عبر الهواء
  // معالجة أوامر الكيبورد لتعديل PID
  if (TelnetStream.available()) {
    char c = TelnetStream.read();
    if (c == 'q') Kp += 0.1;
    if (c == 'a') Kp -= 0.1;
    if (c == 'w') Kd += 1;
    if (c == 's') Kd -= 1;
    if (c == 'e') maximumSpeed += 25;
    if (c == 'd') maximumSpeed -= 25;
    if (c == 'r') baseSpeed += 25;
    if (c == 'f') baseSpeed -= 25;
    if (c == 't') turnSpeed += 25;
    if (c == 'g') turnSpeed -= 25;
    if (c == '0') strategy = 0;
    if (c == '1') strategy = 1;
    if (c == '2') strategy = 2;
    if (c == '3') strategy = 3;

    if (Kp < 0) Kp = 0;
    if (Kd < 0) Kd = 0;
    if (baseSpeed < 0) baseSpeed = 0;
    if (turnSpeed < 0) turnSpeed = 0;
    if (maximumSpeed < 0) maximumSpeed = 0;
    if (strategy < 0) strategy = 0;
  }
}