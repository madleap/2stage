//CONTROL OPTION 8: HYDRAULYSIS + II STAGE HEATING
//STARTING POINT
void controlOption7() {
  /* This is the beginning of the tree for this control option. */
  bool a = dataArrayV[5] < referencePot[0];
  bool b = dataArrayV[6] < referencePot[5];
  bool c = (dataArrayV[0] < referencePot[4]);
  bool i = (dataArrayV[1] < referencePot[5] && dataArrayV[2] < referencePot[5] && dataArrayV[3] < referencePot[5]);

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


  if (a==1 && b==1 && c==1 && i==1){
    modeFunc(12, m12);
  }
  if (a==1 && b==1 && c==1 && i==0){
    modeFunc(11, m11);
  }
  if (a==1 && b==1 && c==0){
    modeFunc(11, m11);
  }
  if (a==1 && b==0 && c==1 && i==1){
    modeFunc(2, m2);
  }
  if (a==1 && b==0 && c==1 && i==0){
    modeFunc(1, m1);
  }
  if (a==1 && b==0 && c==0){
    modeFunc(1, m1);
  }
  if (a==0 && c==1 && i==1){
    modeFunc(2, m2);
  }
  if (a==0 && c==1 && i==0){
    modeFunc(1, m1);
  }
  if (a==0 && c==0){
    modeFunc(1, m1);
  }


}
