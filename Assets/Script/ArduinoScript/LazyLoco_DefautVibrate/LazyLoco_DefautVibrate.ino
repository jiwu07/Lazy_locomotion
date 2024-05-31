
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

float Mode_L[2] = {0, 0}; // time predict each mode left
float Mode_R[2] = {0, 0}; // time predict each mode right

bool L_up = false;
bool R_up = false;

float delta_tl = 0.000f;
float delta_tr = 0.000f;

int pre_mode_L = MODE_M4;
int pre_mode_R = MODE_M4;

float current_velocity = 0;
float target_velocity = 0;

int pre_height0 = 1023;
int pre_height1 = 1023;



int vibrate_counterL = 0;
int vibrate_counterR = 0;

unsigned long previousMillis = 0;  //last time
unsigned long previousMillisUnity =0;
const long time_interval = 5;
const long time_interval_Unity = 20;
}
int analogValueA0 =0;
int a1 =0;
int analogValueA1 =0;
int a0 =0;


//checking vibration based on the string length/sensor data(0-255)
//vibrate every constat distance
void update_vibrate(int& vibrate_counter, int sensordata, AudioSynthWaveform& signal ){
  if(vibrate_counter > 10){
    vibrate(sensordata,signal);
   // Serial.println("count");
    vibrate_counter = 0;
  }
}


//check the string situation and update the current mode
void update_mode(int height, int pre_height, bool& up, int& pre_mode, float& t, float mode_list[],int& vibrate_counter) { 
    float temp = ((float)simulation::time_interval)/1000.000f;
    if (pre_height - height > 2) { // going up
        if (pre_mode == MODE_M3) { // if was M3, insert a M4 before goto M1
            up = false;
            pre_mode = MODE_M4;
            t += temp;
            return;
        }
        if (pre_mode == MODE_M4) { // switch mode from 4-1
            if (mode_list[0] == 0) { // if feet was stopped 
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
        up = true;
        pre_mode = MODE_M1;
        t += temp; // keep mode1 or 2
        return;
    } else if (height - pre_height >2) { // going down
        //update vibrate counter
        vibrate_counter  = vibrate_counter + height - pre_height;
        if (pre_mode == MODE_M1) {
            up = true;
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
bool if_stop(float t, int pre_mode ) {
    if (t > 2.0) { // too long time not move feet then stop
        pre_mode = MODE_M4;
        return true;
    }
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
  a0 = analogRead(A0);
 // a0 = map(analogValueA0, 0, 1023, 0, 255);
  // Read analog input from A1
  a1 = analogRead(A1);
  //a1 = map(analogValueA1, 0, 1023, 0, 255);

  unsigned long currentMillis = millis();  // current time
  
  //if the time interval is reached
  if (currentMillis - previousMillis >= time_interval) {
    previousMillis = currentMillis;  //update the previous time
   // check the update 
  update_mode(a1, pre_height1, L_up, pre_mode_L, delta_tl, Mode_L,vibrate_counterL);
  update_mode(a0, pre_height0, R_up, pre_mode_R, delta_tr, Mode_R,vibrate_counterR);

  //check stop
  //////// if stopping for too long time then consider it as stop walking
  if (if_stop(delta_tl, pre_mode_L)) {
      Mode_L[0] = Mode_L[0] * 1.5;
      Mode_L[1] = Mode_L[1] * 1.5;
  }
  if (if_stop(delta_tr, pre_mode_R)) {
      Mode_R[0] = Mode_R[0] *1.5;
      Mode_R[1] = Mode_R[1] *1.5;
  }

  //calculate velocity
  float target_frequency;

  if(Mode_R[0] >= 5){
   // Mode_R[0] = 0;
  }
   if(Mode_R[1] >=5){
   // Mode_R[1] = 0;
  }
   if(Mode_L[0] >= 5){
    //Mode_L[0] = 0;
  }
   if(Mode_L[1] >= 5){
   // Mode_L[1] = 0;
  }
// too fast wakling threshold  
//here walking frequenzy threshold reference https://www.researchgate.net/publication/291793625_Frequency_and_velocity_of_people_walking
  if (Mode_R[0] + Mode_R[1] <= 0.15 || Mode_L[0] + Mode_L[1] <= 0.15) {
    if(Mode_R[0] + Mode_R[1] <= 0.15){
      Mode_R[0]=0;
      Mode_R[1] = 0;
      if(Mode_L[0] + Mode_L[1] <= 0.15){
        Mode_L[0]=0;
      Mode_L[1] = 0;
        target_frequency = 0;
      }else{
        target_frequency = (1.000 / (Mode_L[0] + Mode_L[1]))/2;
      }
    }else{
        target_frequency = (1.000 / (Mode_R[0] + Mode_R[1]))/2;
    }
  } else {
      target_frequency = (1.000 / (Mode_R[0] + Mode_R[1]) + 1.000 / (Mode_L[0] + Mode_L[1])) / 2.000;
  }

  target_velocity = target_frequency * target_frequency * step_length;
   
  if (current_velocity != target_velocity) {
        current_velocity = 0.85 * target_velocity + 0.15 * current_velocity;
  }

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
  Serial.print(pre_mode_R);
  Serial.print(',');
  Serial.print(pre_mode_L);
  Serial.print(',');
  Serial.println(velocity);
 // }

  /********************Vibration - start **********************/
  //vibrate when foot putting down
 update_vibrate(vibrate_counterL,a1,waveformL);
 update_vibrate(vibrate_counterR,a0, waveformR);

//update value
  pre_height1 = a1;
  pre_height0 = a0;

 }




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
