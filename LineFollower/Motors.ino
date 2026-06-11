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
  ledcWrite(PWMA, innerTurnSpeed); // <-- تم التعديل للسرعة التفاضلية
      
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
  ledcWrite(PWMB, innerTurnSpeed); // <-- تم التعديل للسرعة التفاضلية
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

  // 3. المسافة الإجمالية لمركز الروبوت (للرادار)
  distanceNow = (distanceLeft + distanceRight) / 2.0;

  // 4. حساب زاوية الدوران المطلقة بالراديان ثم تحويلها لدرجات
  // الدوران لليسار سينتج زاوية موجبة، ولليمين زاوية سالبة (مطابق لنظام MPU)
  float absoluteAngle = ((distanceRight - distanceLeft) / trackWidth) * (180.0 / PI);
  
  // 5. تحديث الزاوية الحالية بناءً على نقطة التصفير
  currentAngleZ = absoluteAngle - angleOffset;
}

// دالة نستخدمها بدلاً من (currentAngleZ = 0.0) لتصفير الزاوية دون فقدان المسافة
void resetAngleZ() {
  noInterrupts();
  long currentLeftTicks = leftTicks;
  long currentRightTicks = rightTicks;
  interrupts();

  float distanceLeft = currentLeftTicks * distancePerTick;
  float distanceRight = currentRightTicks * distancePerTick;

  // نجعل الإزاحة تساوي الزاوية المطلقة الحالية، فتصبح currentAngleZ صفر
  angleOffset = ((distanceRight - distanceLeft) / trackWidth) * (180.0 / PI);
  currentAngleZ = 0.0;
}








