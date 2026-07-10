void loopStrategy2() { // للأمام ثم لليسار
  enableInversionDetection = false;

  if (leftOutRadar && leftRadar && leftMidRadar && (bitRead(sensorBit, 8) == 1) && (bitRead(sensorBit, 7) == 1) ){
    leftMotor();
    return;
  }

  cancelTurn_1();

  if (isTurning_2()) return;

  if (activateGo_3()) return;

  if (gap_4()) return;

  cleanIR_6();

  calculateError();
}

  
