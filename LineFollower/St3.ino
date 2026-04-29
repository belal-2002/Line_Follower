void loopStrategy3() { //Right 
  if ((rightRadar == 2) && (midSensor >= 3) && (!leftRadar)) {
    caseMotor = 2;
    goRight = true; 
    loopMotor();
    delay(75);
    turnStartTime = millis();
    lineWasFound = false;
    return;   
  }

  if (((bitRead(sensorBit, 5)) || (bitRead(sensorBit, 6))) && (!rightRadar)) { 
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
 // }
  return;
}

  if (!allSensor) {
    lineWasFound = false;
    if (rightRadarOn) {
      caseMotor = 2;
      goRight = true;
      turnStartTime = millis();
      return;
    }
    if (leftRadarOn) {
      caseMotor = 1;
      goLeft = true;
      turnStartTime = millis();
      return;
    }

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