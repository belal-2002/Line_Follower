void loopStrategy2() { //Left
  if ((leftRadar == 2) && (midSensor >= 3) && (!rightRadar)) {
    goLeft = true; 
    leftMotor();
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

  if (goLeft || goRight) return;

  if (!allSensor) {
    lineWasFound = false;
    if (leftRadarOn) {
      goLeft = true; 
      leftMotor();
      return;
    }
    if (rightRadarOn) {
      goRight = true;
      rightMotor();      
      return;
    }
    return;
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