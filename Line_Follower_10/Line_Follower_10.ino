#include <QTRSensors.h>  // تضمين مكتبة QTRSensors للتحكم بحساسات الخط
  QTRSensors qtr;  // إنشاء كائن من المكتبة للتحكم بالحساسات

  #define IR_L_5 22
  #define IR_L_4 32
  #define IR_L_3 33
  #define IR_L_2 25
  #define IR_L_1 26
  #define IR_R_1 27
  #define IR_R_2 14
  #define IR_R_3 12
  #define IR_R_4 13
  #define IR_R_5 23

  const uint8_t sensorCount = 8;  // تعريف عدد الحساسات (8 حساسات هنا)
  const uint8_t sensorPins[] = {IR_L_4 , IR_L_3 , IR_L_2 , IR_L_1 , IR_R_1 , IR_R_2 , IR_R_3 , IR_R_4 };  // المتصلة بالحساسات (ESP32)تعريف دبابيس الـ
  uint16_t sensorValues[sensorCount];  // مصفوفة لتخزين القيم المقروءة من الحساسات (الوقت بالمايكروثانية)

  #define Push_Button 21

  /*************** تعريف دبابيس التحكم بالمحركات ***************/
  // ------ محرك الجانب الأيمن ------
  #define EN_B 2   // للمحرك الأيمن (التحكم بالسرعة) PWM دبوس تمكين
  #define IN_3 4   // دبوس التحكم بالاتجاه 3 للمحرك الأيمن
  #define IN_4 5   // دبوس التحكم بالاتجاه 4 للمحرك الأيمن

  // ------ محرك الجانب الأيسر ------
  #define EN_A 15   // للمحرك الأيسر (التحكم بالسرعة) PWM دبوس تمكين
  #define IN_1 19   // دبوس التحكم بالاتجاه 1 للمحرك الأيسر
  #define IN_2 18   // دبوس التحكم بالاتجاه 2 للمحرك الأيسر

  /*************** ESP32 المتقدمة لـ PWM إعدادات ***************/
  #define PWM_Channel_R 0  // رقم 0 للمحرك الأيمن PWM قناة
  #define PWM_Channel_L 1   // رقم 1 للمحرك الأيسر PWM قناة
  #define Frequency 7500         // (5 كيلوهرتز) PWM تردد إشارة
  #define Resolution 8           // (تساوي 8 بت = قيم من 0 إلى 255) PWM دقة

  double error = 0;
  double lastError = 0;
  double totalError = 0;
  double integral = 0;
  double output = 0;

  double weightedSum = 0;
  int sum = 0;

  int R_Speed = 0;
  int L_Speed = 0;
  
  int total_Sensor = 10;
  uint8_t i = 0; // يمكن أن يأخذ قيمًا موجبة من 0 إلى 255 فقط (uint8_t) تـعـريـف مُـتـغـيـر من نـوع
  bool Run = 0;
  unsigned long Print_1 = 0;
  int Values[8];

  int sensorValues_total[10];

  int skip = 1;

  // ━━━━━━━ إعدادات PID ━━━━━━━
  double Kp = 45;  // معامل التناسب
  double Ki = 0.0001; // معامل التكامل
  double Kd = 60;  // معامل التفاضل

  int baseSpeed = 170; // السرعة الأساسية (0-255)
  int maxSpeed = 195;   // أقصى سرعة

  // أوزان الحساسات
  int sensorWeights[10] = {-18, -12, -8, -4,-1 , 1, 4, 8, 12, 18};

    // Forward Declarations
  void Stop_motors();
  void calculateError();
  void PID();
  void Move_motors();
  void Print();
  void readSensors();

void setup() {
  Serial.begin(115200);  // تهيئة الاتصال التسلسلي لعرض النتائج (سرعة 115200 باود)

  pinMode(IR_L_5, INPUT);
  pinMode(IR_R_5, INPUT);
  
  // ━━━━━━━ تهيئة الحساسات ━━━━━━━
  qtr.setTypeRC();  // (يعتمد على تفريغ المكثف) (RC)تحديد نوع الحساسات كـ
  qtr.setSensorPins(sensorPins, sensorCount);    // تحديد الدبابيس المستخدمة للحساسات وعددها

  pinMode(Push_Button,INPUT_PULLUP);

  // ------ تهيئة دبابيس التحكم بالاتجاه كمخرجات ------
  pinMode(IN_3, OUTPUT);  // تهيئة IN3 كمخرج
  pinMode(IN_4, OUTPUT);  // تهيئة IN4 كمخرج
  pinMode(IN_1, OUTPUT);  // تهيئة IN1 كمخرج
  pinMode(IN_2, OUTPUT);   // تهيئة IN2 كمخرج

  // ------ المتقدمة PWM تهيئة قنوات ------
  ledcSetup(PWM_Channel_R , Frequency , Resolution);  // للمحرك الأيمن مع التردد والدقة المحددة PWM إعداد قناة
  ledcSetup(PWM_Channel_L , Frequency , Resolution);  // للمحرك الأيسر مع التردد والدقة المحددة PWM إعداد قناة
  // ------ PWM ربط دبابيس التمكين مع قنوات ------
  ledcAttachPin(EN_B , PWM_Channel_R); // بالقناة 0 ENB ربط
  ledcAttachPin(EN_A , PWM_Channel_L);  // بالقناة 1 ENA ربط

  Stop_motors();  // إيقاف كلا المحركين عند بداية التشغيل

}

void loop() {

  if(digitalRead(Push_Button) == LOW){
  Run = 1;
  Serial.println("Run");
  //delay(1000);
  }

  if(Run == 1){
  readSensors();
  calculateError();
  PID();
  Move_motors();
  Print();
  //delay(1000);
  }

}

void readSensors() {
  qtr.read(sensorValues);

  for (int i = 0; i < 8; i++) {
    if(sensorValues[i] >= 1500){
      sensorValues[i]=1;
    }else{
      sensorValues[i]=0;
    }
  }
  sensorValues_total[0] = (digitalRead(IR_L_5));
  sensorValues_total[1] = sensorValues[0];
  sensorValues_total[2] = sensorValues[1];
  sensorValues_total[3] = sensorValues[2];
  sensorValues_total[4] = sensorValues[3];
  sensorValues_total[5] = sensorValues[4];
  sensorValues_total[6] = sensorValues[5];
  sensorValues_total[7] = sensorValues[6];
  sensorValues_total[8] = sensorValues[7];
  sensorValues_total[9] = (digitalRead(IR_R_5));
}

void calculateError(){
  weightedSum = 0;
  sum = 0;

  for (int i = 0; i < 10; i++) {
    weightedSum += sensorValues_total[i] * sensorWeights[i];
    sum += sensorValues_total[i];
  }

  if (sensorValues_total[0] == 0 &&
      sensorValues_total[1] == 0 &&
      sensorValues_total[2] == 0 &&
      sensorValues_total[3] == 0 &&
      sensorValues_total[4] == 0 &&
      sensorValues_total[5] == 0 &&
      sensorValues_total[6] == 0 &&
      sensorValues_total[7] == 0 &&
      sensorValues_total[8] == 0 &&
      sensorValues_total[9] == 0 ){
        skip = 1;
        //Serial.print("wwwwwwww");

      if (R_Speed < L_Speed){
      // يمين	
      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      ledcWrite(PWM_Channel_R , 180);
      ledcWrite(PWM_Channel_L , 180);
      //Serial.print("rrrrrrrrrrrrrrr");

      }
      if (R_Speed > L_Speed){
       // يسار
      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      ledcWrite(PWM_Channel_R , 180);
      ledcWrite(PWM_Channel_L , 180);
      //Serial.print("lllllllllllll");

      }
        return; 
  } else  if (sum == 0){ // إذا فقد الخط، استخدم آخر قيمة خطأ
    error = lastError;
    skip = 0;
  }  else{
    error = weightedSum / sum;
    skip = 0;
  }

}

void PID(){
  if (skip == 1){
   return;
  }
  
  int derivative = error - lastError;
  totalError += error;
  output = (Kp * error) + (Ki * totalError) + (Kd * derivative);
  lastError = error; // حفظ الخطأ السابق


  // تحديث السرعات
  L_Speed = baseSpeed + output;
  R_Speed = baseSpeed - output;

  L_Speed = constrain(L_Speed, 0, maxSpeed);
  R_Speed = constrain(R_Speed, 0, maxSpeed);
}

//  تحريك المواتير
void Move_motors() {
  if (skip == 1){
   return;
  }

  // التحكم بالاتجاه للأمام
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  // تحديد السرعات
  ledcWrite(PWM_Channel_R , R_Speed);
  ledcWrite(PWM_Channel_L , L_Speed);
}

// يقاف المواتير
void Stop_motors() {
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  ledcWrite(PWM_Channel_R , 0);
  ledcWrite(PWM_Channel_L , 0);
}

// طباعة
void Print() {
  if (millis() - Print_1 > 1000) {
    Serial.print("\t");
    for (i = 0 ; i < 10 ; i++) {
      Serial.print(sensorValues_total[i]);  // عرض قيمة الحساس الحالي
      Serial.print("\t");  // (Tab) إضافة تباعد بين القيم
    }
    Serial.println();  // إنهاء السطر بعد عرض جميع القيم
    Serial.println(); 

    Serial.print("   Error= ");
    Serial.print(error);
    Serial.print("   Output= ");
    Serial.print(output);
    Serial.print("   Left= ");
    Serial.print(L_Speed);
    Serial.print("   Right= ");
    Serial.println(R_Speed);

    Serial.println();
    //delay(1000);  // تأخير 100 مللي ثانية قبل القراءة التالية
    Print_1 = millis();
  }
}






