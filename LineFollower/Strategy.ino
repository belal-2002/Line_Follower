// ====================================================================
 // ملف إدارة الاستراتيجيات والدوال المساعدة (Strategy.ino)
 // ====================================================================

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
void cancelTurn_1() {
  if (midMidMidSensor) { 
    if (goLeft || goRight) {
      goLeft = false;
      goRight = false;
      currentError = 0;
      lastError = 0;
      PD_Value = 0;
      resetRadarMemory();
    }

    if (turnLeft || turnRight) {
      if (millis() - turnStartTime >= 250) { 
        turnLeft = false;
        turnRight = false;
        turnCooldownTime = millis();
        currentError = 0;
        lastError = 0;
        PD_Value = 0;
        resetRadarMemory();
      }
    }
  }
}

// التحقق مما إذا كان الروبوت في حالة دوران حالياً
bool isTurning_2() {
  return (turnLeft || turnRight || goLeft || goRight);
}

bool activateGo_3() {
  if (midMidMidSensor == 0) {
    lineWasFound = false;
   
    if (leftMidRadarOn && rightMidRadarOn){
      if (leftRadarOn) { goLeft = true; leftMotor(); resetRadarMemory(); return true; }
      if (rightRadarOn) { goRight = true; rightMotor(); resetRadarMemory(); return true; }
    }
    if (leftMidRadarOn) { goLeft = true; leftMotor(); resetRadarMemory(); return true; }
    if (rightMidRadarOn) { goRight = true; rightMotor(); resetRadarMemory(); return true; }

    return false; 
  }
  return false;
}

bool noLine_4() {
  if (midMidMidSensor == 0) {
    forwardMotor();
    return true;
  }
  return false;
}

bool gap_4() {
  static unsigned long lostTimeStart = 0;
  static bool isSearching = false;
  static unsigned long lineFoundTime = 0;
  static bool verifyingLine = false;

  // أ. في حال التقط الروبوت الخط 
  if (midMidMidSensor > 0) { 
    // إذا كنا في حالة (بحث بالتقدم للأمام)
    if (isSearching) {
      if (!verifyingLine) {
        // بدأنا نرى الخط، نشغل مانع التشويش (10ms)
        verifyingLine = true;
        lineFoundTime = millis();
        forwardMotor(); // نستمر بالتقدم للحفاظ على الزخم
        return true;
      } 
      else {
        // التحقق من مرور 27 ملي ثانية والخط لا يزال موجوداً
        if (millis() - lineFoundTime >= 27) { 
          // تم تأكيد الخط! ننهي البحث ونسلم القيادة
          isSearching = false;
          verifyingLine = false;
          return false; // نرجع false لتنطلق دالة calculateError (PID)
        }
        forwardMotor(); // لم تمر الـ 10ms، نستمر بالتقدم
        return true; 
      }
    }
    return false; // الروبوت يرى الخط بشكل طبيعي ولا يبحث
  }

  // ب. وصلنا هنا يعني أن (midMidMidSensor == 0) والخط مفقود بالكامل
  verifyingLine = false; // تصفير المانع فوراً إذا قطع الخط أثناء الـ 10ms

  if (!isSearching) {
    // اللحظة الأولى لفقدان الخط (بعد أن فشلت رادارات handleLineLoss)
    isSearching = true;
    lostTimeStart = millis();
    forwardMotor();
    return true;
  }

  // ج. مرحلة البحث 
  if (millis() - lostTimeStart < 400) {
    // لم تنتهِ الـ 750 ملي ثانية -> استمر بالتقدم للأمام
    forwardMotor();
    return true;
  } 
  else {
    // انتهى الوقت ولم نجد الخط -> تفعيل الدوران
    goRight = true;
    rightMotor();
    
    // (مهم جداً): ننهي حالة البحث isSearching = false هنا. 
    // لماذا؟ لأن الروبوت الآن "يدور"، والمسؤول عن إيقاف الدوران 
    // سيكون cancelBlindTurn_1، ولا نريد مانع تشويش أثناء الدوران.
    isSearching = false; 
    return true;
  }
}

// نقطة تفعيل دوران حاد جديد (تُرجع true إذا بدأ الدوران)
bool activateTurn_5() {
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
      forwardMotor();     // إبقاء المحركات تدفع للأمام لاختراق الخط وعدم الالتفاف المبكر

      if (millis() - waitStartTime > 250) {
        break; 
      }
    }
    // =================================================================
    
    lineWasFound = false;
    turnLeft = true;
    leftMotor();
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
void cleanIR_6() {
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

// السير للأمام عند وجود خط عريض (+)
bool checkFullLineForward_8() {
  if (midSensor >= 8) { // إذا قرأ 8 حساسات أو أكثر
    forwardMotor(); // استبدلنا forwardStraight بـ forwardMotor
    return true;
  }
  return false;
}

void resetRadarMemory() {
  leftRadarOn = false;
  leftRadarOn2 = false;
  rightRadarOn = false;
  leftMidRadarOn = false;
  rightMidRadarOn = false;
  specialMemory = false;
  checkStateChanges();
}




