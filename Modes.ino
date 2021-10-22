/* The following state machines represent all the possible operational states of the system in terms of actuators.

   They all follow this logic:
   Depending upon the readings from the sensors, the control flow has determined what mode (state) the system should operate at.
   Let's assume it's different from the one we currently operating at (identified by modePot).
   Let's call the new one modeX and the old one OldMode;

   The lockout flag would be now equal to 1, and the OldMode WILL BE CARRIED OUT UNTIL:
   - the lockout is disabled by a fault --> lockout will be set to 0 - DISCONTINUED FOR POPLAR 2-STAGE AD
   - similar to a fault, the lockout is manually overridden in the code to immediately cease
     the given mode and enter the next one.
     **** MAKE SURE YOU ARE NOT IN A MODE THAT HAS TO BE CARRIED OUT TILL THE END ****
     like opening or closing a valve (examples, V3, V4)
   - the lockout is disabled by its timer --> lockout will be set to 0

   During this period, every time we enter State 6 in stateMachine(), we go through the control flow tree, and if the readings from the sensors haven't changed,
   we will keep entering the state machine modeX, although we will be still executing OldMode.
   This is because the actuators haven't yet received any signal to change their status.

   When the lockout flag is finally set to a value of 0, upon entering modeX we will
   set modPot=modeX, lockout to 1, change actuators' status and finally operate under modeX;
   and we will be in an analogous situation of the point above;

   During operation under modeX:
   ModeX might enter unsafe operation areas, or reach its targets before the lockout timer has gone off.
   In this case, the relevant actuators are turned off.
   This is to avoid keep writing digital pins, similarly to case 0 to case 1.
   However, it is quite possible that ModeX might not reach its targets before the lockout timer goes off. In this case, if the readings from the sensors still
   send us to modeX, the lockout timer will be restarted and nextModePot=modePot=modeX. */




//FUNCTION:mode function helper
void modeFunc(byte modeNumber, bool modeBlock[ACTUATORS_IN_MODES]) {
 nextModePot = modeNumber;
 switch (lockoutPot) {
   case 0:
     modePot = modeNumber;
     lockoutPot = 1;
     for (byte k = 0; k < sizeof(actuatorsInModes); k++){
       digitalWrite(actuatorsInModes[k], hl(modeToLowHigh(modeBlock[k])));
     }
     break;
   case 1:
     lockoutBreaker();
     break;
 }
}

//TIMER FUNCTION FOR LOCKOUT
/* The lockout function locks the system to a particular mode for intervalLockout amount of time */
void lockoutFunc() {
  /* Following is the timer which breaks the mode lockout after intervallLockout milliseconds from its activation.
     We are just passing a lambda function as pointer function to timeLapseFunc.
     The lambda changes lockoutPot value to zero once executed */
  timeLockout = timeLapseAFunc(timeLockout, intervalLockout, []() {
    lockoutPot = 0;
  });
}

void forceLockoutReset (){
  timeLockout = millis();
  lockoutPot = 0;
}

//CHECK IF LOCKOUT IS ACTIVE OR NOT (aka MODE IS LOCKED-UNLOCKED)
byte lockoutCheck() {
  if (modePot == nextModePot) {
    return 0;
  }
  else {
    return 1;
  }
}

/*LOCKOUT BREAKER
  Some of the following checks are already in the flowchart.
  However, as the lockout is overwriting them, for safety reasons the lockout breaker will go through them again.
  It's important to notice that some of the following checks might act upon different reference values than those in the flowchart (to avoid cycling).
*/
void lockoutBreaker() {
  for (byte k=0; k< sizeof(injModes);k++){
    if (modePot == injModes[k]){
      injectionStop();
      lowLevelT2Stop();
      }
    }
  for (byte k=0; k< sizeof(digesterHeaterModes);k++){
    if (modePot == digesterHeaterModes[k]){
      digesterPadStop();
      }
    }
  for (byte k=0; k< sizeof(hydrolysisHeaterModes);k++){
    if (modePot == hydrolysisHeaterModes[k]){
      hydrolysisPadStop();
      }
    }
}

void digesterPadStop() {
 /* Pads are too hot, stop immediately*/
 bool a = dataArrayV[1] > referencePot[5];
 bool b = dataArrayV[2] > referencePot[5];
 bool c = dataArrayV[3] > referencePot[5];
 if (a || b || c) {
   digitalWrite(heater2, hl(HIGH));
 }
}

void hydrolysisPadStop() {
 /* Pads are too hot, stop immediately*/
 bool a = dataArrayV[6] > referencePot[5];
 if (a) {
   digitalWrite(heater1, hl(HIGH));
 }
}

void injectionStop(){
  bool a =(injLoaded > referencePot[6]);
  if (a) {
    digitalWrite(pump3, hl(HIGH));
  }
}

void lowLevelT2Stop(){
  if (digitalRead(lowLevelT2)){
    digitalWrite(pump3, hl(HIGH));
    digitalWrite(valve3, hl(HIGH));
  }
}


//FUNCTION: MODE22 System is inactive, either control option has not been selected at startup or after commissioning, or eeprom was found faulty at startup
void idle() {
  nextModePot = 22;
  modePot = 22;
  relaysOff();
}
