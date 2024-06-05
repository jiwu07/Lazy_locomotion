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

// These constants won't change. They're used to give names to the pins used:
const int analog1InPin = A0;  // Analog input pin that the potentiometer is attached to
const int analog2InPin = A1;  // Analog input pin that the potentiometer is attached to
const float dt = 0.002;
int DelayMs = int(dt * 1000);

float PosTreshold = 0.1;
float VelTreshold = 0.1;

float ModeInitR[4] = { -1, -1, -1, -1 };
float ModeInitL[4] = { -1, -1, -1, -1 };
float PhaseWidths[4] = { 0.6, 0.19, 0.02, 0.19};

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

float ModeLengthR[4] = {1,1,0.1,1};
float ModeLengthL[4] = {1,1,0.1,1};

float P_apostL[2][2]={{0,0},{0,0}};
float P_apostR[2][2]={{0,0},{0,0}};

float xhatL[2]={0,0};
float xhatR[2]={0,0};


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
}

void loop() {
  // read the analog in value:
  PrintEvery++;
  CurTime = CurTime + dt;
  LeftFootPos = (float)analogRead(analog1InPin) * SignalGain;
  RightFootPos = (float)analogRead(analog2InPin) * SignalGain;

  LeftFootVel = Derivative(LeftFootPos, PrevLeftFootPos, PrevLeftFootVel, dt, w);
  RightFootVel = Derivative(RightFootPos, PrevRightFootPos, PrevRightFootVel, dt, w);

  LFMode = DistinguishRegions(LeftFootPos, LeftFootVel, PosTreshold, VelTreshold, LFPrevMode);
  RFMode = DistinguishRegions(RightFootPos, RightFootVel, PosTreshold, VelTreshold, RFPrevMode);


  if (LFMode != LFPrevMode) {
    int ModeLoc = LFMode - 1;
    float TimeInterval = CurTime - ModeInitL[ModeLoc];
    //ModeLengthL[LFPrevMode-1]=TimeAfterLastL;
    /*if(LFMode==1)
    {
    UpdatePhaseWidths(PhaseWidthWeight, ModeLengthL, PhaseWidths);
    }*/
    ModeInitL[ModeLoc] = CurTime;
    ExpAbsPhaseL = NextAbsPhaseL;
    NextAbsPhaseL = NextAbsPhaseL + PhaseWidths[ModeLoc];
    TimeAfterLastL = 0;
    StepPaceUpdate = 1 / TimeInterval;
  }

  if (RFMode != RFPrevMode) {
    int ModeLoc = RFMode - 1;
    float TimeInterval = CurTime - ModeInitR[ModeLoc];
    //ModeLengthR[RFPrevMode-1]=TimeAfterLastR;
    /*if(RFMode==1)
    {
    UpdatePhaseWidths(PhaseWidthWeight, ModeLengthR,PhaseWidths);
    }*/
    ModeInitR[ModeLoc] = CurTime;
    ExpAbsPhaseR = NextAbsPhaseR;
    NextAbsPhaseR = NextAbsPhaseR + PhaseWidths[ModeLoc];
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

  // print the results to the Serial Monitor:
  if (PrintEvery == 10) {
    /*Serial.print("Current Time= ");
    Serial.print(CurTime);
    Serial.print(" , ");
    Serial.print(LFMode);
    Serial.print(" , ");

    Serial.print(RFMode);
    Serial.print(" , ");


    Serial.print("ExpStepPace= ");
    Serial.print(" , ");*/
    Serial.print(AbsStepPace);
    

        //Serial.print("Gait Phase");
    //Serial.print(ExpAbsPhaseL);
    //Serial.print(" , ");
        //Serial.print("Gait Phase");
    //Serial.print(ExpAbsPhaseR);
    Serial.print(" , ");
    Serial.print((xhatL[0]+xhatR[0])/2);
            //Serial.print("Gait Phase");
    //Serial.print((xhatL[0]+xhatR[0])/2);
    //Serial.print(" , ");
        //Serial.print("Gait Phase");
    //Serial.print(xhatR[0]);
    //Serial.print(" , ");

    //Serial.print("Gait Phase");
    //Serial.print(ExpAbsPhase);

    Serial.println();
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
  delayMicroseconds(700);
}

int DistinguishRegions(int Data, float dData, float PosTreshold, float VelTreshold, int PrevMode) {
  bool MovingUp = dData > 0;
  bool OffGround = Data > PosTreshold;
  bool MovingFast = abs(dData) > VelTreshold;
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

float Derivative(float Data, float prevData, float PrevdData, float dt, float w) {
  float dData = (Data - prevData) / dt;
  float filtdData = dData * w + PrevdData * (1 - w);
  return filtdData;
}

void UpdatePhaseWidths(float w, float ModeLength[],float* PhaseWidths)
{
  float SumModeLengths = SumofElements(ModeLength);
  //float NewPhaseWidths[4];
  for (int i = 0; i <= 3; i++)
  {
    float ModeLengthPercent= ModeLength[i]/SumModeLengths;
    PhaseWidths[i]= PhaseWidths[i]*(1-w) + ModeLengthPercent*w;
  }
  //return NewPhaseWidths;
}

float SumofElements(float Elements[])
{
  float sum=0;
  for (int i=0; i< sizeof(Elements); i++)
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

