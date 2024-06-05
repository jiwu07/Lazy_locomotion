#include <Arduino.h>
#include <Audio.h>

#include "MultiMap.h"

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

/************* ************* Heigth - ************* ********************/
/************* ************* Height - ************* ********************/
/************* ************* Height - ************* ********************/
float h = 1.6; // subject height

/************* *************  - ************* ********************/
/************* *************  - ************* ********************/
/************* *************  - ************* ********************/
float step_length = h * h / (1.72 * 0.157 * 1.72 * 0.157); // WIP use 1.52 --GUDWIP using 0.157

const long time_interval = 5; //,s
float previousMillis =0;
/**************************/
const float dt = ((float)time_interval)/1000; // 采样时间间隔

float PosTreshold = 0.1;
float VelTreshold = 0.5;
int PrevModeR = 1;
int PrevModeL = 1;
float w1 = 0.8;
float PhaseWidths[4] = {0.55, 0.2, 0.05, 0.2};
float w = 0.1;
float AbsStepPaceR = 0;
float AbsStepPaceL = 0;

float ExpAbsPhaseR = 0;
float ExpAbsPhaseL = 0;

float NextAbsPhaseR = PhaseWidths[0];
float NextAbsPhaseL = PhaseWidths[0];

// Kalman 
float xhat[2] = {0, 0};
float A[2][2] = {{1, dt}, {0, 1}};
float C[1][2] = {1, 0};
float G[2] = {dt * dt / 2, dt};
float Q[2][2] = {{dt * dt * dt * dt / 4* 0.01 * 0.01, 0}, {0, dt * dt* 0.01 * 0.01}} ;
float R = 0.0034 * 0.0034;
float P_apost[2][2] = {0};
float TimeAfterLastR = 0;
float TimeAfterLastL = 0;


// mode init
float ModeInitR[4] = {-1, -1, -1, -1};
float ModeInitL[4] = {-1, -1, -1, -1};
}
int a1 =0;
int a0 =0;
int pre_height0;
int pre_height1;


/************* ************* Setup - ************* ********************/
/************* ************* Setup - ************* ********************/
/************* ************* Setup - ************* ********************/
void setup() {
  Serial.begin(9600);
  using namespace augmentation;

  AudioMemory(20);

  sgtl5000_1.enable();
  sgtl5000_1.volume(1);

  //left audio
  waveformL.begin(WAVEFORM_SINE);
  waveformL.frequency(80);

//right audio
  waveformR.begin(WAVEFORM_SINE);
  waveformR.frequency(80);
   // waveformL.amplitude(1);


}

int count = 0;

void loop() {
  using namespace simulation;
  // Read analog input from A0
  a0 = analogRead(A0);
  // Read analog input from A1
  a1 = analogRead(A1);

  unsigned long currentMillis = millis();  // current time
  
  //if the time interval is reached, then do the calculation
  if (currentMillis - previousMillis >= time_interval) {
    previousMillis = currentMillis;  //update the previous time
    float Vel0 = -(a0 - pre_height0)/ dt;
    float Vel1 = -(a1 - pre_height1)/ dt;

    float frequency_Right = RealTimeDistinguishAndCalculate(1023-a0,Vel0, PrevModeR, ModeInitR, TimeAfterLastR, AbsStepPaceR, ExpAbsPhaseR, waveformR);
    float frequency_Left =  RealTimeDistinguishAndCalculate(1023-a1,Vel1, PrevModeL, ModeInitL, TimeAfterLastL, AbsStepPaceL, ExpAbsPhaseL,  waveformL);

  float frequency = 0.5* frequency_Right + 0.5*frequency_Left;
  //calculate velocity  

  float current_velocity = frequency * frequency * step_length;

  int velocity = int(current_velocity);

  //send to unity //todo
  //if (currentMillis - previousMillisUnity >= time_interval_Unity) {
 //   previousMillisUnity = currentMillis;
  Serial.print(previousMillis/1000.00);
  Serial.print(',');
  Serial.print(a0);
  Serial.print(',');
  Serial.print(a1);
  Serial.print(',');
  Serial.print(PrevModeR);
  Serial.print(',');
  Serial.print(PrevModeL);
  Serial.print(',');
  Serial.println(velocity);
 // }
  }
  pre_height0 = a0;
  pre_height1 = a1;
}



float RealTimeDistinguishAndCalculate(int data,int Vel, int& PrevMode, float ModeInit[], float& TimeAfterLast, float& AbsStepPace, float& ExpAbsPhase,AudioSynthWaveform& signal) {
  using namespace simulation;

  int NewMode = DistinguishRegions(data, Vel, PosTreshold, VelTreshold, PrevMode,signal);

  if (NewMode != PrevMode) {
    TimeAfterLast = 0;
    float CurTime = millis() / 1000.0;
    float TimeInt = CurTime - ModeInit[NewMode];
    float StepPaceUpd = 1 / TimeInt;
    ModeInit[NewMode] = CurTime;
    AbsStepPace = StepPaceUpd * w1 + AbsStepPace * (1 - w1);
  } else {
    TimeAfterLast += dt;
    if (TimeAfterLast > 1) {
      AbsStepPace *= 0.99;
    }
  }


  PrevMode = NewMode;
  return AbsStepPace;

}



// 模式识别函数
int DistinguishRegions(int Data, float dData, float PosTreshold, float VelTreshold, int PrevMode,AudioSynthWaveform& signal) {
  bool MovingUp = dData > 0;
  bool OffGround = Data > PosTreshold;
  bool MovingFast = abs(dData) > VelTreshold * 2;
  int NewMode = PrevMode;

  if (PrevMode == 1) {
    if (OffGround && MovingUp) {
      NewMode = 2;
    }
  } else if (PrevMode == 2) {
    if (!MovingFast || !MovingUp) {
      NewMode = 3;
    }
  } else if (PrevMode == 3) {
    //vibrate here
    vibrate(Data,signal);
    if (!MovingUp && MovingFast) {
      NewMode = 4;
    }
  } else if (PrevMode == 4) {
    if (!OffGround) {
      NewMode = 1;
    }
  }

  return NewMode;
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
 // Serial.print(amp);

  last_bin = bin;
  last_triggered_pos = sensor_val_percent;

//signal.amplitude(1);
}
