// تعريف المتغيرات محلياً (static) لتحتفظ بقيمتها
static int calibState = 0;
static bool lastIsRunning = false;
static byte lastStrategyForCalib = 255;

// متغيرات التخزين المؤقت للمعايرة
static float whiteAvg[12];
static float blackAvg[12];
static float overallWhiteAvg = 0;
static float overallBlackAvg = 0;

void loopStrategy7() {
    // 1. إجبار المحركات على التوقف التام دائماً أثناء وضع المعايرة للأمان
    stopMotor();

    // 2. اكتشاف الدخول الجديد للاستراتيجية 7 لتصفير المتغيرات (يحميك عند تغيير الاستراتيجيات والعودة)
    if (strategy != lastStrategyForCalib) {
        calibState = 0;
        lastIsRunning = isRunning; // مزامنة الحالة الحالية لتجاهل الـ false عند التشغيل
        TelnetStream.println("\n=========================================");
        TelnetStream.println("⚙️ تم تفعيل وضع المعايرة (الاستراتيجية 7)");
        TelnetStream.println("1. ضع الروبوت على [الخط الأبيض].");
        TelnetStream.println("2. اضغط زر التشغيل (سيصبح isRunning = True).");
        TelnetStream.println("=========================================\n");
        lastStrategyForCalib = strategy;
    }

    // 3. قراءة الحالة الحالية للتشغيل (التي يتم التحكم بها بشكل آمن من Switch.ino)
    bool currentIsRunning = isRunning;

    // 4. آلة الحالة (State Machine) للمعايرة حسب طلبك المخصص
    if (calibState == 0) {
        // انتظار التحول من إيقاف (false) إلى تشغيل (true) -> معايرة الأبيض
        if (currentIsRunning == true && lastIsRunning == false) {
            TelnetStream.println("\n=== بدء معايرة الخط الأبيض ===");
            runCalibrationPhase(false);
            playToneWhiteDone();
            calibState = 1; // الانتقال لانتظار الأسود
            TelnetStream.println("\n✅ تمت معايرة الأبيض.");
            TelnetStream.println("--> الروبوت الآن في وضع التشغيل (الآن انقله للون الأسود).");
            TelnetStream.println("--> اضغط الزر مرة أخرى للإيقاف (سيصبح isRunning = False) لتبدأ معايرة الأسود...");
        }
    }
    else if (calibState == 1) {
        // انتظار التحول من تشغيل (true) إلى إيقاف (false) -> معايرة الأسود
        if (currentIsRunning == false && lastIsRunning == true) {
            TelnetStream.println("\n=== بدء معايرة الخط الأسود ===");
            runCalibrationPhase(true);
            finalizeCalibration();
            playToneCalibrationComplete();
            calibState = 0; // تصفير الحالة للتمكن من الإعادة فوراً دون الحاجة لترسيت
            TelnetStream.println("\n🎉 تمت المعايرة بالكامل! النظام جاهز.");
            TelnetStream.println("يمكنك تغيير الاستراتيجية للانطلاق، أو تكرار العملية بوضع الروبوت على الأبيض والضغط مجدداً.");
        }
    }

    // تحديث الحالة السابقة للمقارنة في الدورة القادمة
    lastIsRunning = currentIsRunning;

    // إبقاء الاتصال نشطاً وتفريغ البفر
    if (TelnetStream.available()) { TelnetStream.read(); }
}

void runCalibrationPhase(bool isBlack) {
    long sums[12] = {0};
    TelnetStream.println("جاري أخذ 100 قراءة لضمان الدقة...");

    for (int i = 0; i < 100; i++) {
        for (int s = 0; s < 12; s++) {
            int val = analogRead(sensorPins[s]);
            sums[s] += val;
        }
        delay(10); // تأخير زمني بسيط لضمان استقرار القراءات التناظرية
    }

    TelnetStream.println("--- متوسط الحساسات الـ 12 ---");
    float overallSum = 0;

    for (int s = 0; s < 12; s++) {
        float avg = (float)sums[s] / 100.0;
        if (isBlack) {
            blackAvg[s] = avg;
        } else {
            whiteAvg[s] = avg;
        }

        TelnetStream.print(avg, 0);
        TelnetStream.print("\t");

        // استثناء S2 و S11 من المتوسط العام (كما في الكود الأصلي)
        if (s != 1 && s != 10) {
            overallSum += avg;
        }
    }
    TelnetStream.println();

    // قسمة على 10 لأننا استثنينا حساسين
    float overall = overallSum / 10.0; 
    
    if (isBlack) {
        overallBlackAvg = overall;
    } else {
        overallWhiteAvg = overall;
    }
}

void finalizeCalibration() {
    int calculatedThreshold = round((overallWhiteAvg + overallBlackAvg) / 2.0);

    // تحديث المتغيرات العامة الحية
    target_White = round(overallWhiteAvg);
    target_Black = round(overallBlackAvg);
    lineThreshold = calculatedThreshold;

    for (int i = 0; i < 12; i++) {
        S_White[i] = round(whiteAvg[i]);
        S_Black[i] = round(blackAvg[i]);
    }

    TelnetStream.println("\n=======================================================");
    TelnetStream.println("🎉 اكتملت المعايرة بنجاح! تم تحديث الذاكرة الحية.");
    TelnetStream.println("يمكنك نسخ الكود التالي ولصقه في ملف LineFollower.ino:");
    TelnetStream.println("=======================================================\n");

    TelnetStream.print("int S_White[12] = {");
    for (int i = 0; i < 12; i++) {
        TelnetStream.print(S_White[i]);
        if (i < 11) TelnetStream.print(", ");
    }
    TelnetStream.println("};");

    TelnetStream.print("int S_Black[12] = {");
    for (int i = 0; i < 12; i++) {
        TelnetStream.print(S_Black[i]);
        if (i < 11) TelnetStream.print(", ");
    }
    TelnetStream.println("};");

    TelnetStream.print("int target_White = "); TelnetStream.print(target_White); TelnetStream.println(";");
    TelnetStream.print("int target_Black = "); TelnetStream.print(target_Black); TelnetStream.println(";");
    TelnetStream.print("int lineThreshold = "); TelnetStream.print(lineThreshold); TelnetStream.println(";");
    TelnetStream.println("\n=======================================================");
}