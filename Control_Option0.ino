//CONTROL OPTION 0: HYDROLYSIS AUTOMATIC (full II stage)
//STARTING POINT

void controlOption0() {
  /* This is the beginning of the tree for this control option. */
  bool a = dataArrayV[5] < referencePot[0]; // hydrolysis temp
  bool b = dataArrayV[6] < referencePot[5];

  /* Turn OFF separator in case it was ON in one of the other control options (those that allow it to be ON)*/
  digitalWrite(separator, hl(HIGH));

  /* Please note that control of the mixer is dealt independently from modes via funtion pointer */
  timeMixerStart = timeLapseAFunc(timeMixerStart, referencePot[3]*timeMultiplierMixer, []() {
  mixerFuncSwitch = 1;
  });
  mixerFunc();

  /* Check if the lockout is set. We call a function that checks a timer that breaks the lockout
     after some time from its activation (defined in intervallLockout, in milliseconds).
     The lockout timer will be restarted again at the end of this cycle if we get inside a new mode */
  if (lockoutPot == 1) {
    lockoutFunc();
  }

  /* Branches */
  if (a == 1 && b == 1){
    operationCore(modeBlockB, modeStatusBlockB);
  }
  if (a == 1 && b == 0){
    operationCore(modeBlockA, modeStatusBlockA);
  }
  if (a == 0 && b == 1){
    operationCore(modeBlockA, modeStatusBlockA);
  }
  if (a == 0 && b == 0){
    operationCore(modeBlockA, modeStatusBlockA);
  }
}
