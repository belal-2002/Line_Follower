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

void forwardStraight() {
  // السير للأمام مع تصحيح المسار (P-Controller)
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); 
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);

  // 1. معامل التصحيح (Kp_Straight): 
  // رقم بسيط يحدد شراسة تعديل المسار (ابحث عن القيمة المثالية بالتجربة، غالباً بين 5.0 و 15.0)
  float Kp_Straight = 8.0; 

  // 2. حساب قيمة الانحراف بناءً على زاوية الإنكودر الحالية
  // إذا انحرف الروبوت لليسار ستكون الزاوية موجبة -> فقيمة التصحيح موجبة
  // إذا انحرف لليمين ستكون الزاوية سالبة -> فقيمة التصحيح سالبة
  int correction = currentAngleZ * Kp_Straight; 

  // 3. تطبيق التصحيح الذكي:
  // العجل الأيسر: نضيف له قيمة التصحيح (لزيادة سرعته إذا انحرفنا يساراً ليعيدنا لليمين)
  // العجل الأيمن: نطرح منه قيمة التصحيح
  int leftPWM  = constrain(baseSpeed + correction, 0, maximumSpeed);
  int rightPWM = constrain(baseSpeed - correction, 0, maximumSpeed);

  ledcWrite(PWMA, leftPWM);
  ledcWrite(PWMB, rightPWM);
}

void sweepSearchTurn() {
  if (abs(currentAngleZ) < 180.0) { 
  // المحرك الأيسر للخلف بسرعة الدوران
  digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH);
  ledcWrite(PWMA, (turnSpeed / 4*3)); 
  // المحرك الأيمن للأمام بنفس سرعة الدوران
  digitalWrite(BIN1, LOW);  digitalWrite(BIN2, HIGH); 
  ledcWrite(PWMB, (turnSpeed / 4*3));
  } else { 
    if (abs(currentAngleZ) < 675.0) {
      // المحرك الأيسر للخلف بسرعة الدوران
      digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH);
      ledcWrite(PWMA, turnSpeed); 
      // المحرك الأيمن للأمام بنفس سرعة الدوران
      digitalWrite(BIN1, LOW);  digitalWrite(BIN2, HIGH); 
      ledcWrite(PWMB, (turnSpeed / 4*3));
    } else { 
      sweep180Done = true;
    }
  }
}

void updateDistance() {
  noInterrupts();
  long currentLeftTicks = leftTicks;
  long currentRightTicks = rightTicks;
  interrupts();

  // --- حسابات الإزاحة والزاوية الحالية (كما هي في كودك) ---
  float distanceLeft = currentLeftTicks * distancePerTick;
  float distanceRight = currentRightTicks * distancePerTick;
  distanceNow = (distanceLeft + distanceRight) / 2.0; 
  float absoluteAngle = ((distanceRight - distanceLeft) / trackWidth) * (180.0 / PI);
  currentAngleZ = absoluteAngle - angleOffset;

  // --- الإضافة الجديدة: العداد التراكمي (Odometer) ---
  long deltaLeft = currentLeftTicks - lastLeftTicks_odo;
  long deltaRight = currentRightTicks - lastRightTicks_odo;
  
  // تحديث القيم السابقة للدورة القادمة
  lastLeftTicks_odo = currentLeftTicks;
  lastRightTicks_odo = currentRightTicks;

  // نحسب مقدار الحركة الفعلي (موجب دائماً) ونضيفه للعداد التراكمي
  float deltaDistance = (abs(deltaLeft) + abs(deltaRight)) / 2.0 * distancePerTick;
  totalOdometer += deltaDistance; 
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








