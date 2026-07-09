void loopStrategy4() { // لليسار

  cancelTurn_1();

  if (isTurning_2()) return;

  if (activateGo_3()) return;

  if (gap_4()) return;

  // تحديد العدد هنا: نمرر الرقم 1 ليتجاهل المنعطف مرة واحدة ويلتف في الثانية
  if (activateTurn_5(1)) return; 

  cleanIR_6();

  calculateError();
}



