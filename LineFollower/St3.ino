void loopStrategy3() { // لليسار

  cancelTurn_1();

  if (isTurning_2()) return;

  if (activateGo_3()) return;

  if (noLine_4()) return;

  if (activateTurn_5(1)) return; 

  cleanIR_6();

  calculateError();
}