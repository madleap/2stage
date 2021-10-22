//CONTROL OPTION 8: II STAGE HEATING
//STARTING POINT
void controlOption8() {
  /* This is the beginning of the tree for this control option. */
  bool c = (dataArrayV[0] < referencePot[4]);
  bool i = (dataArrayV[1] < referencePot[5] && dataArrayV[2] < referencePot[5] && dataArrayV[3] < referencePot[5]);

  /* Turn OFF separator in case it was ON in one of the other control options (those that allow it to be ON)*/
  digitalWrite(separator, hl(HIGH));

  /* Turn OFF mixer in case it was ON in one of the other control options (those that allow it to be ON)*/
  mixerOff();

  /* Check if the lockout is set. We call a function that checks a timer that breaks the lockout
     after some time from its activation (defined in intervallLockout, in milliseconds).
     The lockout timer will be restarted again at the end of this cycle if we get inside a new mode */
  if (lockoutPot == 1) {
    lockoutFunc();
  }


  if (c==1 && i==1){
    modeFunc(2, m2);
  }
  if (c==1 && i==0){
    modeFunc(1, m1);
  }
  if (c==0){
    modeFunc(1, m1);
  }


}
