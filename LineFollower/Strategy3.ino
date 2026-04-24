void loopStrategy3() { //Right 
  if (rightRadar && (midSensor >= 3) && (!leftRadar)) {
    lineWasFound = false;
    caseMotor = 2;
    goRight = true; 
    loopMotor();
    delay(127);
    return;   
  }

  if ((bitRead(sensorBit, 5)) || (bitRead(sensorBit, 6)) && (!rightRadar)) { 
    goLeft = false;
    goRight = false;
  }
  
  //if (goLeft && leftRadar) { goLeft = false; calculateError(); return; }
  //if (goRight && rightRadar) { goRight = false; calculateError(); return; }
  
  if (goLeft || goRight) return;

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