void loopStrategy7() {
  enableInversionDetection = false;

  if (midSensor >= 5){
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