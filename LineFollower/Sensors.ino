

void calculateError() {
  long weightedSum = 0;
  long sum = 0;
  int rawValues[12];
  
  bool leftRadar = false;
  bool rightRadar = false;

  // 1. قراءة جميع الحساسات
  for (int i = 0; i < 12; i++) {
    rawValues[i] = analogRead(sensorPins[i]);
  }

  // 2. فحص رادارات المنعطفات الحادة (إشارة رقمية استنتاجية)
  // نعتبر أن زاوية 90 موجودة إذا قرأ حساس أقصى اليمين أو أقصى اليسار اللون الأسود
  if (rawValues[0] > 1700 || rawValues[1] > 1700) leftRadar = true;
  if (rawValues[10] > 1700 || rawValues[11] > 1700) rightRadar = true;

  // 3. تحديد الحالة الهجينة (State Machine)
  if (leftRadar && !rightRadar) {
    currentState = SHARP_LEFT;
    isLineLost = false;
    return; // خروج فوري: لا تحسب PD أثناء المنعطف الحاد
  } 
  else if (rightRadar && !leftRadar) {
    currentState = SHARP_RIGHT;
    isLineLost = false;
    return; // خروج فوري
  } 
  else {
    currentState = NORMAL_PD; // المسار طبيعي (مستقيم أو انحناء ناعم)
  }

  // 4. حساب الـ PD باستخدام الحساسات المركزية الـ 8 فقط (Index 2 إلى 9)
  for (int i = 2; i <= 9; i++) {
    if (rawValues[i] > 1700) {
      weightedSum += (long)rawValues[i] * sensorWeights[i];
      sum += rawValues[i];
    }
  }

  // 5. تحديث الخطأ وحالة الفقدان
  if (sum > 0) {
    currentError = (float)weightedSum / (float)sum;
    isLineLost = false;
  } else {
    isLineLost = true; // الروبوت فقد الخط كلياً في المسار الطبيعي
  }
}