void calculatePD() {
  P = currentError;
  // حذف القسمة على dt تماماً لمنع الفرملة العكسية العنيفة
  D = currentError - lastError; 
  lastError = currentError;
  
  PD_Value = (Kp * P) + (Kd * D);

  moveMotor();
}