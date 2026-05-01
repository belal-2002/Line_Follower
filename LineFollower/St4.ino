void loopStrategy4() { //Right 
  // إلغاء الدوران الأعمى فور ملامسة حساسات المنتصف للخط
  if (midSensor) { 
    goLeft = false;
    goRight = false;
  }
  
  // الاستشفاء المبكر (إنهاء الدوران فور التقاط الرادار للخط)
  if (goRight && rightRadar) { goRight = false; calculateError(); return; }
  if (goLeft && leftRadar) { goLeft = false; calculateError(); return; }

  if (((bitRead(sensorBit, 5)) || (bitRead(sensorBit, 6))) && (!rightRadar)) { 
    //if (millis() - turnStartTime > 100) {
    turnRight = false;
    //}
  }
  
  // الاستمرار في الدوران إذا بدأناه
  if (turnRight || goLeft || goRight) return;

  // الدخول في حالة الفقدان الكلي للخط
  if (!allSensor) {
    lineWasFound = false;
    if (rightRadarOn) {
      goRight = true;
      rightMotor();
      return;
    }
    if (leftRadarOn) {
      goLeft = true; 
      leftMotor();
      return;
    }
    forwardMotor();
    return;
  }

  if ((rightRadar == 2) && (midSensor >= 3) && (!leftRadar)) {
    turnRight = true; 
    rightMotor();
    delay(85);
    turnStartTime = millis();
    lineWasFound = false;
    return;   
  }

  if (leftRadar && rightRadar && midSensor) {
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
    bitClear(sensorBit, 10);
    bitClear(sensorBit, 11);
  }
  else if (leftRadar && midSensor) {
    bitClear(sensorBit, 10);
    bitClear(sensorBit, 11);
  }
  
  calculateError();
}      