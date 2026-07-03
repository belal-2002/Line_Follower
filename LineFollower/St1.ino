void loopStrategy1() { // للأمام ثم لليسار

  cancelTurn_1();

  if (isTurning_2()) return;

  if (activateGo_3()) return;

  if (noLine_401()) return;

  cleanIR_6();

  calculateError();
}
