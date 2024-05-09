#include <Arduino.h>
#include <Audio.h>
#include "MultiMap.h"

//code part
namespace simulation{

//define the stepping into 4 mode
#define MODE_M1 1
#define MODE_M2 2
#define MODE_M3 3
#define MODE_M4 4

float h = 1.6; // subject height
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

float time_interval = 20;

}
int analogValueA0 =0;
int a1 =0;
int analogValueA1 =0;
int a0 =0;


//check the string situation and update the current mode
void update_mode(int height, int pre_height, bool& up, int& pre_mode, float& t, float mode_list[]) { 
    float temp = 0.02;
    if (pre_height - height > 2) { // going up
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
      
        //M4 stop vibration
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
    if (t > 0.8) { // too long time not move feet then stop
        return true;
    }
    
    pre_mode = MODE_M4;
    return false;
}

void setup() {
  Serial.begin(9600);
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
 
  //Serial.println(a0);
  //Serial.println(a1);

  update_mode(a1, pre_height1, L_up, pre_mode_L, delta_tl, Mode_L);
  update_mode(a0, pre_height0, R_up, pre_mode_R, delta_tr, Mode_R);


//////// if stopping for too long time then consider it as stop walking
  if (if_stop(delta_tl, pre_mode_L)) {
      Mode_L[0] = 1.0;
      Mode_L[1] = 1.0;
  }
  if (if_stop(delta_tr, pre_mode_R)) {
      Mode_R[0] = 1.0;
      Mode_R[1] = 1.0;
  }



//Serial.println(Mode_L[0]);
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

  // Send data to Unity
  Serial.print(a0);
  Serial.print(',');
  Serial.print(a1);
  Serial.print(',');
  Serial.println(velocity);

  
  
  pre_height1 = a1;
  pre_height0 = a0;



  delay(time_interval);
}
