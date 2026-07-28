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

// إلغاء الدوران أو السير المستقيم الأعمى فور ملامسة حساسات المنتصف للخط
void cancelTurn_1() {

  if (midSensor && straight){
    straight = false; // إيقاف حالة المستقيم
  }

  if (midMidMidSensor) {
    // تمت إضافة straight هنا ضمن الشرط
    if (goLeft || goRight || straight) { 
      goLeft = false;
      goRight = false;
      straight = false; // إيقاف حالة المستقيم
      currentError = 0;
      lastError = 0;
      PD_Value = 0;
      resetRadarMemory();
      //stop();
    }
    if (turnLeft || turnRight) {
      if (millis() - turnStartTime >= 290) {
        turnLeft = false;
        turnRight = false;
        turnCooldownTime = millis();
        currentError = 0;
        lastError = 0;
        PD_Value = 0;
        resetRadarMemory();
        //stop();
      }
    }
  }
}


// التحقق مما إذا كان الروبوت في حالة دوران أو عبور تقاطع حالياً
bool isTurning_2() {
  // تمت إضافة straight للتحقق من الحالة
  return (turnLeft || turnRight || goLeft || goRight || straight);
}

// قمنا بإزالة = false من هنا لأننا أعلنا عنها في الملف الرئيسي
bool activateGo_3(bool isStrategy1) {
  if (midMidMidSensor == 0) {
    lineWasFound = false;
      // نتحقق: هل نحن في الاستراتيجية الأولى؟ وهل العداد لم يصل للرقم 5 بعد؟
      if (isStrategy1 && (straightCounter < 4) ) {
        straight = true;
        forwardMotor();
        resetRadarMemory();
        straightCounter++; // زيادة العداد بمقدار 1
        return true;
      }
    // إذا التقط حساس اليسار نقطة التقاطع
    if (leftMidRadarOn) {
        goLeft = true;
        leftMotor();
        resetRadarMemory();
        return true;
      }
    
    // حساس اليمين يبقى يعمل كالمعتاد في جميع الحالات
    if (rightMidRadarOn) { 
      goRight = true; 
      rightMotor(); 
      resetRadarMemory(); 
      return true; 
    }
    
    return false;
  }
  return false;
}

bool noLine_4() {
  if (midMidMidSensor == 0) {
    //stopMotor();
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
        // التحقق من مرور 11 ملي ثانية والخط لا يزال موجوداً
        if (millis() - lineFoundTime >= 20) { 
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
  if (millis() - lostTimeStart < 200) {
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
if (
      (specialMemory && (leftOutRadar == 1)) ||
      (leftOutRadarOn2 && (leftMidRadar == 1) && (bitRead(sensorBit, 8) == 1) && (rightMidRadar == 0) && (rightOutRadar == 0)) ||
      (leftMidRadar && leftRadar && (bitRead(sensorBit, 8) == 1) && (bitRead(sensorBit, 7) == 1) && (rightMidRadar == 0) && (bitRead(sensorBit, 3) == 0) && (bitRead(sensorBit, 4) == 0) && (bitRead(sensorBit, 5) == 0))
) {
    // ضع الكود المراد تنفيذه هنا

    // =================================================================
    // التعديل الجديد: حبس الكود حتى ينطفئ الحساس (تجاوز عرض الخط)
    // =================================================================
    
    // 1. أخذ نقطة مرجعية للوقت والمسافة كعامل أمان (Failsafe)
    unsigned long waitStartTime = millis();

    // 2. حلقة الانتظار: الكود سيبقى عالقاً هنا ولن ينفذ أي شرط آخر في أي مكان
    while (leftOutRadar == 1) {
      loopSensors();      // تحديث قراءات الحساسات الحية لاكتشاف لحظة انطفاء leftMidRadar
      forwardMotor();     // إبقاء المحركات تدفع للأمام لاختراق الخط وعدم الالتفاف المبكر

      if (millis() - waitStartTime > 220) {
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

// تنظيف البتات لتجاهل التقاطعات
void cleanIR_6() {
  // 1. اكتشاف تقاطع الزائد (+) وتجاوزه مستقيماً
  if (leftMidRadar && rightMidRadar && midMidSensor) {
    sensorValue[2] = 0; // تصفير S3 (كما طلبت)
    sensorValue[9] = 0; // تصفير S10 (كما طلبت)
  }
  // 2. تجاهل الفخاخ اليمنى
  else if (rightMidRadar && midMidSensor) {
    sensorValue[2] = 0; // تصفير S3
  }
  // 3. تجاهل الفخاخ اليسرى
  else if (leftMidRadar && midMidSensor) {
    sensorValue[9] = 0; // تصفير S10
  }
  
  // فلتر التشويش المركزي: إذا كانت الحساسات المركزية S6 و S7 ترى الخط (البت 6 و 5)
  if (bitRead(sensorBit, 6) && bitRead(sensorBit, 5)){
    // نصفر الحساسات المجاورة للرادارات لضمان الهدوء
    sensorValue[2] = 0; // S3
    sensorValue[3] = 0; // S4
    sensorValue[4] = 0; // S5
    sensorValue[7] = 0; // S8
    sensorValue[8] = 0; // S9
    sensorValue[9] = 0; // S10
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
  leftOutRadarOn = false; 
  leftOutRadarOn2 = false; 
  rightOutRadarOn = false; 
  leftRadarOn = false;
  rightRadarOn = false;
  leftMidRadarOn = false;
  rightMidRadarOn = false;
  specialMemory = false;
}

void stop() {
  stopMotor();
  delay(30);
  digitalWrite(STBY, HIGH);
    
  resetRadarMemory();
  goLeft = false;
  goRight = false;
  turnLeft = false;
  turnRight = false;
  currentError = 0;
  lastError = 0;
  PD_Value = 0;
  lineWasFound = true;
  Turn180now = false;
  sweep180Done = false;
  turnCooldownTime = 0;
}

// ====================================================================
// دالة إعادة ضبط النظام عند حدوث تبديل في الألوان
// ====================================================================
void executeInversionReset() {
    // 1. إيقاف المحركات تماماً
    stopMotor();
    
    // 2. إطلاق نغمة التنبيه (كما في الكود القديم)
    tone(buzzerPin, 3000, 90);
    
    // 3. توقيف المعالج بالكامل لمدة 10 ملي ثانية (حسب طلبك)
    delay(60);
    
    // 4. إعادة تفعيل درايفر المحركات
    digitalWrite(STBY, HIGH);
    
    // 5. تصفير جميع الأعلام والمتغيرات الملاحية (باستثناء pitchOffset)
    resetRadarMemory();
    goLeft = false;
    goRight = false;
    turnLeft = false;
    turnRight = false;
    currentError = 0;
    lastError = 0;
    PD_Value = 0;
    lineWasFound = true;
    Turn180now = false;
    sweep180Done = false;
    turnCooldownTime = 0;

    // 6. تصفير عدادات التبديل نفسها لتبدأ نظيفة للمرة القادمة
    invBlackCounter = 0;
    invWhiteCounter = 0;
    invIsCounting = false;
    invMissedLoops = 0;
}






