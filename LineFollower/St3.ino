void loopStrategy3() { // لليسار

  cancelBlindTurn_1();

  if (isCurrentlyTurning_4()) return;

  if (handleLineLoss_5()) return;

  if (activateTurn_6()) return; 

  ignoreIntersections_7();

  calculateError();
}