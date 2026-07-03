void loopStrategy4() { // لليسار

  cancelTurn_1();

  if (isTurning_2()) return;

  if (activateGo_3()) return;

  if (noLine_402()) return;

  if (activateTurn_5()) return; 

  cleanIR_6();

  calculateError();
}



