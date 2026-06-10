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
    if ((abs(currentAngleZ) >= 70.0) || 
        (abs(currentAngleZ) >= 50.0 && (midSensor))) {
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
    forwardMotor();
    return;
  }

  // --- نقطة تفعيل الدوران ---
  if ((leftMidRadar) && (midMidSensor >= 2) && (!rightMidRadar)) {
    turnLeft = true; 
    //goLeft = true;
    leftMotor();
    // تصفير الزاوية ليبدأ الحساب الدقيق من لحظة اتخاذ قرار الدوران
    currentAngleZ = 0.0; 
    // تمت إزالة delay(85) والاعتماد على الوقت هنا
    lineWasFound = false;
    return;   
  }

  bitClear(sensorBit, 0);
  bitClear(sensorBit, 9);


  calculateError();
  
}