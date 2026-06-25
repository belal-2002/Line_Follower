// ====================================================================
// ملف قراءة حساس التوازن (MPU6050) ومعالجة المنحدرات (MPU.ino)
// ====================================================================

void updateMPU() {
  // 1. طلب البيانات من سجلات مقياس التسارع (من سجل 0x3B إلى 0x40 لـ X, Y, Z)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  
  // ====================================================================
  // 2. نظام الحماية من الاهتزازات وفصل الأسلاك (Safety I2C Check)
  // ====================================================================
  // نطلب 6 بايتات، وننفذ القراءة "فقط" إذا أكد الحساس إرسال الـ 6 بايتات بالكامل
  if (Wire.requestFrom(MPU_ADDR, 6, true) == 6) {
    
    // تجميع البايتات في متغيرات 16-بت (Bitwise Shift)
    int16_t ax = Wire.read() << 8 | Wire.read();
    int16_t ay = Wire.read() << 8 | Wire.read();
    int16_t az = Wire.read() << 8 | Wire.read();

    // حساب زاوية الميلان الرأسي اللحظية (Pitch) بالدرجات
    float rawPitch = atan2((float)-ax, sqrt((float)ay * ay + (float)az * az)) * 180.0 / PI;
    
    // تطبيق زاوية الإزاحة لمعرفة الميلان الفعلي مقارنة بنقطة انطلاق الروبوت
    pitchAngle = rawPitch - pitchOffset;
  }
  // إذا فشل الاتصال (اهتزاز سلك)، نتجاهل القراءة في هذه الدورة 
  // ليحتفظ متغير pitchAngle بآخر قراءة سليمة ولا يُعطينا -1 أو أرقاماً جنونية.

  // ====================================================================
  // 3. منطق التحكم الذكي بالسرعة بناءً على التضاريس (Slope Handling)
  // ====================================================================
  
  if (pitchAngle > 10.0) { 
    // أ. حالة الصعود: الروبوت مائل للأعلى -> زيادة السرعة فوراً للتغلب على الجاذبية
    maximumSpeed = originalMaximumSpeed * 2;
    baseSpeed = originalBaseSpeed * 2;
    turnSpeed = originalTurnSpeed * 2;
  } 
  else if (pitchAngle < -10.0) {
    // ب. حالة النزول: الروبوت مائل للأسفل -> تقليل السرعة لمنع الانزلاق أو فقدان الخط بسبب القصور الذاتي
    maximumSpeed = originalMaximumSpeed / 2;
    baseSpeed = originalBaseSpeed / 2;
    turnSpeed = originalTurnSpeed / 2;
  } 
  else {
    // ج. الأرض المستوية: العودة للسرعات القياسية الطبيعية
    maximumSpeed = originalMaximumSpeed;
    baseSpeed = originalBaseSpeed;
    turnSpeed = originalTurnSpeed;
  }
}

// ====================================================================
// دالة أخذ الصفر المرجعي للزاوية (تُستدعى عند تشغيل الروبوت)
// ====================================================================
void resetPitchOffset() {
  float sumPitch = 0;
  int validReadings = 0; // عداد لمعرفة عدد القراءات التي نجحت فعلياً

  // نأخذ 10 قراءات سريعة لضمان دقة نقطة الصفر (تستغرق حوالي 20 ملي ثانية)
  for (int i = 0; i < 10; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B); 
    Wire.endTransmission(false);
    
    // تطبيق نظام الحماية أثناء المعايرة أيضاً!
    if (Wire.requestFrom(MPU_ADDR, 6, true) == 6) {
      int16_t ax = Wire.read() << 8 | Wire.read();
      int16_t ay = Wire.read() << 8 | Wire.read();
      int16_t az = Wire.read() << 8 | Wire.read();

      float rawPitch = atan2((float)-ax, sqrt((float)ay * ay + (float)az * az)) * 180.0 / PI;
      sumPitch += rawPitch;
      validReadings++; // توثيق نجاح القراءة
    }
    delay(2); // تأخير زمني بسيط لضمان استقرار ناقل الـ I2C
  }
  
  // حفظ المتوسط كنقطة مرجعية (بشرط وجود قراءات ناجحة لحماية النظام من القسمة على صفر)
  if (validReadings > 0) {
    pitchOffset = sumPitch / (float)validReadings; 
  } else {
    pitchOffset = 0.0; // إذا كان السلك مفصولاً تماماً، نعتبر الإزاحة صفر للأمان
  }
}