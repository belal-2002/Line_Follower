void loopStrategy5() { //Left

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
  if (goLeft && leftMidRadar) { goLeft = false; calculateError(); return; }
  if (goRight && rightMidRadar) { goRight = false; calculateError(); return; }

  // --- التعديل الجديد باستخدام MPU6050 بدلاً من الوقت ---
  if (turnLeft) {
    /*
    if (midMidSensor) { 
      leftRadarOn = false;
      rightRadarOn = false;
      leftMidRadarOn = false;
      rightMidRadarOn = false;
    }
    */
    if ((abs(currentAngleZ) >= 50.0) || 
        (abs(currentAngleZ) >= 40.0 && (midSensor)) ||
        (millis() - turnStartTime >= 400)) {  // <-- إضافة شرط الـ 400 ملي ثانية هنا
      turnLeft = false;
    }
  }
  
  // الاستمرار في الدوران إذا بدأناه
  if (turnLeft || goLeft || goRight) return;

  // الدخول في حالة الفقدان الكلي للخط
  if (!midSensor) {
    lineWasFound = false;
    if (leftMidRadarOn) {
      goLeft = true; 
      leftMotor();
      return;
    }
    if (rightMidRadarOn) {
      goRight = true;
      rightMotor();      
      return;
    }
    LineNotFoundTime = millis();
    if ((distanceNow - lostLineDistance) > gapDistance) {
      if (!recoveryTurn180) {  // الاستشفاء عبر الدوران الموضعي 180 درجة (الملاذ الأخير)
        recoveryTurn180 = true;  // إذا لم نكن في حالة الدوران، نبدأها الآن
        resetAngleZ(); // تصفير الزاوية لبدء حساب 180 درجة
        spinTurn180(); // بدء الدوران الموضعي
        return;
      } else {        
        if (abs(currentAngleZ) >= 180.0) {  // نحن الآن في منتصف عملية الدوران، نتحقق من الزاوية
            // اكتملت الـ 180 درجة ولم يجد الخط
            forwardMotor(); // الاتجاه للأمام كما طلبت
            return;
        } else {
            // لم يكمل 180 درجة بعد، استمر بالدوران حول نفسه
            spinTurn180();
            return;
          }
        }
    } else { 
      forwardMotor();
      return;  
    }
  }
  if (millis() - LineNotFoundTime >= 10) {
    recoveryTurn180 = false; // إنهاء حالة البحث
    LineNotFoundStartDistance = distanceNow;
  }

  // --- نقطة تفعيل الدوران ---
  if ((leftMidRadar) && (midMidSensor >= 4) && (!rightMidRadar)) {
    turnLeft = true; 
    //goLeft = true;
    leftMotor();
    // تصفير الزاوية ليبدأ الحساب الدقيق من لحظة اتخاذ قرار الدوران
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