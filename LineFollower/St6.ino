void loopStrategy6() { //Right 

  // إلغاء الدوران الأعمى فور ملامسة حساسات المنتصف للخط
  if (midMidSensor) { 
    if (goLeft || goRight){
      leftRadarOn = false;
      rightRadarOn = false;
      leftMidRadarOn = false;
      rightMidRadarOn = false;
    }
    goLeft = false;
    goRight = false;
  }
  
  // الاستشفاء المبكر
  if (goRight && rightMidRadar) { goRight = false; calculateError(); return; }
  if (goLeft && leftMidRadar) { goLeft = false; calculateError(); return; }

  // --- التعديل الجديد باستخدام MPU6050 بدلاً من الوقت ---
  if (turnRight) {
    if ((abs(currentAngleZ) >= 50.0) || 
        (abs(currentAngleZ) >= 40.0 && (midMidSensor))||
        (millis() - turnStartTime >= 400)) {  // <-- إضافة شرط الـ 400 ملي ثانية هنا
      turnRight = false;
    }
  }
  
  // الاستمرار في الدوران إذا بدأناه
  if (turnRight || goLeft || goRight) return;

  // الدخول في حالة الفقدان الكلي للخط
  if (!midSensor) {
    lineWasFound = false;
    if (rightMidRadarOn) {
      goRight = true;
      rightMotor();
      return;
    }
    if (leftMidRadarOn) {
      goLeft = true; 
      leftMotor();
      return;
    }
    LineNotFoundTime = millis();
    if ((totalOdometer - lostLineDistance) > gapDistance) {
      if (!Turn180now) {  // الاستشفاء عبر الدوران الموضعي 180 درجة (الملاذ الأخير)
        Turn180now = true;  // إذا لم نكن في حالة الدوران، نبدأها الآن
        sweep180Done = false;
        resetAngleZ(); // تصفير الزاوية لبدء حساب 180 درجة
        sweepSearchTurn(); // بدء الدوران 
        return;
      } else {  
        if (sweep180Done) {  // نحن الآن في منتصف عملية الدوران، نتحقق من الزاوية
        //if (abs(currentAngleZ) >= 180.0) {  // نحن الآن في منتصف عملية الدوران، نتحقق من الزاوية
            // اكتملت الـ 180 درجة ولم يجد الخط
            forwardMotor(); // الاتجاه للأمام كما طلبت
            return;
        } else {
            // لم يكمل 180 درجة بعد، استمر بالدوران حول نفسه
            sweepSearchTurn();
            return;
          }
        }
    } else { 
      forwardMotor();
      return;  
    }
  }
  if (millis() - LineNotFoundTime >= 10) {
    Turn180now = false; // إنهاء حالة البحث
    sweep180Done = false;
    lostLineDistance = totalOdometer;
  }

  // --- نقطة تفعيل الدوران ---
  if ((rightMidRadar) && (midMidSensor >= 4) && (!leftRadar)) {
    turnRight = true; 
    //goRight = true;
    rightMotor();
    
    // تصفير الزاوية لحساب الـ 60 درجة بشكل دقيق
    resetAngleZ(); 
    // تمت إزالة delay(85) والاعتماد على الوقت هنا

    turnStartTime = millis(); 
    lineWasFound = false;
    return;   
  }

  bitClear(sensorBit, 0);
  bitClear(sensorBit, 9);


  calculateError();
}