void loopStrategy0() {
  if (leftRadar){
    leftRadarOn = true;
    leftRadarTime = millis();
  } else { if (millis() - leftRadarTime > RadarTime) {
      leftRadarOn = false;
    }  
  } 
  if (rightRadar){
    rightRadarOn = true;
    rightRadarTime = millis();
  } else { if (millis() - rightRadarTime > RadarTime) {
      rightRadarOn = false;
    }  
  } 

  if (midSensor) { 
    goLeft = false;
    goRight = false;
  }
  
  if (goLeft && leftRadar) calculateError();
  if (goRight && rightRadar) calculateError();

  if (!allSensor) {
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

  if (radar && midSensor){
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
    bitClear(sensorBit, 10);
    bitClear(sensorBit, 11);
  }
  
  calculateError();
}      