void loopPrint() {
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 500) {

      for (int i = 0; i < 12; i++) {
        int sensorValue = analogRead(sensorPins[i]); // قراءة القيمة التناظرية
        TelnetStream.print(sensorValue);             // طباعة الرقم
        TelnetStream.print("\t");                    // وضع مسافة (Tab) لترتيب الأرقام في أعمدة
      }

      TelnetStream.print("\t");
      //TelnetStream.printf("Kp:%.1f Ki:%.3f Kd:%.1f | ", Kp, Ki, Kd);
      TelnetStream.printf("Kp:%.1f  Kd:%.1f  Speed:%d  TSpeed:%d", Kp, Kd, baseSpeed, turnSpeed);
      TelnetStream.print("\t");
      TelnetStream.printf("Err:%4.1f | ML:%d MR:%d", currentError, leftMotorSpeed, rightMotorSpeed);

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
  }
}