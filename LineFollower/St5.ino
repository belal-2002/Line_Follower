void loopStrategy5() { //Left

  // إلغاء الدوران الأعمى فور ملامسة حساسات المنتصف للخط
  if (midMidSensor) { 
    goLeft = false;
    goRight = false;
  }

  // الاستشفاء المبكر
  if (goLeft && leftMidRadar) { goLeft = false; calculateError(); return; }
  if (goRight && rightMidRadar) { goRight = false; calculateError(); return; }

  // --- التعديل الجديد باستخدام MPU6050 بدلاً من الوقت ---
  if (turnLeft) {
    // ننهي الدوران الإجباري في إحدى الحالتين:
    // 1. الروبوت دار 60 درجة بالكامل بناءً على الحساس.
    // 2. الروبوت تجاوز 40 درجة (كحد أمان أولي) والتقطت حساسات المنتصف الخط مجدداً.
    if ((abs(currentAngleZ) >= 70.0) || 
        (abs(currentAngleZ) >= 50.0 && (bitRead(sensorBit, 5) || bitRead(sensorBit, 6)))) {
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
  bitClear(sensorBit, 1);
  bitClear(sensorBit, 10);
  bitClear(sensorBit, 11);

  calculateError();
  
}