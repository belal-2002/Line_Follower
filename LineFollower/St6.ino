void loopStrategy6() {
if (isInverted) { 
    isPostInversion = true; 
  }

  if (!isPostInversion) {
    enableInversionDetection = true;
    if (midSensor >= 6){
      leftMotor();
      return;
    }
    cancelTurn_1();
    if (isTurning_2()) return;
    if (activateGo_3()) return; 
    if (gap_4()) return;
    cleanIR_6();
    calculateError();
  } else {
    cancelTurn_1();
    if (isTurning_2()) return;
    if (activateGo_3()) return;
    if (gap_4()) return;
    cleanIR_6();
    calculateError();
  }
}