void loopStrategy3() { // للأمام ثم لليسار
  enableInversionDetection = true;

  cancelTurn_1();

  if (isTurning_2()) return;

  if (activateGo_3(true)) return;

  if (gap_4()) return;

  cleanIR_6();

  calculateError();
}