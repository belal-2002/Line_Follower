/*
      if (allSensor >= 11 || radar >= 3) {  // السير في خط مستقيم لتجاوز التقاطع
        caseMotor = 3;
        lineWasFound = false;
        lineAvailable = false;
        loopMotor();
        return;
      }

      // لازم افحص بت معين
      if (leftRadar == 2) {
    
      }
*/      
/*



void loopStrategy0() {
  if (leftSensor){
    leftRadarOn = true;
    leftRadarTime = millis();
  } else { if (millis() - leftRadarTime > RadarTime) {
      leftRadarOn = false;
    }  
  } 
  if (rightSensor){
    rightRadarOn = true;
    rightRadarTime = millis();
  } else { if (millis() - rightRadarTime > RadarTime) {
      rightRadarOn = false;
    }  
  } 

  //if (midSensor >= 7) {
  //  calculateError();
  //  return;
  //}

  if (midSensor) { 
    goLeft = false;
    goRight = false;
  }

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
    if (goLeft || goRight) return;

    calculateError();
    return;
  }
  calculateError();
} 


*/

