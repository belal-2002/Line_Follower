void moveMotor() {
  // إرجاع المحركات للاتجاه الأمامي الطبيعي
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
      
  // تقييد السرعة وإرسالها للمحركات (10-bit)
  leftMotorSpeed  = constrain(baseSpeed + PD_Value, 0, maximumSpeed);
  rightMotorSpeed = constrain(baseSpeed - PD_Value, 0, maximumSpeed);

  ledcWrite(PWMA, leftMotorSpeed);
  ledcWrite(PWMB, rightMotorSpeed);
}

void leftMotor() {
  // الدوران لليسار (Left Turn)
 // 1. المحرك الأيسر (الداخلي) يعمل كمرساة ويدور للخلف بسرعة منخفضة
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
  ledcWrite(PWMA, turnSpeed); // <-- تم التعديل للسرعة التفاضلية
      
  // 2. المحرك الأيمن (الخارجي) يستمر بالدفع للأمام بالسرعة العالية المحددة
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); 
  ledcWrite(PWMB, turnSpeed);      // 600
}

void rightMotor() {
  // الدوران لليمين (Right Turn)
  // 1. المحرك الأيسر (الخارجي) يستمر بالدفع للأمام بالسرعة العالية المحددة
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
  ledcWrite(PWMA, turnSpeed);      // 600
      
  // 2. المحرك الأيمن (الداخلي) يعمل كمرساة ويدور للخلف بسرعة منخفضة
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); 
  ledcWrite(PWMB, turnSpeed); // <-- تم التعديل للسرعة التفاضلية
}

void stopMotor() {
  // توقف تام
  digitalWrite(STBY, LOW);  
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, LOW); 
  digitalWrite(BIN1, LOW);  digitalWrite(BIN2, LOW);  
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
}

void forwardMotor() {
  // السير في خط مستقيم
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);

  ledcWrite(PWMA, baseSpeed);
  ledcWrite(PWMB, baseSpeed);
}

void updateDistance() {
  // 1. إيقاف المقاطعات لحظياً لنسخ القيم بأمان
  noInterrupts();
  long currentLeftTicks = leftTicks;
  long currentRightTicks = rightTicks;
  interrupts();

  // 2. حساب المسافة المقطوعة لكل عجل بالسنتيمتر
  float distanceLeft = currentLeftTicks * distancePerTick;
  float distanceRight = currentRightTicks * distancePerTick;

  // 3. المسافة الإجمالية لمركز الروبوت (لحسابات الرادار)
  distanceNow = (distanceLeft + distanceRight) / 2.0;

  /* * ملاحظة هندسية حول "الاتجاه":
   * يمكننا حساب زاوية دوران الروبوت من العجلات (Odometry) كالتالي:
   * float encoderAngleRad = (distanceRight - distanceLeft) / wheelBase;
   * float encoderAngleDeg = encoderAngleRad * (180.0 / PI);
   * * ولكن! الانزلاق في الحلبة سيجعل هذا الرقم يفقد دقته بسرعة.
   * أنت تمتلك MPU6050 والذي يعتبر المعيار الذهبي لقياس زاوية الدوران (currentAngleZ).
   * لذلك، يفضل الاعتماد على الـ Encoders لمعرفة المسافة الخطية (متى تقطع التقاطع)،
   * والاعتماد على MPU6050 لمعرفة زاوية الانعطاف (متى تتوقف عن الدوران).
   */
}






