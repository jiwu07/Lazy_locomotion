
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
float h = 1.7; // subject height



/************* *************  - ************* ********************/
/************* *************  - ************* ********************/
/************* *************  - ************* ********************/
float step_length = h * h / (1.72 * 0.157 * 1.72 * 0.157); // WIP use 1.52 --GUDWIP using 0.157

float maxTime = -1;



float Mode_L[3] = { maxTime, maxTime, maxTime}; // pre time each mode left [mode4,mode1+2,mode3]
float Mode_R[3] =  { maxTime, maxTime, maxTime}; // pre time each mode right

float PhaseWidths[3] = { 0.6, 0.2, 0.2};

float PhaseWidthsR[3] = { 0.6, 0.2, 0.2};
float PhaseWidthsL[3] = { 0.6, 0.2, 0.2};

float ExpAbsPhaseR = PhaseWidths[0];
float NextAbsPhaseR = PhaseWidths[1];
float ExpAbsPhaseL = PhaseWidths[0];
float NextAbsPhaseL = PhaseWidths[1];

float PhaseWidthWeight= 0.8;
float w2 = 0.9;
float xhatL[2]={0,0};
float xhatR[2]={0,0};
float P_apostL[2][2]={{0,0},{0,0}};
float P_apostR[2][2]={{0,0},{0,0}};

bool L_up = false; //determin if lift/put down feet
bool R_up = false;

float delta_tl = 0.000f; //time count for current mode
float delta_tr = 0.000f;

float Threshold_timeChanging = 0.08;


int pre_mode_L = MODE_M4; //previous mode
int pre_mode_R = MODE_M4;

float AbsStepPace = 0;

float Threshold_VelChanging = 100;
float Threshold_VelChanging2 = 300;
int pre_height0 = 1023;
int pre_height1 = 1023;

float Threshold_up = 1000; //threshold up changing 
float Threshold_mode4 = 950; //consider is mode 4 if the sensor data is more than this
float Threshold_down = 900; // threshold min distance


int vibrate_counterL = 0; // counter for vibrate interval
int vibrate_counterR = 0;

unsigned long previousMillis = 0;  //last time
unsigned long previousMillisUnity =0;
const long time_interval =2; // update time
const long time_interval_Unity = 20; // send to unity time
}
int a0 =0;
int a1 =0;
float dt = ((float)simulation::time_interval)/1000.000f;
float stepPaceUpdate = 0;
int PrintEvery = 0;
float CurTime = 0;



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
bool update_mode(int height, int pre_height, bool& up, int& pre_mode, float& t, float mode_list[],int& vibrate_counter, float PhaseWidths[], float& ExpAbsPhase, float& NextAbsPhase) { 
    t += dt;//update time;

    if(height > simulation::Threshold_mode4){
      //as mode 4
      if(pre_mode == MODE_M3){
        //switch from 3-4
        ExpAbsPhase = NextAbsPhase;
        NextAbsPhase = NextAbsPhase + PhaseWidths[0];
        mode_list[1] = t;// update mode 3 time
        t=0;
        stepPaceUpdate = 1/(mode_list[0]+mode_list[1]+mode_list[2]);
        pre_mode = MODE_M4;
        return true;
      }
      //consider keep on the ground
      pre_mode = MODE_M4;
      return false;
      
    }

    ///hier are mode 123
    if (pre_height - height > 2 ) { //lifting up
        if (pre_mode == MODE_M3) { // if was M3, insert a M4 before goto M1 update time m3, reset the time
            up = false;
            pre_mode = MODE_M4;
            mode_list[2] =t; //update mode3 time
            t=0;//reset time
            ExpAbsPhase = NextAbsPhase;
            NextAbsPhase = NextAbsPhase + PhaseWidths[0];//+current mode width 
            stepPaceUpdate = 1/(mode_list[0]+mode_list[1]+mode_list[2]); //update pace [mode 4, 12, 3]
            return true;
        }
        if (pre_mode == MODE_M4) { // switch mode from 4-1
            //pre mode 4 then update phase width
            UpdatePhaseWidths(simulation::PhaseWidthWeight, mode_list,PhaseWidths);
            ExpAbsPhase = NextAbsPhase;
            NextAbsPhase = NextAbsPhase + PhaseWidths[1];//current mode 1
            mode_list[0] = t; // update mode 4 time
            stepPaceUpdate = 1/(mode_list[0]+mode_list[1]+mode_list[2]); //update pace
            up = true;
            pre_mode = MODE_M1;
            t = 0; // reset time
            return true;     
        }
        //if pre is mode 2/1 do nothing 
        up = true;
        pre_mode = MODE_M1;
        return false;
     } else if (height - pre_height >2 ) { // going down
        //update vibrate counter
        vibrate_counter  = vibrate_counter + height - pre_height;
        //if pre is 1 insert 2
        if (pre_mode == MODE_M1) {
            up = true;
            pre_mode = MODE_M2; // donothing
            return false; // 2 and 1 combined as 1 mode
        }
        if (pre_mode == MODE_M2) { // switch mode  from 2-3
            ExpAbsPhase = NextAbsPhase;
            NextAbsPhase = NextAbsPhase + PhaseWidths[2];
            mode_list[1] = t; // update time
            stepPaceUpdate = 1/(mode_list[0]+mode_list[1]+mode_list[2]);
            up = false; //going down
            pre_mode = MODE_M3;
            t = 0; //reset timr
            return true;
        }
        //if pre is 3 then do nothing
        up = false;
        pre_mode = MODE_M3;
        return false;
    } else {
        // not moving here and also not on the ground/not mode 4  
        //do nothing  
        if (up) {   
            pre_mode = MODE_M2;
            return false;
        }
        pre_mode = MODE_M3;
        return false;
    }
}


/************* ************* Setup - ************* ********************/
/************* ************* Setup - ************* ********************/
/************* ************* Setup - ************* ********************/
void setup() {
  Serial.begin(115200);
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
      PrintEvery++;
      CurTime = CurTime + dt;

   // check the update 
  bool isChangeL = update_mode(a1, pre_height1, L_up, pre_mode_L, delta_tl, Mode_L,vibrate_counterL,PhaseWidthsL,  ExpAbsPhaseL,  NextAbsPhaseL);
  bool isChangeR = update_mode(a0, pre_height0, R_up, pre_mode_R, delta_tr, Mode_R,vibrate_counterR,PhaseWidthsR,  ExpAbsPhaseR,  NextAbsPhaseR);



  if( isChangeL  || isChangeR){
    //Serial.print("update abssteppace");
    //Serial.print(" , ");
  AbsStepPace = AbsStepPace * (1 - w2) + stepPaceUpdate * w2;
  }
if (delta_tl > 0.5 && delta_tr > 0.5) {
  // Serial.print("here");
    //Serial.print(" , ");
    AbsStepPace = AbsStepPace * 0.9;
  }
//kalmanfilter part
  if (ExpAbsPhaseR < NextAbsPhaseR) {
    ExpAbsPhaseR = ExpAbsPhaseR + AbsStepPace * dt;
  }

  if (ExpAbsPhaseL < NextAbsPhaseL) {
    ExpAbsPhaseL = ExpAbsPhaseL + AbsStepPace * dt;
  }


KalmanFilter(ExpAbsPhaseL, AbsStepPace, (float*) xhatL, dt, (float*)P_apostL);
KalmanFilter(ExpAbsPhaseR, AbsStepPace, (float*) xhatR, dt, (float*)P_apostR);

  // Make delay for good sampling
  int time=micros();
  float t2=(float)time/1000000;
  float TimeBtw=CurTime-t2-0.048;
  if(TimeBtw>0)
  {
    delayMicroseconds((int)(TimeBtw*900));//1000));
  }
  
  //send to unity 
//currentMillis = millis();
 //if (currentMillis - previousMillisUnity >= time_interval_Unity) {
  if (PrintEvery == 10) {

  //  previousMillisUnity = currentMillis;
  //Serial.print(currentMillis);
  //Serial.print(',');
  Serial.print(a0);
  Serial.print(',');
  Serial.print(a1);
  Serial.print(',');
  //Serial.print(pre_mode_L);
 //Serial.print(',');
  //Serial.print(pre_mode_R);
  //Serial.print(',');
  //Serial.print(Mode_L[0]);
  //Serial.print(',');
  //Serial.print(Mode_L[1]);
  //Serial.print(',');
  //Serial.print(Mode_R[0]);
  //Serial.print(',');
  //Serial.print(Mode_R[1]);
  //Serial.print(',');
   //Serial.print(AbsStepPace);
    Serial.print((xhatL[1]+xhatR[1])/2);
  //  Serial.print(" , ");
    //Serial.print(delta_tl);
      //  Serial.print(" , ");
 //Serial.print(delta_tr);
        Serial.print(" , ");
    Serial.println((xhatL[0]+xhatR[0])/2);
    PrintEvery =0;
  }

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


void UpdatePhaseWidths(float w, float ModeLength[],float PhaseWidths[])
{
  float SumModeLengths = SumofElements(ModeLength);
  
  for (int i = 0; i < 3; i++)
  {
    float ModeLengthPercent= ModeLength[i]/SumModeLengths;
    PhaseWidths[i]= PhaseWidths[i]*(1-w) + ModeLengthPercent*w;
  }
  //return NewPhaseWidths;
}

//Matrix Multiplication Routine
// C = A*B
void MatrixMultiply(float* A, float* B, int m, int p, int n, float* C)
{
	// A = input matrix (m x p)
	// B = input matrix (p x n)
	// m = number of rows in A
	// p = number of columns in A = number of rows in B
	// n = number of columns in B
	// C = output matrix = A*B (m x n)
	int i, j, k;
	for (i = 0; i < m; i++)
		for(j = 0; j < n; j++)
		{
			C[n * i + j] = 0;
			for (k = 0; k < p; k++)
				C[n * i + j] = C[n * i + j] + A[p * i + k] * B[n * k + j];
		}
}

//Matrix Addition Routine
void MatrixAdd(float* A, float* B, int m, int n, float* C)
{
	// A = input matrix (m x n)
	// B = input matrix (m x n)
	// m = number of rows in A = number of rows in B
	// n = number of columns in A = number of columns in B
	// C = output matrix = A+B (m x n)
	int i, j;
	for (i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			C[n * i + j] = A[n * i + j] + B[n * i + j];
}

//Matrix Addition Routine
/*void MatrixAdd2(float* A, float* B, int m, int n, float* C)
{
	// A = input matrix (m x n)
	// B = input matrix (m x n)
	// m = number of rows in A = number of rows in B
	// n = number of columns in A = number of columns in B
	// C = output matrix = A+B (m x n)
	int i, j;
	for (i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			C[i][j] = A[i][j] + B[i][j];
}*/

void MatrixTranspose(float* A, int m, int n, float* C)
{
	// A = input matrix (m x n)
	// m = number of rows in A
	// n = number of columns in A
	// C = output matrix = the transpose of A (n x m)
	int i, j;
	for (i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			C[m * j + i] = A[n * i + j];
}

void MatrixCopy(float* A, int n, int m, float* B)
{
	int i, j;
	for (i = 0; i < m; i++)
		for(j = 0; j < n; j++)
		{
			B[n * i + j] = A[n * i + j];
		}
}

float SumofElements(float Elements[])
{
  float sum=0;
  for (int i=0; i < 3 ; i++)
  {
    sum = sum + Elements[i];
  }
  return sum;
}


void KalmanFilter(float ExpAbsPhase, float AbsStepPace, float* xhat,float dt, float* P_apost)
{
  //, float* A
  float w=1;
  float A[2][2]={{1,dt},{0,1}};
  //float C[2]={1, 0};
  //A=[1 dt; 0 1];
  //C=[1 0];
  //G=[dt^2/2; dt];
  float P_apri[2][2];
  //float Q[2][2]={{1/4*dt*dt*dt*dt*0.0001, 0},{0, dt*dt*0.0001}};//*0.01^2;%22^2;
  float Q[2][2]={{1/4*dt*dt*dt*dt, 0},{0, dt*dt}};//*0.01^2;%22^2;
  float AuxMult[2][2];//={{0f,0f},{0f,0f}};
  float AT[2][2];
  //float CT[1][2];
  float Kl[2];//={0,0};
  float xhat_pre[2];
  float dy;
  MatrixTranspose((float*)A,2,2,(float*)AT);
  //MatrixTranspose((float*)C,1,2,(float*)CT);
  float R=0.0034*0.0034;
  // KALMAN FILTER
  float y=ExpAbsPhase;
  // ----- Prediction -----
  float a_k=w*(AbsStepPace-xhat[1])/dt;
  xhat_pre[0]=xhat[0] + xhat[1]*dt+dt*dt/2*a_k;
  xhat_pre[1]=xhat[1]+dt*a_k;
  //P_apri=A*P_apost*A' + Q;
  MatrixMultiply((float*)A,(float*)P_apost,2,2,2,(float*)AuxMult); 
  MatrixMultiply((float*)AuxMult,(float*)AT,2,2,2,(float*)P_apri);
  MatrixAdd((float*)P_apri,(float*)Q,2,2,(float*)P_apri); 
  //MatrixCopy((float*)P_apri2, 2, 2, (float*)P_apri);
  // ----- Update -----
  //MatrixMultiply((float*)P_apri,(float*)CT,2,2,1,(float*)Kl);
  //float kl0=P_apri[0][0];
  //float kl1= P_apri[1][0];
  Kl[0]=P_apri[0][0];
  Kl[1]=P_apri[1][0];
  //Kr=inv(C*P_apri*C'+R);
  float Kr=1/(P_apri[0][0]+R);
  //K=Kl*Kr;
  dy=y-A[0][0]*xhat_pre[0] + A[0][1]*xhat_pre[1];
  xhat[0]= xhat_pre[0] + Kl[0]*Kr*dy;
  xhat[1]= xhat_pre[1] + Kl[1]*Kr*dy;
  float Km[2][2] = {{1-Kl[0]*Kr,0},{-Kl[1]*Kr,1}};
  MatrixMultiply((float*)Km, (float*)P_apri, 2, 2, 2, (float*)P_apost);
}
