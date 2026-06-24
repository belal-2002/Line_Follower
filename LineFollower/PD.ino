void calculatePD() {
  float P = currentError;

  float D = currentError - lastError; 
  lastError = currentError;
  
  float PD_Value = (Kp * P) + (Kd * D);

  moveMotor();
}