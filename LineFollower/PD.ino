void calculatePD() {
  float P = currentError;

  float D = currentError - lastError; 
  lastError = currentError;
  
  PD_Value = (Kp * P) + (Kd * D);

  moveMotor();
}