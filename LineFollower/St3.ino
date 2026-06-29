void loopStrategy3() {  //لامام ثم يسار

  cancelBlindTurn_1();
  
  //if (checkEarlyRecovery_2()) return;

  if (isCurrentlyTurning_4()) return;

  if (handleLineLoss_5()) return;

  ignoreIntersections_7();

  calculateError();
}