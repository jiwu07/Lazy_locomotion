
#include <Arduino.h>
#include <Audio.h>
#include "MultiMap.h"
//vibration part
/************* AUGMENTATION PARAMETERS - START ********************/
namespace augmentation {
// Set the number of pulses across the sensor range.
// The sensor's range (analog reading) is split into equidistant bins.
static const uint8_t kBins = 30;

// Set the amplitude of the output.
// Ideally, keep the amplitude at the maximum (1.0) and
// tune it using the trim pot on the Haptic Servo shield.
static const float kAmplitude = 1.0;
// Should pulses be rendered differently between press and release?
static const bool kAsymAmp = true;
// Should pulses be rendered with in-/decreasing amplitude?
static const bool kFadeAmp = true;
// The following parameters are only used, if kAsymAmp or kFadeAmp is true.
static const float kAmpMin = 0.3;
static const float kAmpMax = 1.0;

// Set the vibration frequency.
// LRAs have a limited frequency spectrum. Hence, check its datasheet
// to get the resonance frequency for maximum output.
static const uint16_t kFrequency = 80;

// Set the duration of a individual pulses.
// Depending on the mechanical properties of the attached actuator,
// very short pulses might not be possible.
static const uint8_t kPulseDuration = 3;
// Should pulses played with random durations?
static const bool kRandDuration = false;
// The following parameters are only used, if kRandDuration is true.
static const uint8_t kPulseDurationMin = 3;
static const uint8_t kPulseDurationMax = 15;
}
/************* AUGMENTATION PARAMETERS - END ********************/


namespace {
static const float kFilterWeight = 0.9;
static const float kSensorRef[15] = {0.00, 3.33, 6.66, 10.00, 13.33, 16.66, 20.00, 23.33, 26.66, 30.00, 33.33, 50.00, 66.66, 83.33, 100.00};
static const float kSensorRes[15] = {0.00, 51.81, 68.42, 77.22, 81.13, 82.11, 84.07, 86.02, 87.98, 88.47, 88.95, 89.93, 90.91, 91.89, 92.38};

static float sensor_val_filtered = 0;
static int last_bin = 0;
static float last_triggered_pos = 0;
static const float kBinDebounceWidth = 100.0 / augmentation::kBins / 3.0;

AudioSynthWaveform    waveformL;
AudioSynthWaveform    waveformR;

AudioOutputI2S        i2s1;
AudioConnection       patchCord1(waveformR, 0, i2s1, 0);
AudioConnection       patchCord2(waveformL, 0, i2s1, 1);
AudioControlSGTL5000  sgtl5000_1;


}






//code part
namespace simulation{

//define the stepping into 4 mode
#define MODE_M1 1
#define MODE_M2 2
#define MODE_M3 3
#define MODE_M4 4

/************* ************* Heigth - ************* ********************/
/************* ************* Height - ************* ********************/
/************* ************* Height - ************* ********************/
float h = 1.6; // subject height



/************* *************  - ************* ********************/
/************* *************  - ************* ********************/
/************* *************  - ************* ********************/
float step_length = h * h / (1.72 * 0.157 * 1.72 * 0.157); // WIP use 1.52 --GUDWIP using 0.157

float Mode_L[2] = {1.0, 1.0}; // time predict each mode left
float Mode_R[2] = {1.0, 1.0}; // time predict each mode right

bool L_up = false;
bool R_up = false;

float delta_tl = 0;
float delta_tr = 0;

int pre_mode_L = MODE_M4;
int pre_mode_R = MODE_M4;

float current_velocity = 0;
float target_velocity = 0;

int pre_height0 = 255;
int pre_height1 = 255;

unsigned long previousMillis = 0;  //last time
const long time_interval = 1;


int isJump = 0;

/************* Delay PARAMETERS -  ********************/
const int bufferSize = 480;       // 1秒内的最大输入数量
int inputsL[bufferSize];           // 存储输入的数组
int inputsR[bufferSize];           // 存储输入的数组
int inputCountL = 0;               // 当前输入的数量
int inputCountR= 0;               // 当前输入的数量
unsigned long previousMillisL =0;          // 记录开始时间
unsigned long previousMillisR =0;          // 记录开始时间
const unsigned long interval = 20; //0.2秒的时间间隔（单位：毫秒）
int outputCountL =0;
int outputCountR = 0;

bool isVibrateL = 0;
bool isVibrateR = 0;


}
int analogValueA0 =0;
int a1 =0;
int analogValueA1 =0;
int a0 =0;
float temp  =0.001;

//checking vibration based on the string length/sensor data(0-255)
//vibrate every constat distance
void update_vibrate(int& vibrate_counter, int sensordata, AudioSynthWaveform& signal ){
  if(vibrate_counter > 15){
   // vibrate(sensordata,signal);
    //Serial.println("count");
    vibrate_counter = 0;
  }
}


//check the string situation and update the current mode
void update_mode(int height, int pre_height, bool& up, int& pre_mode, float& t, float mode_list[],bool& isVibrate) { 
    if (pre_height - height >3) { // going up
        isVibrate = false;
        if (pre_mode == MODE_M3) { // if was M3, insert a M4 before goto M1
            up = false;
            pre_mode = MODE_M4;
            t += temp;
            return;
        }
        if (pre_mode == MODE_M4) { // switch mode from 4-1
            if (mode_list[0] == 1) { // if feet was stopped 
                up = true; //go up
                pre_mode = MODE_M1;
                t = temp;
                return; // means won't have time when mode put down
            }else{
              mode_list[1] = t; // update time
              up = true;
              pre_mode = MODE_M1;
              t = temp;
              return;
            }
            
        }
        up = 1;
        pre_mode = MODE_M1;
        t += temp; // keep mode1 or 2
        return;
    } else if (height - pre_height >2) { // going down
        //update vibrate 
        isVibrate  = true;
        if (pre_mode == MODE_M1) {
            up = 1;
            pre_mode = MODE_M2;
            t += temp;
            return;
        }
        if (pre_mode == MODE_M2) { // switch mode  from 2-3
            mode_list[0] = t; // update time
            up = false;
            pre_mode = MODE_M3;
            t = temp;
            return;
        }
        up = false;
        pre_mode = MODE_M3;
        t += temp; // keep mode m3 or m4
        return;
    } else {
        t += temp;
        if (up) {   
            pre_mode = MODE_M2;
            return;
        }  
        pre_mode = MODE_M4;
        return;
    }
}

//if the feet position is not changing until 0.8s then consider it stop moving
bool if_stop(float t, int pre_mode, bool& isVibrate ) {
    if (t > 0.8) { // too long time not move feet then stop
      isVibrate = false;
        return true;
    }
    pre_mode = MODE_M4;
    return false;
}
/************* ************* Setup - ************* ********************/
/************* ************* Setup - ************* ********************/
/************* ************* Setup - ************* ********************/
void setup() {
  Serial.begin(9600);
  using namespace augmentation;

  AudioMemory(20);

  sgtl5000_1.enable();
  sgtl5000_1.volume(1);
  waveformL.begin(WAVEFORM_SINE);
  waveformL.frequency(80);

  waveformR.begin(WAVEFORM_SINE);
  waveformR.frequency(80);
   // waveformL.amplitude(1);



}

int count = 0;

void loop() {
  using namespace simulation;
  // Read analog input from A0
  analogValueA0 = analogRead(A0);
  a0 = map(analogValueA0, 0, 1023, 0, 255);

  // Read analog input from A1
  analogValueA1 = analogRead(A1);
  a1 = map(analogValueA1, 0, 1023, 0, 255);
  
unsigned long currentMillis = millis();  // current time

if (currentMillis - previousMillis >= time_interval) {
    previousMillis = currentMillis;
  update_mode(a1, pre_height1, L_up, pre_mode_L, delta_tl, Mode_L,isVibrateL);
  update_mode(a0, pre_height0, R_up, pre_mode_R, delta_tr, Mode_R,isVibrateR);

  previousMillis = millis();
//Serial.print(millis());
//////// if stopping for too long time then consider it as stop walking
  if (if_stop(delta_tl, pre_mode_L,isVibrateL)) {
      Mode_L[0] = 1.0;
      Mode_L[1] = 1.0;
  }
  if (if_stop(delta_tr, pre_mode_R,isVibrateR)) {
      Mode_R[0] = 1.0;
      Mode_R[1] = 1.0;
  }


  
  float target_frequency;
  if (Mode_R[0] + Mode_R[1] == 2.0 && Mode_L[0] + Mode_L[1] == 2.0) {
      target_frequency = 0;
  } else {
      target_frequency = (1.0 / (Mode_R[0] + Mode_R[1]) + 1.0 / (Mode_L[0] + Mode_L[1])) / 2.0;
  }

  target_velocity = target_frequency * target_frequency * step_length;
   
  if (current_velocity != target_velocity) {
        current_velocity = 0.85 * target_velocity + 0.15 * current_velocity;
  }

  int velocity = int(current_velocity);

  //if(L_up && R_up){
  //  isJump = 1;
  //}elae{ isjump = 0;}
 
  // Send data to Unity todo here
  Serial.print(a0);
  Serial.print(',');
  Serial.print(a1);
  Serial.print(',');
  Serial.println(velocity);

  pre_height1 = a1;
  pre_height0 = a0;
}

/************* VIBRATION  - Start ********************/
//vibrate when foot putting down with delay
if(isVibrateL){
  if(previousMillisL == 0){
    previousMillisL = millis();
  }
  ///delay
  if (inputCountL < bufferSize) {
    inputsL[inputCountL] = a1;   // storage input
    inputCountL++;                    // update amount
  }
unsigned long currentMillisL = millis();
   if (currentMillisL - previousMillisL >= interval ) {
    if(outputCountL < inputCountL && pre_mode_L != MODE_M1 && pre_mode_L != MODE_M2){
      vibrate(inputsL[outputCountL],waveformL);
      outputCountL++;
    }else{
     // 清空输入数组
    memset(inputsL, 0, sizeof(inputsL));
    
    // 重置计数器和开始时间
    outputCountL = 0;
    inputCountL =0;
    }

   }
}else{
  previousMillisL = 0;
}




//vibrate when foot putting down with delay
if(isVibrateR){
  if(previousMillisR == 0){
    previousMillisR = millis();
  }
  ///delay
  if (inputCountR < bufferSize) {
    inputsR[inputCountR] = a0;   // 将输入存储在数组中
    inputCountR++;                    // 更新输入数量
  }

  
unsigned long currentMillisR = millis();

  // 检查是否经过了delay
  if (currentMillisR - previousMillisR >= interval&& pre_mode_R != MODE_M1 && pre_mode_R != MODE_M2 ) {
    if(outputCountR < inputCountR){
      vibrate(inputsR[outputCountR],waveformR);

      outputCountR++;
    }else{
     // 清空输入数组
    memset(inputsR, 0, sizeof(inputsR));
    
    // 重置计数器和开始时间
    outputCountR = 0;
    inputCountR =0;

    }

  }
  
}else{
  previousMillisR = 0;
}
 
/************* VIBRATION  - END ********************/



}

void vibrate(int input, AudioSynthWaveform& signal){
  using namespace augmentation;

  //signal.amplitude(1);
  sensor_val_filtered = ((1.0 - kFilterWeight) * sensor_val_filtered)
                          + (kFilterWeight *  input) ;

  float sensor_val_percent = multiMap<const float>(
                              sensor_val_filtered/10.23,
                              kSensorRes,
                              kSensorRef,
                              15);
  sensor_val_percent =  sensor_val_filtered/9;                         
  //Serial.print(sensor_val_percent);
  //Serial.print(",");


//map the precentage to 0-30
  int bin = map(sensor_val_percent, 0.f, 100.f, 0, kBins);
    //Serial.println(bin);

 
  if (bin == last_bin || abs(last_triggered_pos - sensor_val_percent) < kBinDebounceWidth) {
    return;
  }

 
  float amp = kAmplitude;

  if (!kAsymAmp) {
    if (kFadeAmp) {
      if (sensor_val_percent < last_triggered_pos) {
        amp = map(sensor_val_percent, 0.f, 100.f, 0.f, kAmpMin);
      } else {
        amp = map(sensor_val_percent, 0.f, 100.f, kAmpMin, kAmpMax);
      }
    } else {
      amp = (sensor_val_percent < last_triggered_pos) ? kAmpMin : kAmpMax;
    }
  } else {
    amp = (kFadeAmp) ? map(sensor_val_percent, 0.f, 100.f, kAmpMin, kAmpMax) : kAmplitude;
  }
  signal.amplitude(amp);
  //waveformL.amplitude(amp);

  if (kRandDuration) {
    delay(random(kPulseDurationMin, kPulseDurationMax));
  } else {
    delay(kPulseDuration);
  }

  signal.amplitude(0.f);
  //waveformL.amplitude(0.f);
  //Serial.print(amp);

  last_bin = bin;
  last_triggered_pos = sensor_val_percent;

//signal.amplitude(1);
}
