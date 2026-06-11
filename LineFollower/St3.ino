void loopStrategy3() { //Left
  // إلغاء الدوران الأعمى فور ملامسة حساسات المنتصف للخط
  if (midSensor) { 
    goLeft = false;
    goRight = false;
  }

  // الاستشفاء المبكر
  if (goLeft && leftRadar) { goLeft = false; calculateError(); return; }
  if (goRight && rightRadar) { goRight = false; calculateError(); return; }

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
  if (!allSensor) {
    lineWasFound = false;
    if (leftRadarOn) {
      goLeft = true; 
      leftMotor();
      return;
    }
    if (rightRadarOn) {
      goRight = true;
      rightMotor();      
      return;
    }
    forwardMotor();
    return;
  }

  // --- نقطة تفعيل الدوران ---
  if ((leftRadar) && (midSensor >= 2) && (!rightRadar)) {
    turnLeft = true; 
    //goLeft = true;
    leftMotor();
    // تصفير الزاوية ليبدأ الحساب الدقيق من لحظة اتخاذ قرار الدوران
    resetAngleZ(); 
    // تمت إزالة delay(85) والاعتماد على الوقت هنا
    lineWasFound = false;
    return;   
  }


  /*
  // 1. اكتشاف تقاطع الزائد (+) وتجاوزه مستقيماً
  if (leftRadar && rightRadar && midSensor) {
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
    bitClear(sensorBit, 10);
    bitClear(sensorBit, 11);
  }
  // 2. تجاهل الفخاخ اليمنى (حرف T المتجه لليمين)
  else if (rightRadar && midSensor) {
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
  }
  */
  calculateError();
}