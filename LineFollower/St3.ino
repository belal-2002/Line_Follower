void loopStrategy3() { // Left Strategy
  cancelBlindTurn_1();
  
  if (checkEarlyRecovery_2(leftMidRadar, rightMidRadar)) return;
  
  checkMPUTurnCompletion_3(70.0, 50.0, 350); // دوران بزاوية 70 وتايم أوت 350ms
  
  if (isCurrentlyTurning_4()) return;
  
  if (handleLineLoss_5(midSensor, leftMidRadarOn, rightMidRadarOn)) return;

  if (activateTurn_6(true, leftMidRadar, 2, rightRadar)) return; // true تعني دوران يسار

  ignoreIntersections_7();
  calculateError();
}