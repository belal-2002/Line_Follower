void calculatePD() {
  // حساب الزمن الدقيق (dt)
  P = 0;
  D = 0;
  lastTime = 0;
  currentTime = micros();

  dt = (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  if (dt <= 0.0){
    dt = 0.001;
  }

  if (lineLost) {
    // ذاكرة الروبوت: استخدام قيمة P قوية للعودة إذا قفز عن الخط
    P = (lastError > 0) ? 60.0 : -60.0; 
    D = 0; // إبطال التفاضل لمنع الصدمة
  } else {
    P = currentError;
    D = (currentError - lastError) / dt;
  }

  // حساب المعادلة
  float PD_Value = (Kp * P) + (Kd * D);

  // تحديث الذاكرة للمستقبل
  if (!lineLost) {
    lastError = currentError;
  }
}