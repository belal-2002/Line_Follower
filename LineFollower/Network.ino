void setupNetwork() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //ArduinoOTA.begin();
  //TelnetStream.begin();
}
void TurnOnService() {
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.setHostname("Belal_Line_Follower");     // اسم الروبوت على الشبكة
    ArduinoOTA.setPassword("789");      // كلمة المرور السرية

    // بدء الخدمات بعد ضبط الحماية
    ArduinoOTA.begin();
    TelnetStream.begin();
    serviceStarted = true; 
  }
}

void handleNetwork() {
  ArduinoOTA.handle(); // استقبال أكواد البرمجة عبر الهواء

  // معالجة أوامر الكيبورد لتعديل PID
  if (TelnetStream.available()) {
    char c = TelnetStream.read();
    if (c == 'q') Kp += 1;
    if (c == 'a') Kp -= 1;
    if (c == 'w') Ki += 0.001;
    if (c == 's') Ki -= 0.001;
    if (c == 'e') Kd += 1;
    if (c == 'd') Kd -= 1;

    if (Kp < 0) Kp = 0;
    if (Ki < 0) Ki = 0;
    if (Kd < 0) Kd = 0;
  }
}

void printDebugData() {
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 500) {

      for (int i = 0; i < 12; i++) {
        int sensorValue = analogRead(sensorPins[i]); // قراءة القيمة التناظرية
        TelnetStream.print(sensorValue);             // طباعة الرقم
        TelnetStream.print("\t");                    // وضع مسافة (Tab) لترتيب الأرقام في أعمدة
      }

      TelnetStream.print("\t");
      TelnetStream.printf("Kp:%.1f Ki:%.3f Kd:%.1f | ", Kp, Ki, Kd);
      TelnetStream.print("\t");
      TelnetStream.printf("Err:%4.1f | ML:%d MR:%d\n", currentError, leftMotorSpeed, rightMotorSpeed);
      TelnetStream.println();

    lastPrintTime = millis();
  }
}