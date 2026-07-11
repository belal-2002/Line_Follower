void loopStrategy6() {
  enableInversionDetection = true;
  
  if (midSensor >= 4){
    leftMotor();
    return;
  }
  
  cancelTurn_1();

  if (isTurning_2()) return;

  if (activateGo_3()) return;

  if (gap_4()) return;

  if (activateTurn_5()) return; 

  cleanIR_6();

  calculateError();
}