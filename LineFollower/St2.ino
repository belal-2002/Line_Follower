void loopStrategy2() { //Left
  if ((leftRadar == 2) && (midSensor >= 3) && (!rightRadar)) {
    caseMotor = 1;
    goLeft = true; 
    loopMotor();
    delay(75);
    turnStartTime = millis();
    lineWasFound = false;
    return;   
  }

  if (((bitRead(sensorBit, 5)) || (bitRead(sensorBit, 6))) && (!leftRadar)) {
    //if (millis() - turnStartTime > 100) {
    goLeft = false;
    goRight = false;
    //}
  }

  //if (goLeft && leftRadar) { goLeft = false; calculateError(); return; }
  //if (goRight && rightRadar) { goRight = false; calculateError(); return; }

  if (goLeft || goRight) {
  //if (millis() - turnStartTime > 100) { // أقصى وقت مسموح للدوران الأعمى
      goLeft = false; 
      goRight = false;
      caseMotor = 0; // العودة للوضع الطبيعي
  //}
  return;
}

  if (!allSensor) {
    lineWasFound = false;
    if (leftRadarOn) {
      caseMotor = 1;
      goLeft = true;
      turnStartTime = millis(); 
      return;
    }
    if (rightRadarOn) {
      caseMotor = 2;
      goRight = true;
      turnStartTime = millis();
      return;
    }
  }

  // 1. اكتشاف تقاطع الزائد (+) وتجاوزه مستقيماً
  if (leftRadar && rightRadar && midSensor) {
    // إعماء الرادارين معاً ليمر الروبوت بناءً على حساسات المنتصف فقط
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
    bitClear(sensorBit, 10);
    bitClear(sensorBit, 11);
  }
  // 2. تجاهل الفخاخ اليمنى (حرف T المتجه لليمين)
  else if (rightRadar && midSensor) {
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
  }
  
  calculateError();
}      