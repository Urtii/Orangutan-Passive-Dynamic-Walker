/* RandoWalkerTall
by Amy R. Wu and Matthew A. Robertson
modified 20 May 2017
adapted to Particle Photon by Onurcan YILMAZ
*/

#include "MPU6050.h"
#include "MQTT.h"

Servo servoRL;  // create servo object to control a servo
Servo servoRM;
Servo servoLL;
Servo servoLM;

// settings
// RL: mid = 74, down = 110, up = 50
// RM: mid = 25, down = 0, up = 50
// LL: mid = 25, down = 0, up = 50
// LM: mid = 74, down = 100, up = 50

// right lateral
int down_RL = 40;
int mid_RL = 74;
int up_RL = 110; // up = increase

// right medial
int down_RM = 0;
int mid_RM = 20;
int up_RM = 50; // up = increase

// left lateral
int down_LL = 55;
int mid_LL = 25;
int up_LL = 0; // up = decrease

// left medial
int down_LM = 105;
int mid_LM = 60;
int up_LM = 30; // up = decrease

int stepDelay = 240;

const int footSwitchM = 2;
const int footSwitchL = 12;
int switchState = 1;

// delays
int contactDelay = 0;
int pushOff = 60;
int swing = 300; //220, 300, 270, 250

unsigned int stepTimeCounter = 0;
unsigned long time1;
unsigned long time2;
int pushoff_RL = 0;
int pushoff_LL = 0;
int pushoff_RM = 0;
int pushoff_LM = 0;
int pushoffTimeSFM = 800; //550, 800, 650
int pushoffTimeSFL = 800; //550, 800, 650, 450

//Timers
//laterals
Timer lateral_contactDelay(999999,contactDelay_lateral,true);
Timer lateral_pushOff(999999,pushoff_lateral,true);
Timer lateral_swing(999999,swing_lateral,true);
//medials
Timer medial_contactDelay(999999,contactDelay_medial,true);
Timer medial_pushOff(999999,pushoff_medial,true);
Timer medial_swing(999999,swing_medial,true);


//callback functions

//laterals
void contactDelay_lateral(){
  time1 = millis();
  //     Serial.println(time1-time2);

  pushoff_RL = mid_RL - (mid_RL-down_RL)*(time1-time2)/pushoffTimeSFL; //450
  if (pushoff_RL < down_RL) {
    pushoff_RL = down_RL;
  };

  pushoff_LL = mid_LL + (down_LL-mid_LL)*(time1-time2)/pushoffTimeSFL;
  if (pushoff_LL > down_LL) {
    pushoff_LL = down_LL;
  };

  // Medial in stance
  // LATERAL (outer):

  servoRL.write(pushoff_RL);   //PUSH OFF
  servoLL.write(pushoff_LL);  //PUSH OFF

  //lateral_contactDelay.stopFromISR();
  lateral_pushOff.changePeriodFromISR(pushOff);
}

void pushoff_lateral(){
  servoRL.write(up_RL);     //RETRACT for swing
  servoLL.write(up_LL);     //RETRACT for swing

  //lateral_pushOff.stopFromISR();
  lateral_swing.changePeriodFromISR(swing);

}

void swing_lateral(){               //SWING

  servoRL.write(mid_RL);    //RESET
  servoLL.write(mid_LL);    //RESET

  switchState = !switchState;

  //lateral_swing.stopFromISR();

}


//medials
void contactDelay_medial(){

  time2 = millis();
  //     Serial.println(time2-time1);

  pushoff_RM = mid_RM + (down_RM-mid_RM)*(time2-time1)/pushoffTimeSFM;
  if (pushoff_RM > down_RM) {
    pushoff_RM = down_RM;
  };

  pushoff_LM = mid_LM - (mid_LM-down_LM)*(time2-time1)/pushoffTimeSFM;
  if (pushoff_LM < down_LM) {
    pushoff_LM = down_LM;
  };

  /*   Serial.println(0);
  Serial.println(pushoff_RM-mid_RM);
  Serial.println(pushoff_LM-mid_LM);
  Serial.println(0);
  */

  // Lateral in stance
  // MEDIAL (inner):
  servoRM.write(pushoff_RM);   //PUSH OFF
  servoLM.write(pushoff_LM);   //PUSH OFF

  //medial_contactDelay.stopFromISR();
  medial_pushOff.changePeriodFromISR(pushOff);

}

void pushoff_medial(){
  servoLM.write(up_LM);     //RETRACT for swing
  servoRM.write(up_RM);     //RETRACT for swing

  //medial_pushOff.stopFromISR();
  medial_swing.changePeriodFromISR(swing);

}

void swing_medial(){               //SWING

  servoRM.write(mid_RM);    //RESET
  servoLM.write(mid_LM);    //RESET

  switchState = !switchState;

  //medial_swing.stopFromISR();

}


void setup() {
  pinMode(footSwitchM, INPUT);
  pinMode(footSwitchL, INPUT);

  servoRL.attach(18);  // attaches the servo on pin x to the servo object
  servoRM.attach(17);  // attaches the servo on pin x to the servo object
  servoLM.attach(15);  // attaches the servo on pin x to the servo object
  servoLL.attach(14);  // attaches the servo on pin x to the servo object
  servoRL.write(mid_RL);  // tell servo to go to position in variable 'pos'
  servoLL.write(mid_LL);
  servoRM.write(mid_RM);
  servoLM.write(mid_LM);

  lateral_contactDelay.stop();
  lateral_pushOff.stop();
  lateral_swing.stop();
  medial_contactDelay.stop();
  medial_pushOff.stop();
  medial_swing.stop();
  delay(2000);

  //   Serial.begin(9600);
}

void loop() {

  int buttonStateM = digitalRead(footSwitchM);
  int buttonStateL = digitalRead(footSwitchL);

  if(buttonStateM == HIGH && switchState){ //if MEDIAL contact...
    lateral_contactDelay.changePeriod(contactDelay);
    /* delayed version
    delay(contactDelay);

    time1 = millis();
    //     Serial.println(time1-time2);

    pushoff_RL = mid_RL - (mid_RL-down_RL)*(time1-time2)/pushoffTimeSFL; //450
    if (pushoff_RL < down_RL) {
    pushoff_RL = down_RL;
  };

  pushoff_LL = mid_LL + (down_LL-mid_LL)*(time1-time2)/pushoffTimeSFL;
  if (pushoff_LL > down_LL) {
  pushoff_LL = down_LL;
};

// Medial in stance
// LATERAL (outer):

servoRL.write(pushoff_RL);   //PUSH OFF
servoLL.write(pushoff_LL);  //PUSH OFF
delay(pushOff);
servoRL.write(up_RL);     //RETRACT for swing
servoLL.write(up_LL);     //RETRACT for swing

delay(swing);               //SWING

servoRL.write(mid_RL);    //RESET
servoLL.write(mid_LL);    //RESET

switchState = !switchState;
*/
}
else if(buttonStateL == HIGH && !switchState){ //if LATERAL contact...
  medial_contactDelay.changePeriod(contactDelay);


  /*delayed version
  delay(contactDelay);

  time2 = millis();
  //     Serial.println(time2-time1);

  pushoff_RM = mid_RM + (down_RM-mid_RM)*(time2-time1)/pushoffTimeSFM;
  if (pushoff_RM > down_RM) {
  pushoff_RM = down_RM;
};

pushoff_LM = mid_LM - (mid_LM-down_LM)*(time2-time1)/pushoffTimeSFM;
if (pushoff_LM < down_LM) {
pushoff_LM = down_LM;
};

//Serial.println(0);
//Serial.println(pushoff_RM-mid_RM);
//Serial.println(pushoff_LM-mid_LM);
//Serial.println(0);


// Lateral in stance
// MEDIAL (inner):
servoRM.write(pushoff_RM);   //PUSH OFF
servoLM.write(pushoff_LM);   //PUSH OFF
delay(pushOff);
servoLM.write(up_LM);     //RETRACT for swing
servoRM.write(up_RM);     //RETRACT for swing

delay(swing);               //SWING

servoRM.write(mid_RM);    //RESET
servoLM.write(mid_LM);    //RESET

switchState = !switchState;
*/
}



}
