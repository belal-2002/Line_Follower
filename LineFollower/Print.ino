void loopPrint() {
  ArduinoOTA.handle(); // استقبال أكواد البرمجة عبر الهواء
  if (!(strategy == 0)){
  if (millis() - lastPrintTime > 1500) {
  /*
  String output = "";
  for (int i = 0; i < 10; i++) {
    output += bitRead(sensorBit, 9 - i);
    output += "\t";
  }
  TelnetStream.print(output);
  TelnetStream.println();    
  */
  
  /*
  for (int i = 0; i < 10; i++) {
    TelnetStream.print(sensorValue[i]);  
    TelnetStream.print("\t");               
  }
  */


  TelnetStream.print("\t");
  TelnetStream.println();
  TelnetStream.println();
  TelnetStream.printf("Kp:%.2f  Kd:%.1f  maxSpeed:%d  Speed:%d  TSpeed:%d  strategy:%d  Angle:%.1f", Kp, Kd, maximumSpeed, baseSpeed, turnSpeed, strategy, pitchAngle);
  //TelnetStream.print("\t");
  //TelnetStream.printf("Err:%4.1f | ML:%d MR:%d", currentError, leftMotorSpeed, rightMotorSpeed);
  
  TelnetStream.print("  Left: ");
  TelnetStream.print(currentLeftTicks);
  TelnetStream.print("  Right: ");
  TelnetStream.print(currentRightTicks);
  TelnetStream.print("  distanceNow: ");
  TelnetStream.print(distanceNow);  
  TelnetStream.println();

  /*
  TelnetStream.print("  Kp: ");
  TelnetStream.print(Kp);
  TelnetStream.print("  Kd: ");
  TelnetStream.print(Kd);
  TelnetStream.println();
  /*

  lastPrintTime = millis();
  }

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
    if (c == 't') {turnSpeed += 25; innerTurnSpeed = turnSpeed / 5*4;}
    if (c == 'g') {turnSpeed -= 25; innerTurnSpeed = turnSpeed / 5*4;}
    if (c == '0') strategy = 0;
    if (c == '1') strategy = 1;
    if (c == '2') strategy = 2;
    if (c == '3') strategy = 3;

    if (Kp < 0) Kp = 0;
    if (Kd < 0) Kd = 0;
    if (baseSpeed < 0) baseSpeed = 0;
    if (turnSpeed < 0) turnSpeed = 0;
    if (innerTurnSpeed < 0) innerTurnSpeed = 0;
    if (maximumSpeed < 0) maximumSpeed = 0;
  }
  } else {
    if (millis() - lastPrintTime > 3500) {
      TelnetStream.printf("strategy:%d", strategy);
      lastPrintTime = millis();
      //TelnetStream.println();
    }
  }
}