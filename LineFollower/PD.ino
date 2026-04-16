void calculatePD() { // يعمل الآن كمتحكم هجين (Hybrid PD)
  
  // 1. حساب الزمن الدقيق (dt)
  unsigned long currentTime = micros();
  static unsigned long lastTime = 0;
  float dt = (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;
  if (dt <= 0.0) dt = 0.001; 

  // ==========================================
  // المقاطعة العنيفة: التفاف 90 درجة (Tank Turn)
  // ==========================================
  if (currentState == SHARP_LEFT) {
    // عكس المحرك الأيسر للخلف، ودفع الأيمن للأمام
    digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); 
    digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); 
    
    ledcWrite(PWMA, tankTurnSpeed);
    ledcWrite(PWMB, tankTurnSpeed);
    
    lastError = -40; // زرع ذاكرة وهمية في حال فقدان الخط لضمان استمرار الدوران
    return; // الخروج من الدالة (إعدام الـ PD مؤقتاً)
  } 
  else if (currentState == SHARP_RIGHT) {
    // دفع المحرك الأيسر للأمام، وعكس الأيمن للخلف
    digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
    digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); 
    
    ledcWrite(PWMA, tankTurnSpeed);
    ledcWrite(PWMB, tankTurnSpeed);
    
    lastError = 40; 
    return; // الخروج من الدالة
  }

  // ==========================================
  // الحالة الطبيعية: تتبع المسار السلس (Pure PD)
  // ==========================================
  
  // إرجاع المحركات للاتجاه الأمامي الطبيعي
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);

  float P = 0;
  float D = 0;

  if (isLineLost) {
    // ذاكرة الروبوت: استخدام قيمة P قوية للعودة إذا قفز عن الخط
    P = (lastError > 0) ? 60.0 : -60.0; 
    D = 0; // إبطال التفاضل لمنع الصدمة
  } else {
    P = currentError;
    D = (currentError - lastError) / dt;
  }

  // حساب المعادلة
  float PD_Value = (Kp * P) + (Kd * D);

  // تقييد السرعة وإرسالها للمحركات (10-bit)
  int leftMotorSpeed  = constrain(baseSpeed + PD_Value, 0, 1023);
  int rightMotorSpeed = constrain(baseSpeed - PD_Value, 0, 1023);

  ledcWrite(PWMA, leftMotorSpeed);
  ledcWrite(PWMB, rightMotorSpeed);

  // تحديث الذاكرة للمستقبل
  if (!isLineLost) {
    lastError = currentError;
  }
}