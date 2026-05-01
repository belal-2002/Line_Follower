void loopStrategy4() { //Right 
  if ((rightRadar == 2) && (midSensor >= 3) && (!leftRadar)) {
    goRight = true; 
    rightMotor();
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
  
  if (goLeft || goRight) return;

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