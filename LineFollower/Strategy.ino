// ====================================================================
 // ملف إدارة الاستراتيجيات والدوال المساعدة (Strategy.ino)
 // ====================================================================

 static bool zeroAngleZ = false; // متغير لحفظ حالة تصفير الزاوية للمسار العريض
 unsigned long turnCooldownTime = 0; // متغير لحفظ زمن بدء فترة الحصانة

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
  if (midMidMidSensor) { 
    if (turnLeft || turnRight || goLeft || goRight){
      resetRadarMemory();
      currentError = 0;
      lastError = 0;
      PD_Value = 0;

      goLeft = false;
      goRight = false;
      
      if ((millis() - turnStartTime >= 250) && ((turnLeft) || (turnRight)) { 
      turnLeft = false;
      turnRight = false;
      turnCooldownTime = millis(); // <-- إضافة: بدء فترة الحصانة فور الخروج
      resetRadarMemory();
    }
  }
}

// التحقق مما إذا كان الروبوت في حالة دوران حالياً
bool isCurrentlyTurning_4() {
  return (turnLeft || turnRight || goLeft || goRight);
}

// معالجة الفقدان الكلي للخط (تُرجع true إذا تصرفت)
bool handleLineLoss_5() {
  if (midMidMidSensor == 0) {
    lineWasFound = false;
    if (leftMidRadarOn && rightMidRadarOn){
      if (leftRadarOn) {
        goLeft = true;
        leftMotor();
        resetRadarMemory();
        return true;
      }
      if (rightRadarOn) {
        goRight = true;
        rightMotor();
        resetRadarMemory();
        return true;
      }
    }
    if (leftMidRadarOn) {
      goLeft = true;
      leftMotor();
      resetRadarMemory();
      return true;
    }
    if (rightMidRadarOn) {
      goRight = true;
      rightMotor();
      resetRadarMemory();
      return true;
    }
    //stopMotor();
    forwardMotor();
    return true;
  }
  return false;
}

// نقطة تفعيل دوران حاد جديد (تُرجع true إذا بدأ الدوران)
bool activateTurn_6() {
  if (millis() - turnCooldownTime < 250) {
    return false; 
  }
  if ((specialMemory && (leftRadar == 1)) ||
      ( leftRadarOn2 && (leftMidRadar == 1) && (bitRead(sensorBit, 7) == 1) && (bitRead(sensorBit, 6) == 1) &&
      (rightRadar == 0) && (rightMidRadar == 0) && (bitRead(sensorBit, 2) == 0) && (bitRead(sensorBit, 3) == 0) )){
    // =================================================================
    // التعديل الجديد: حبس الكود حتى ينطفئ الحساس (تجاوز عرض الخط)
    // =================================================================
    
    // 1. أخذ نقطة مرجعية للوقت والمسافة كعامل أمان (Failsafe)
    unsigned long waitStartTime = millis();

    // 2. حلقة الانتظار: الكود سيبقى عالقاً هنا ولن ينفذ أي شرط آخر في أي مكان
    while ((leftMidRadar == 1) || (leftRadar == 1)) {
      loopSensors();      // تحديث قراءات الحساسات الحية لاكتشاف لحظة انطفاء leftMidRadar
      updateDistance();   // تحديث الأودومتري (الإنكودرات) لضمان عدم ضياع نبضات العجلات أثناء الانتظار
      forwardMotor();     // إبقاء المحركات تدفع للأمام لاختراق الخط وعدم الالتفاف المبكر

      if (millis() - waitStartTime > 250) {
        break; 
      }
    }
    // =================================================================
    
    lineWasFound = false;
    turnLeft = true;
    trackTurnLeftState("activateTurn_6");/////////// 
    leftMotor();
    resetAngleZ(); // تصفير زاوية الـ MPU
    turnStartTime = millis();
    resetRadarMemory();
    turnRight = false;
    goLeft = false;
    goRight = false;
    return true;  
  }
  return false;
}


// تنظيف البتات لتجاهل التقاطعات (تم تصحيح البتات 10 و 11 الكارثية!)
void ignoreIntersections_7() {
  // 1. اكتشاف تقاطع الزائد (+) وتجاوزه مستقيماً
  if (leftMidRadar && rightMidRadar && midMidSensor) {
  sensorValue[1] = 0; 
  sensorValue[8] = 0;
  }
  // 2. تجاهل الفخاخ اليمنى (حرف T المتجه لليمين)
  else if (rightMidRadar && midMidSensor) {
    sensorValue[1] = 0; 
  }
  // 3. تجاهل الفخاخ اليسرى (ميزة إضافية مجانية!)
  else if (leftMidRadar && midMidSensor) {
    sensorValue[8] = 0;
  }
  if (bitRead(sensorBit, 4) && bitRead(sensorBit, 5)){
    sensorValue[1] = 0; 
    sensorValue[2] = 0;
    sensorValue[3] = 0; 
    sensorValue[6] = 0;
    sensorValue[7] = 0; 
    sensorValue[8] = 0;
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


void trackTurnLeftState(String locationName) {
  unsigned long currentTime = millis();
  
  if (turnLeft != lastTurnLeftState) {
    turnLeftChangeCount++; 
    unsigned long duration = currentTime - lastTurnLeftChangeTime; 
    
    TelnetStream.print("turnLeft: ");
    TelnetStream.print(turnLeft ? "ON" : "OFF"); 
    
    // الميزة الجديدة: طباعة اسم الدالة التي تسببت في التغيير
    TelnetStream.print(" | Changed By: ");
    TelnetStream.print(locationName); 
    
    TelnetStream.print(" | Changes Count: ");
    TelnetStream.print(turnLeftChangeCount);     
    TelnetStream.print(" | Time elapsed: ");
    TelnetStream.print(duration);
    TelnetStream.println(" ms");
    
    lastTurnLeftState = turnLeft;
    lastTurnLeftChangeTime = currentTime;
  }
}

void resetRadarMemory() {
  leftRadarOn = false;
  leftRadarOn2 = false;
  rightRadarOn = false;
  leftMidRadarOn = false;
  rightMidRadarOn = false;
  specialMemory = false;
}







