void calculatePD() {
  P = 0;
  D = 0;
  currentTime = micros();

  dt = (float) (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  if (dt <= 0.0){
    dt = 0.001;
  }

  P = currentError;
  D = (currentError - lastError) / dt;
  lastError = currentError; //**
  PD_Value = (Kp * P) + (Kd * D);
}