/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground through 220 ohm resistor

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInOutSerial
*/

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


// These constants won't change. They're used to give names to the pins used:
const int analog1InPin = A0;  // Analog input pin that the potentiometer is attached to
const int analog2InPin = A1;  // Analog input pin that the potentiometer is attached to
const float dt = 0.002;
float time_interval = dt * 1000;
float time_interval_Unity = 20;

float previousMillisUnity =0;
float previousMillis=0;

float PosTreshold = 0.1;
float VelTreshold = 0.1;

float ModeInitR[3] = { -1, -1, -1};
float ModeInitL[3] = { -1, -1, -1};
float PhaseWidths[3] = { 0.6, 0.2, 0.2};

float PhaseWidthsR[3] = { 0.6, 0.2, 0.2};
float PhaseWidthsL[3] = { 0.6, 0.2, 0.2};

float w = 0.9;
float w2 = 0.9;
float PhaseWidthWeight= 0.8;

float LeftFootPos = 0;       // value read from the pot
float RightFootPos = 0;      // value read from the pot
float PrevLeftFootPos = 0;   // value read from the pot
float PrevRightFootPos = 0;  // value read from the pot

float LeftFootVel = 0;       // value read from the pot
float RightFootVel = 0;      // value read from the pot
float PrevLeftFootVel = 0;   // value read from the pot
float PrevRightFootVel = 0;  // value read from the pot

float CurTime = 0;
float TimeAfterLastR = 0;
float TimeAfterLastL = 0;
int LFPrevMode = 1;
int LFMode = 1;

int RFPrevMode = 1;
int RFMode = 1;

int PrintEvery = 0;

float AbsStepPace = 0;
float ExpAbsPhaseR = PhaseWidths[0];
float NextAbsPhaseR = PhaseWidths[1];
float ExpAbsPhaseL = PhaseWidths[0];
float NextAbsPhaseL = PhaseWidths[1];
float StepPaceUpdate = 0;
float SignalGain = 0.005;

float ModeLengthR[3] = {1,1,1};
float ModeLengthL[3] = {1,1,1};

float P_apostL[2][2]={{0,0},{0,0}};
float P_apostR[2][2]={{0,0},{0,0}};

float xhatL[2]={0,0};
float xhatR[2]={0,0};

int vibrate_counterR = 0;
int vibrate_counterL = 0;


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
}

void loop() {
  // read the analog in value:
    unsigned long currentMillis = millis();  // current time
  
  //if the time interval is reached
  if (currentMillis - previousMillis >= time_interval) {
    previousMillis = currentMillis;  //update the previous time
  PrintEvery++;
  CurTime = CurTime + dt;
  LeftFootPos = (1023-(float)analogRead(analog1InPin)) * SignalGain;
  RightFootPos = (1023-(float)analogRead(analog2InPin)) * SignalGain;

  LeftFootVel = Derivative(LeftFootPos, PrevLeftFootPos, PrevLeftFootVel, dt, w);
  RightFootVel = Derivative(RightFootPos, PrevRightFootPos, PrevRightFootVel, dt, w);

  LFMode = DistinguishRegions(LeftFootPos, LeftFootVel, PosTreshold, VelTreshold, LFPrevMode, vibrate_counterL, PrevLeftFootPos);
  RFMode = DistinguishRegions(RightFootPos, RightFootVel, PosTreshold, VelTreshold, RFPrevMode, vibrate_counterR,PrevRightFootPos);


  if (LFMode != LFPrevMode) {
    int ModeLoc = LFMode - 1;
    float TimeInterval = CurTime - ModeInitL[ModeLoc];
    ModeLengthL[LFPrevMode-1]=TimeAfterLastL;
    if(LFMode==1)
    {
    UpdatePhaseWidths(PhaseWidthWeight, (float*)ModeLengthL, (float*)PhaseWidthsL);
    }
    ModeInitL[ModeLoc] = CurTime;
    ExpAbsPhaseL = NextAbsPhaseL;
    NextAbsPhaseL = NextAbsPhaseL + PhaseWidthsL[ModeLoc];
    TimeAfterLastL = 0;
    StepPaceUpdate = 1 / TimeInterval;
  }

  if (RFMode != RFPrevMode) {
    int ModeLoc = RFMode - 1;
    float TimeInterval = CurTime - ModeInitR[ModeLoc];
    ModeLengthR[RFPrevMode-1]=TimeAfterLastR;
    if(RFMode==1)
    {
    UpdatePhaseWidths(PhaseWidthWeight, (float*)ModeLengthR,(float*)PhaseWidthsR);
    }
    ModeInitR[ModeLoc] = CurTime;
    ExpAbsPhaseR = NextAbsPhaseR;
    NextAbsPhaseR = NextAbsPhaseR + PhaseWidthsR[ModeLoc];
    TimeAfterLastR = 0;
    StepPaceUpdate = 1 / TimeInterval;
  }

  if (RFMode != RFPrevMode || LFMode != LFPrevMode) {
    AbsStepPace = AbsStepPace * (1 - w2) + StepPaceUpdate * w2;
  }

  if (TimeAfterLastR > 0.5 && TimeAfterLastL > 0.5) {
    AbsStepPace = AbsStepPace * 0.9;
  }

  if (ExpAbsPhaseR < NextAbsPhaseR) {
    ExpAbsPhaseR = ExpAbsPhaseR + AbsStepPace * dt;
  }

  if (ExpAbsPhaseL < NextAbsPhaseL) {
    ExpAbsPhaseL = ExpAbsPhaseL + AbsStepPace * dt;
  }

  float ExpAbsPhase = (ExpAbsPhaseL + ExpAbsPhaseR) / 2;

  // Kalman Filter
  //float P_apriL[2][2];//={{0,0},{0,0}};
  KalmanFilter(ExpAbsPhaseL, AbsStepPace, (float*) xhatL, dt, (float*)P_apostL);
  KalmanFilter(ExpAbsPhaseR, AbsStepPace, (float*) xhatR, dt, (float*)P_apostR);

  /********************Vibration - start **********************/
  //vibrate when foot putting down
  update_vibrate(vibrate_counterL,LeftFootPos,waveformL);
  update_vibrate(vibrate_counterR,RightFootPos, waveformR);


  // Make delay for good sampling
  int time=micros();
  float t2=(float)time/1000000;
  float TimeBtw=CurTime-t2-0.05;
  if(TimeBtw>0)
  {
    delayMicroseconds((int)(TimeBtw*900));//1000));
  }
  
  // print the results to the Serial Monitor:
  //if (currentMillis - previousMillisUnity >= time_interval_Unity) {
    //previousMillisUnity = currentMillis;
    if (PrintEvery == 10) {
     //   Serial.print(CurTime);
//Serial.print(" , ");
    // Serial.print("Current Time= ");
    Serial.print(CurTime);
   /*Serial.print(" , ");
    Serial.print(LFMode);
    Serial.print(" , ");

    Serial.print(RFMode);
    Serial.print(" , ");


    Serial.print("ExpStepPace= ");
    Serial.print(" , ");*/

    //Serial.print(LeftFootPos);
    Serial.print(" , ");
    Serial.print(RightFootPos);
    Serial.print(" , ");
    //Serial.print(AbsStepPace);
    Serial.print((xhatL[1]+xhatR[1])/2);
    Serial.print(" , ");
    Serial.print((xhatL[0]+xhatR[0])/2);

    Serial.println();

    //PrintPhaseWidths((float*) PhaseWidthsL);
    PrintEvery = 0;
  }

  TimeAfterLastR = TimeAfterLastR + dt;
  TimeAfterLastL = TimeAfterLastL + dt;
  // Update previous values

  PrevLeftFootPos = LeftFootPos;
  PrevRightFootPos = RightFootPos;
  PrevLeftFootVel = LeftFootVel;
  PrevRightFootVel = RightFootVel;
  LFPrevMode = LFMode;
  RFPrevMode = RFMode;

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  //delay(DelayMs);
}
}
void update_vibrate(int& vibrate_counter, int sensordata, AudioSynthWaveform& signal ){
  if(vibrate_counter > 10){
    vibrate(sensordata,signal);
   // Serial.println("count");
    vibrate_counter = 0;
  }
}

int DistinguishRegions(int Data, float dData, float PosTreshold, float VelTreshold, int PrevMode, int vibrate_counter,  int prevData) {
  bool MovingUp = dData > 0;
  bool OffGround = Data > PosTreshold;
  bool MovingFast = abs(dData) > VelTreshold;
  int NewMode = PrevMode;

  if (PrevMode == 1) {
    if (OffGround && MovingUp) {
      NewMode = 2;
    }
  } 
  else if (PrevMode == 2) {
    if (!MovingUp && MovingFast) {
      NewMode = 3;
    }
  } else if (PrevMode == 3) {
    if (!OffGround) {
      NewMode = 1;
    }
    //vibrate
    vibrate_counter  = vibrate_counter - Data + prevData;

  }

  return NewMode;
}

float Derivative(float Data, float prevData, float PrevdData, float dt, float w) {
  float dData = (Data - prevData) / dt;
  float filtdData = dData * w + PrevdData * (1 - w);
  return filtdData;
}

void UpdatePhaseWidths(float w, float* ModeLength,float* PhaseWidths)
{
  float SumModeLengths = SumofElements(ModeLength);
  
  for (int i = 0; i < 3; i++)
  {
    float ModeLengthPercent= ModeLength[i]/SumModeLengths;
    PhaseWidths[i]= PhaseWidths[i]*(1-w) + ModeLengthPercent*w;
  }
  //return NewPhaseWidths;
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

void KalmanFilter(float ExpAbsPhase, float AbsStepPace, float* xhat,float dt, float* P_apost)
{
  //, float* A
  float w=1;
  float A[2][2]={{1,dt},{0,1}};
  float C[2]={1, 0};
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

void PrintPhaseWidths(float* PhaseWidths)
{
  Serial.print("Phases: ");
  for (int i=0;i<3;i++)
  {
    Serial.print(": ");
    Serial.print("Phase ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(PhaseWidths[i]);

  }
  Serial.println();
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
