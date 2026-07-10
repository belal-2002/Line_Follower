void loopStrategy4() { // لليسار
  enableInversionDetection = false;

  cancelTurn_1();

  if (isTurning_2()) return;

  if (activateGo_3()) return;

  if (gap_4()) return;

  if (activateTurn_5()) return; 

  cleanIR_6();

  calculateError();
}



