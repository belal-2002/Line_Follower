void loopStrategy2() { // Left Strategy

  cancelBlindTurn_1();
  
  //if (checkEarlyRecovery_2()) return;
  
  //checkMPUTurnCompletion_3();
  
  if (isCurrentlyTurning_4()) return;
  
  if (handleLineLoss_5()) return;

  if (activateTurn_6()) return; 

  ignoreIntersections_7();

  calculateError();
}
  
