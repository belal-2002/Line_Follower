void loopStrategy3() { // للأمام ثم لليسار
  enableInversionDetection = false;
  
  if (midSensor >= 4){
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