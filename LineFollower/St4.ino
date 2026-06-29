void loopStrategy4() {  //يسار

  cancelBlindTurn_1();
  
  checkMPUTurnCompletion_3();

  if (isCurrentlyTurning_4()) return;

  if (handleLineLoss_5()) return;

  if (activateTurn_6()) return; 

  ignoreIntersections_7();

  calculateError();
}