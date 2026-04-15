void setupNetwork() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // انتظار الاتصال
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.begin();
  TelnetStream.begin();
}

void handleNetwork() {
  ArduinoOTA.handle(); // استقبال أكواد البرمجة عبر الهواء
  
  // معالجة أوامر الكيبورد لتعديل PID
  if (TelnetStream.available()) {
    char c = TelnetStream.read();
    
    if (c == 'q') Kp += 0.1;
    if (c == 'a') Kp -= 0.1;
    
    if (c == 'w') Ki += 0.01;
    if (c == 's') Ki -= 0.01;
    
    if (c == 'e') Kd += 0.1;
    if (c == 'd') Kd -= 0.1;

    // منع القيم السلبية
    if (Kp < 0) Kp = 0;
    if (Ki < 0) Ki = 0;
    if (Kd < 0) Kd = 0;
  }
}

void printDebugData() {
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 150) {
    if (isRunning) {
      TelnetStream.printf("Kp:%.1f  Ki:%.2f  Kd:%.1f  |  ", Kp, Ki, Kd);
     
      TelnetStream.printf("Err:%4.1f | M_Left:%3d  M_Right:%3d\n", currentError, leftMotorSpeed, rightMotorSpeed);
      
    }
    lastPrintTime = millis();
  }
}