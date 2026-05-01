void loopStrategy3() { //Left
  // إلغاء الدوران الأعمى فور ملامسة حساسات المنتصف للخط
  if (midSensor) { 
    goLeft = false;
    goRight = false;
  }

  // الاستشفاء المبكر (إنهاء الدوران فور التقاط الرادار للخط)
  if (goLeft && leftRadar) { goLeft = false; calculateError(); return; }
  if (goRight && rightRadar) { goRight = false; calculateError(); return; }

  if (((bitRead(sensorBit, 5)) || (bitRead(sensorBit, 6))) && (!leftRadar)) {
    //if (millis() - turnStartTime > 100) {
    turnLeft = false;
    //}
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

  if ((leftRadar == 2) && (midSensor >= 3) && (!rightRadar)) {
    turnLeft = true; 
    leftMotor();
    delay(65);
    turnStartTime = millis();
    lineWasFound = false;
    return;   
  }

  // 1. اكتشاف تقاطع الزائد (+) وتجاوزه مستقيماً
  if (leftRadar && rightRadar && midSensor) {
    // إعماء الرادارين معاً ليمر الروبوت بناءً على حساسات المنتصف فقط
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
  
  calculateError();
}      