void moveMotor() {
  // إرجاع المحركات للاتجاه الأمامي الطبيعي
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
      
  // تقييد السرعة وإرسالها للمحركات (10-bit)
  leftMotorSpeed  = constrain(baseSpeed + PD_Value, 0, maximumSpeed);
  rightMotorSpeed = constrain(baseSpeed - PD_Value, 0, maximumSpeed);

  ledcWrite(PWMA, leftMotorSpeed);
  ledcWrite(PWMB, rightMotorSpeed);

  leftSpeed = leftMotorSpeed;
  rightSpeed = rightMotorSpeed; 
  updateDistance();
}

void leftMotor() {
  // الدوران لليسار (Left Turn)
 // 1. المحرك الأيسر (الداخلي) يعمل كمرساة ويدور للخلف بسرعة منخفضة
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
  ledcWrite(PWMA, innerTurnSpeed); // <-- تم التعديل للسرعة التفاضلية
      
  // 2. المحرك الأيمن (الخارجي) يستمر بالدفع للأمام بالسرعة العالية المحددة
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); 
  ledcWrite(PWMB, turnSpeed);      // 600

  leftSpeed = 0;
  rightSpeed = 0; 
  updateDistance();
}

void rightMotor() {
  // الدوران لليمين (Right Turn)
  // 1. المحرك الأيسر (الخارجي) يستمر بالدفع للأمام بالسرعة العالية المحددة
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); 
  ledcWrite(PWMA, turnSpeed);      // 600
      
  // 2. المحرك الأيمن (الداخلي) يعمل كمرساة ويدور للخلف بسرعة منخفضة
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); 
  ledcWrite(PWMB, innerTurnSpeed); // <-- تم التعديل للسرعة التفاضلية

  leftSpeed = 0;
  rightSpeed = 0; 
  updateDistance();
}

void stopMotor() {
  // توقف تام
  digitalWrite(STBY, LOW);  
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, LOW); 
  digitalWrite(BIN1, LOW);  digitalWrite(BIN2, LOW);  
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);

  leftSpeed = 0;
  rightSpeed = 0; 
  updateDistance();
}

void forwardMotor() {
  // السير في خط مستقيم
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);

  ledcWrite(PWMA, baseSpeed);
  ledcWrite(PWMB, baseSpeed);

  leftSpeed = leftMotorSpeed;
  rightSpeed = rightMotorSpeed; 
  updateDistance();
}

void updateDistance() {
  unsigned long currentMicros = micros();
  // حساب فرق الزمن بالثواني
  float dist_dt = (currentMicros - lastDistTime) / 1000000.0;
  lastDistTime = currentMicros;

  // حماية من القيم الشاذة عند بداية التشغيل
  if (dist_dt > 0.1) dist_dt = 0.001; 

  // حساب متوسط السرعة الحالية للمحركين (كقيمة مطلقة لأننا نهتم بالمسافة بغض النظر عن الاتجاه)
  float avgSpeedPWM = (abs(leftSpeed) + abs(rightSpeed)) / 2.0;

  // تحويل PWM إلى سرعة (سم/ثانية) ثم حساب المسافة المقطوعة (المسافة = السرعة × الزمن)
  float speedCmPerSec = avgSpeedPWM * pwmToCmFactor;
  float distanceMoved = speedCmPerSec * dist_dt;

  distanceNow += distanceMoved; // إضافة المسافة الجديدة للمسافة الإجمالية
}






