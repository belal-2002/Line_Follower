void loopStrategy4() { //Right 
  // إلغاء الدوران الأعمى فور ملامسة حساسات المنتصف للخط
  if (midSensor) { 
    goLeft = false;
    goRight = false;
  }
  
  // الاستشفاء المبكر
  if (goRight && rightRadar) { goRight = false; calculateError(); return; }
  if (goLeft && leftRadar) { goLeft = false; calculateError(); return; }

  // --- التعديل الجديد باستخدام MPU6050 بدلاً من الوقت ---
  if (turnRight) {
    if ((abs(currentAngleZ) >= 70.0) || 
        (abs(currentAngleZ) >= 50.0 && (bitRead(sensorBit, 5) || bitRead(sensorBit, 6)))) {
      turnRight = false;
    }
  }
  
  // الاستمرار في الدوران إذا بدأناه
  if (turnRight || goLeft || goRight) return;

  // الدخول في حالة الفقدان الكلي للخط
  if (!allSensor) {
    lineWasFound = false;
    if (rightRadarOn) {
      goRight = true;
      rightMotor();
      return;
    }
    if (leftRadarOn) {
      goLeft = true; 
      leftMotor();
      return;
    }
    forwardMotor();
    return;
  }

  // --- نقطة تفعيل الدوران ---
  if ((rightRadar) && (midSensor >= 2) && (!leftRadar)) {
    turnRight = true; 
    //goRight = true;
    rightMotor();
    
    // تصفير الزاوية لحساب الـ 60 درجة بشكل دقيق
    resetAngleZ(); 
    // تمت إزالة delay(85) والاعتماد على الوقت هنا
    
    lineWasFound = false;
    return;   
  }


  /*
  if (leftRadar && rightRadar && midSensor) {
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
    bitClear(sensorBit, 10);
    bitClear(sensorBit, 11);
  }
  else if (leftRadar && midSensor) {
    bitClear(sensorBit, 10);
    bitClear(sensorBit, 11);
  }
  */
  calculateError();
}