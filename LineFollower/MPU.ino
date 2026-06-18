void updateMPU() {
  // قراءة سجلات مقياس التسارع (من سجل 0x3B إلى 0x40 لـ X, Y, Z)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  
  int16_t ax = Wire.read() << 8 | Wire.read();
  int16_t ay = Wire.read() << 8 | Wire.read();
  int16_t az = Wire.read() << 8 | Wire.read();

  // حساب زاوية الميلان الرأسي (Pitch) بالدرجات
  float rawPitch = atan2((float)-ax, sqrt((float)ay * ay + (float)az * az)) * 180.0 / PI;
  pitchAngle = rawPitch - pitchOffset;

  // --- منطق التحكم بالسرعة الذكي حسب حالة المنحدر ---
  
  if (pitchAngle > 10.0) { 
    // حالة الصعود: الروبوت مائل للأعلى -> زيادة السرعة فوراً للمساعدة في التغلب على الجاذبية
    maximumSpeed = originalMaximumSpeed * 2;
    baseSpeed = originalBaseSpeed * 2;
    turnSpeed = originalTurnSpeed * 2;
  } 
  else if (pitchAngle < -10.0) {
    // حالة النزول: الروبوت مائل للأسفل -> تقليل السرعة لمنع الانزلاق العنيف أو فقدان الخط
    maximumSpeed = originalMaximumSpeed / 2;
    baseSpeed = originalBaseSpeed / 2;
    turnSpeed = originalTurnSpeed / 2;
  } 
  else {
    // الأرض المستوية: سرعة طبيعية بدون أي تعديل
    maximumSpeed = originalMaximumSpeed;
    baseSpeed = originalBaseSpeed;
    turnSpeed = originalTurnSpeed;
  }
}

void resetPitchOffset() {
  float sumPitch = 0;
  // نأخذ متوسط 10 قراءات سريعة جداً لضمان دقة نقطة الصفر (المدة الكلية 20 ملي ثانية)
  for (int i = 0; i < 10; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B); 
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6, true);
    
    int16_t ax = Wire.read() << 8 | Wire.read();
    int16_t ay = Wire.read() << 8 | Wire.read();
    int16_t az = Wire.read() << 8 | Wire.read();

    // حساب الزاوية الخام اللحظية بنفس الطريقة (استخدمنا الطريقة الأولى التي اتفقنا عليها)
    float rawPitch = atan2((float)-ax, sqrt((float)ay * ay + (float)az * az)) * 180.0 / PI;
    sumPitch += rawPitch;
    delay(2);
  }
  
  // حفظ المتوسط كنقطة مرجعية
  pitchOffset = sumPitch / 10.0; 
}




