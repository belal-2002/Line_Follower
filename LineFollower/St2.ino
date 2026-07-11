void loopStrategy2() { // للأمام ثم لليسار
  enableInversionDetection = true;

  if (leftOutRadar && leftRadar && leftMidRadar && (bitRead(sensorBit, 8) == 1) ){
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

  
