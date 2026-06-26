// ====================================================================
 // ملف إدارة الاستراتيجيات والدوال المساعدة (Strategy.ino)
 // ====================================================================

 static bool zeroAngleZ = false; // متغير لحفظ حالة تصفير الزاوية للمسار العريض

 // --------------------------------------------------------------------
 // الموزع الرئيسي للاستراتيجيات
 // --------------------------------------------------------------------
void loopStrategy() {
  switch (strategy) {
    case 0: loopStrategy0(); break; // 000
    case 1: loopStrategy1(); break; // 001
    case 2: loopStrategy2(); break; // 010
    case 3: loopStrategy3(); break; // 011
    case 4: loopStrategy4(); break; // 100
    case 5: loopStrategy5(); break; // 101
    case 6: loopStrategy6(); break; // 110
    case 7: loopStrategy7(); break; // 111
  }
}

// ====================================================================
// الدوال المساعدة (Helper Functions) لتوحيد أكواد السباق
// ====================================================================

//  إلغاء الدوران الأعمى فور ملامسة حساسات المنتصف للخط
void cancelBlindTurn_1() {
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
}

// الاستشفاء المبكر (تُرجع true إذا تم إنهاء الدوران)
bool checkEarlyRecovery_2() {
  if (goLeft && leftMidRadar) { goLeft = false; calculateError(); return true; }
  if (goRight && rightMidRadar) { goRight = false; calculateError(); return true; }
  return false;
}

// فحص اكتمال الدوران المبرمج باستخدام زاوية والوقت
void checkMPUTurnCompletion_3(float maxAngle, float minAngle, unsigned long timeOut) {
  // فحص اكتمال دوران اليسار
  if (turnLeft) {
    if ((abs(currentAngleZ) >= maxAngle) || 
        (abs(currentAngleZ) >= minAngle && midMidSensor) ||
        (millis() - turnStartTime >= timeOut)) {  
      turnLeft = false;
    }
  }
  // فحص اكتمال دوران اليمين
  if (turnRight) {
    if ((abs(currentAngleZ) >= maxAngle) || 
        (abs(currentAngleZ) >= minAngle && midMidSensor) ||
        (millis() - turnStartTime >= timeOut)) {  
      turnRight = false;
    }
  }
}

// التحقق مما إذا كان الروبوت في حالة دوران حالياً
bool isCurrentlyTurning_4() {
  return (turnLeft || turnRight || goLeft || goRight);
}

// معالجة الفقدان الكلي للخط (تُرجع true إذا تصرفت)
bool handleLineLoss_5() {
  if (!midSensor) {
    lineWasFound = false;
    if (leftMidRadarOn) {
      goLeft = true; 
      leftMotor();
      return true;
    }
    if (rightMidRadarOn) {
      goRight = true;
      rightMotor();      
      return true;
    }
    forwardMotor();
    return true;
  }
  return false;
}



// نقطة تفعيل دوران حاد جديد (تُرجع true إذا بدأ الدوران)
bool activateTurn_6(bool isLeftTurn, bool triggerRadar, byte midCondition, bool oppositeRadar) {
  if ((leftMidRadar) && (midMidSensor >= 3) && (!rightRadar)) {
    turnLeft = true; 
    leftMotor();
    resetAngleZ(); // تصفير زاوية الـ MPU
    turnStartTime = millis();
    lineWasFound = false;
    return true;  
  }
  return false;
}


// تنظيف البتات لتجاهل التقاطعات (تم تصحيح البتات 10 و 11 الكارثية!)
void ignoreIntersections_7() {
  // 1. اكتشاف تقاطع الزائد (+) وتجاوزه مستقيماً
  if (leftRadar && rightRadar && midSensor) {
    bitClear(sensorBit, 0); // يمين خارجي
    bitClear(sensorBit, 1); // يمين داخلي
    bitClear(sensorBit, 9); // يسار خارجي (تم تصحيحها)
    bitClear(sensorBit, 8); // يسار داخلي (تم تصحيحها)
  }
  // 2. تجاهل الفخاخ اليمنى (حرف T المتجه لليمين)
  else if (rightRadar && midSensor) {
    bitClear(sensorBit, 0);
    bitClear(sensorBit, 1);
  }
  // 3. تجاهل الفخاخ اليسرى (ميزة إضافية مجانية!)
  else if (leftRadar && midSensor) {
    bitClear(sensorBit, 9);
    bitClear(sensorBit, 8);
  }
}

// السير للأمام بالإنكودر فقط عند وجود خط عريض (+)
bool checkFullLineForward_8() {
  if (midSensor >= 8) { // إذا قرأ 8 حساسات أو أكثر
    if (!zeroAngleZ) {
      resetAngleZ();
      zeroAngleZ = true;
    }
    forwardStraight();
    return true;
  }
  zeroAngleZ = false;
  return false;
}