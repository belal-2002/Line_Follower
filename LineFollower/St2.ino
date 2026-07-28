void loopStrategy2() { // للأمام ثم لليسار

  cancelTurn_1();

  if (isTurning_2()) return;

  if (activateGo_3()) return;

  if (noLine_4()) return;

  cleanIR_6();

  calculateError(); 
}
  
