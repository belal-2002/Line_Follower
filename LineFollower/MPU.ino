void updateMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 2, true);
  int16_t gyroZ_raw = Wire.read() << 8 | Wire.read();

  unsigned long currentTime = micros();
  float dt = (currentTime - lastMpuTime) / 1000000.0;
  lastMpuTime = currentTime;

  // تحويل القراءة الخام إلى درجات/ثانية
  float gyroZ_rate = (gyroZ_raw - gyroZ_offset) / 131.0;

  // فلتر بسيط لتجاهل التشويش (Deadband)
  if(abs(gyroZ_rate) > 1.0) {
    currentAngleZ += gyroZ_rate * dt;
  }
}