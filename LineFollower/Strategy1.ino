void loopStrategy1() { 
  // إلغاء الدوران الأعمى فور ملامسة حساسات المنتصف للخط
  if (midSensor) { 
    goLeft = false;
    goRight = false;
  }
  
  // الاستشفاء المبكر (إنهاء الدوران فور التقاط الرادار للخط)
  if (goRight && rightRadar) { goRight = false; calculateError(); return; }
  if (goLeft && leftRadar) { goLeft = false; calculateError(); return; }
  
  // الاستمرار في الدوران إذا بدأناه
  if (goLeft || goRight) return;

  // الدخول في حالة الفقدان الكلي للخط
  if (!allSensor) {
    lineWasFound = false;
    if (rightRadarOn) {
      caseMotor = 2;
      goRight = true;
      return;
    }
    if (leftRadarOn) {
      caseMotor = 1;
      goLeft = true; 
      return;
    }
  }

  // تجاوز التقاطعات العرضية بأمان
  if (radar && midSensor){
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
    bitClear(sensorBit, 10);
    bitClear(sensorBit, 11);
  }
  
  calculateError();
}      