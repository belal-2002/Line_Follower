void loopStrategy() {
  switch (strategy) {
    case 0: // 000
      if (midSensor >= 7) {  // السير في خط مستقيم لتجاوز التقاطع
        caseMotor = 3;
        lineWasFound = false;
        lineAvailable = false;
        loopMotor();
        return;
      }

      // لازم افحص بت معين
      if (leftRadar == 2) {
    
      }
    break;
    case 1: // 001
      // بس يخلص الخط الاسود لف
      if (midSensor >= 7) {  // السير في خط مستقيم لتجاوز التقاطع
        caseMotor = 3;
        lineWasFound = false;
        lineAvailable = false;
        loopMotor();
        return;
      }

      // لازم افحص بت معين
      if (leftRadar == 2) {
    
      }
    break;
    case 2: // 010
      if (allSensor >= 11 || radar >= 3) {  // السير في خط مستقيم لتجاوز التقاطع
        caseMotor = 3;
        lineWasFound = false;
        lineAvailable = false;
        loopMotor();
        return;
      }

      // لازم افحص بت معين
      if (leftRadar == 2) {
    
      }
    break;
    case 3: // 011

    break;
    case 4: // 100

    break;
    case 5: // 101

    break;
    case 6: // 110

    break;
    case 7: // 111

    break;
  }
}