void loopStrategy2() { 
  if ((bitRead(sensorBit, 11)) && (midSensor >= 2) && (!rightRadar)) {
    lineWasFound = false;
    caseMotor = 1;
    goLeft = true; 
    return;   
  }

  if (midSensor && (!leftRadar)) { 
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

  if (rightRadar && midSensor){
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
  }
  
  calculateError();
}      