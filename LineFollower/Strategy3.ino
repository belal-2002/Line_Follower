void loopStrategy3() { 
  if ((bitRead(sensorBit, 0)) && (midSensor >= 2) && (!leftRadar)) {
    lineWasFound = false;
    caseMotor = 2;
    goRight = true; 
    return;   
  }

  if (midSensor && (!rightRadar)) { 
    goLeft = false;
    goRight = false;
  }
  
  if (goLeft && leftRadar) { goLeft = false; calculateError(); return; }
  if (goRight && rightRadar) { goRight = false; calculateError(); return; }
  
  if (goLeft || goRight) return;

  if (!allSensor) {
    lineWasFound = false;
    if (leftRadarOn) {
      caseMotor = 1;
      goLeft = true; 
      return;
    }
    if (rightRadarOn) {
      caseMotor = 2;
      goRight = true;
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