  // ==========================================
  // المقاطعة العنيفة: التفاف 90 درجة (Tank Turn)
  // ==========================================
  if (currentState == SHARP_LEFT) {
    // عكس المحرك الأيسر للخلف، ودفع الأيمن للأمام
    digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); 
    digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); 
    
    ledcWrite(PWMA, turnSpeed);
    ledcWrite(PWMB, turnSpeed);
    
    lastError = -40; // زرع ذاكرة وهمية في حال فقدان الخط لضمان استمرار الدوران
    delay(250);
    return; // الخروج من الدالة (إعدام الـ PD مؤقتاً)
  } 
  else if (currentState == SHARP_RIGHT) {
    // دفع المحرك الأيسر للأمام، وعكس الأيمن للخلف
    digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
    digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); 
    
    ledcWrite(PWMA, turnSpeed);
    ledcWrite(PWMB, turnSpeed);
    
    lastError = 40; 

    delay(250);
    return; // الخروج من الدالة
  }
