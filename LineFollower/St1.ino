void loopStrategy1() { // للأمام ثم لليسار

  cancelBlindTurn_1();

  if (isCurrentlyTurning_4()) return;

  if (handleLineLoss_5()) return;

  ignoreIntersections_7();

  calculateError();
}
