// ====================================================================
// ملف الاستراتيجية 0: المعايرة الذكية للحساسات (Calibration Mode)
// ====================================================================

// --- المتغيرات المحلية (Static) ---
// استخدام static يحفظ قيم المتغيرات في الذاكرة حتى بعد الخروج من الدالة
static int calibState = 0;              // آلة الحالة (0 = انتظار الأبيض، 1 = انتظار الأسود)
static bool lastIsRunning = false;      // ذاكرة لحالة زر التشغيل السابقة لاكتشاف لحظة الضغط (Edge Detection)
static byte lastStrategyForCalib = 255; // ذاكرة لرقم الاستراتيجية السابقة لاكتشاف لحظة الدخول لوضع المعايرة

// مصفوفات ومتغيرات لتخزين قيم المعايرة مؤقتاً قبل اعتمادها
static float whiteAvg[12];              // متوسط القراءات الـ 100 للون الأبيض لكل حساس
static float blackAvg[12];              // متوسط القراءات الـ 100 للون الأسود لكل حساس
static float overallWhiteAvg = 0;       // المتوسط العام الموحد للون الأبيض (لجميع الحساسات الوسطى)
static float overallBlackAvg = 0;       // المتوسط العام الموحد للون الأسود (لجميع الحساسات الوسطى)

// ====================================================================
// الدالة الرئيسية للاستراتيجية 0 (تُستدعى باستمرار داخل الـ Loop)
// ====================================================================
void loopStrategy0() {
    
    // 1. الأمان أولاً: إجبار المحركات على التوقف التام دائماً أثناء وضع المعايرة
    stopMotor();

    // 2. تهيئة النظام عند الدخول لأول مرة في وضع المعايرة
    // (يحمي النظام من التداخل إذا قمت بتغيير الاستراتيجيات والعودة للصفر)
    if (strategy != lastStrategyForCalib) {
        calibState = 0;            // تصفير آلة الحالة لتبدأ من معايرة الأبيض
        lastIsRunning = isRunning; // مزامنة حالة الزر الحالية لمنع بدء المعايرة فوراً بشكل خاطئ
        
        // طباعة تعليمات الاستخدام على شاشة التلنت
        TelnetStream.println("\n=========================================");
        TelnetStream.println("⚙️ تم تفعيل وضع المعايرة (الاستراتيجية 0)");
        TelnetStream.println("1. ضع الروبوت على [الخط الأبيض] تماماً.");
        TelnetStream.println("2. اضغط زر التشغيل (سيصبح isRunning = True).");
        TelnetStream.println("=========================================\n");
        
        lastStrategyForCalib = strategy; // حفظ الحالة لمنع تكرار الطباعة
    }

    // 3. قراءة الحالة الحية لزر التشغيل (التي يتم التحكم بها بشكل آمن من Switch.ino)
    bool currentIsRunning = isRunning;

    // ==========================================
    // 4. آلة الحالة (State Machine) لإدارة المعايرة
    // ==========================================
    
    // --- الحالة 0: في انتظار معايرة الخط الأبيض ---
    if (calibState == 0) {
        // نكتشف لحظة "تشغيل" الزر (من False إلى True)
        if (currentIsRunning == true && lastIsRunning == false) {
            TelnetStream.println("\n=== بدء معايرة الخط الأبيض ===");
            
            runCalibrationPhase(false); // استدعاء دالة أخذ القراءات وتمرير (false) لتعني "هذا ليس أسود"
            playToneWhiteDone();        // إطلاق 3 نغمات سريعة للتأكيد
            
            calibState = 1;             // الانتقال للحالة 1 (انتظار الأسود)
            
            TelnetStream.println("\n✅ تمت معايرة الأبيض بنجاح.");
            TelnetStream.println("--> الروبوت الآن في وضع التشغيل (الآن انقله وضعه فوق الخط الأسود).");
            TelnetStream.println("--> اضغط الزر مرة أخرى للإيقاف (سيصبح isRunning = False) لتبدأ معايرة الأسود...");
        }
    }
    // --- الحالة 1: في انتظار معايرة الخط الأسود ---
    else if (calibState == 1) {
        // نكتشف لحظة "إيقاف" الزر (من True إلى False)
        if (currentIsRunning == false && lastIsRunning == true) {
            TelnetStream.println("\n=== بدء معايرة الخط الأسود ===");
            
            runCalibrationPhase(true);     // استدعاء دالة أخذ القراءات وتمرير (true) لتعني "هذا أسود"
            finalizeCalibration();         // استدعاء دالة الحسابات النهائية وطباعة الأكواد
            playToneCalibrationComplete(); // إطلاق نغمة المعايرة الطويلة
            
            calibState = 0;                // تصفير الحالة للتمكن من إعادة المعايرة فوراً دون إطفاء الروبوت
            
            TelnetStream.println("\n🎉 تمت المعايرة بالكامل! النظام جاهز والمصفوفات الحية تم تحديثها.");
            TelnetStream.println("يمكنك تغيير مفاتيح الاستراتيجية للانطلاق، أو تكرار العملية بوضع الروبوت على الأبيض والضغط مجدداً.");
        }
    }

    // 5. تحديث حالة الزر السابقة للمقارنة في الدورة الزمنية القادمة
    lastIsRunning = currentIsRunning;

    // 6. تفريغ الذاكرة المؤقتة (Buffer) للتلنت لمنع تكدس البيانات
    if (TelnetStream.available()) { 
        TelnetStream.read(); 
    }
}

// ====================================================================
// دالة فرعية: أخذ قراءات الحساسات وحساب المتوسطات (Sampling Phase)
// المتغير (isBlack) يحدد ما إذا كنا نحفظ البيانات في مصفوفة الأسود أم الأبيض
// ====================================================================
void runCalibrationPhase(bool isBlack) {
    long sums[12] = {0}; // مصفوفة لـ 12 حساس
    TelnetStream.println("جاري أخذ 100 قراءة متتالية لضمان الدقة...");
    
    for (int i = 0; i < 100; i++) {
        for (int s = 0; s < 12; s++) {
            int val = analogRead(sensorPins[s]);
            sums[s] += val; 
        }
        delay(10);
    }

    TelnetStream.println("--- متوسط الحساسات الـ 12 ---");
    float overallSum = 0; 

    for (int s = 0; s < 12; s++) {
        float avg = (float)sums[s] / 100.0;
        
        if (isBlack) blackAvg[s] = avg;
        else whiteAvg[s] = avg;

        TelnetStream.print(avg, 0);
        TelnetStream.print("\t");

        // استثناء الحساسات (S1, S2, S11, S12) من المتوسط العام بناءً على طلبك ونصحيتي!
        // أرقامهم في المصفوفة هي 0 و 1 و 10 و 11
        if (s != 0 && s != 1 && s != 10 && s != 11) {
            overallSum += avg;
        }
    }
    TelnetStream.println(); 

    // حساب المتوسط للـ 8 حساسات المتبقية (قسمة على 8)
    float overall = overallSum / 8.0;
    if (isBlack) overallBlackAvg = overall;
    else overallWhiteAvg = overall;
}

// ====================================================================
// دالة فرعية: الحسابات النهائية وتحديث المتغيرات الحية وتوليد الكود
// ====================================================================
void finalizeCalibration() {
    int calculatedThreshold = round((overallWhiteAvg + overallBlackAvg) / 2.0);
    target_White = round(overallWhiteAvg);
    target_Black = round(overallBlackAvg);
    lineThreshold = calculatedThreshold;

    for (int i = 0; i < 12; i++) {
        S_White[i] = round(whiteAvg[i]);
        S_Black[i] = round(blackAvg[i]);
    }

    TelnetStream.println("\n=======================================================");
    TelnetStream.println("🎉 اكتملت المعايرة بنجاح! تم تحديث الذاكرة الحية.");
    TelnetStream.println("يمكنك نسخ الكود التالي ولصقه في ملف LineFollower.ino لحفظه دائماً:");
    
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


