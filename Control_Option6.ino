//CONTROL OPTION 6: HYDROLYSIS AUTOMATIC (full II stage) WITH TIMER ON HEATING TO ALLOW COOLING DOWN
//STARTING POINT

void controlOption6() {
  /* This is the beginning of the tree for this control option. */
  bool a = (dataArrayV[5] < referencePot[0]); // hydrolysis temp
  bool b = (dataArrayV[6] < referencePot[5]); // heater stat
  bool c = (hydrolysisPot == 1);
  bool d = (hydrolysisPot == 2);
  bool e = ((getTime() - oldUnixHydrolysis) < (referencePot[1]*timeMultiplierHydrolysis));

  /* Turn OFF separator in case it was ON in one of the other control options (those that allow it to be ON)*/
  digitalWrite(separator, hl(HIGH));

  /* If we come from NO HYDROLYSIS STATUS (hydrolysisPot = 0), when serial command is received, condition d becomes true
  because it is changed from 0 to 2 inside the serial function, then we come here and do some eeprom writing.
  Because there's a lot of eeprom writing - and use of delays - have this here instead of inside serial function
  NOTE: to avoid this you could restructure writeUnsignedIntoEEPROM with state machines as done for reference points*/
  if (d == 1){
    oldUnixHydrolysis = getTime();
    hydrolysisPot = 1;
    writeByteEEPROM( addressEEPROM, hydrolysisPotAddress, hydrolysisPot);
    delay(5);
    writeUnsignedIntoEEPROM( addressEEPROM, hydrolysisTimerAddress, oldUnixHydrolysis);
  }


  /* Only have mixer during heating/hydrolysis
     Please note that control of the mixer is dealt independently from modes via funtion pointer */
  if (c == 1) {
    timeMixerStart = timeLapseAFunc(timeMixerStart, referencePot[3]*timeMultiplierMixer, []() {
      mixerFuncSwitch = 1;
    });
    mixerFunc();
  } else{
    mixerOff();
    operationCore(modeBlockA, modeStatusBlockA);
  }

  /* Check if the lockout is set. We call a function that checks a timer that breaks the lockout
     after some time from its activation (defined in intervallLockout, in milliseconds).
     The lockout timer will be restarted again at the end of this cycle if we get inside a new mode */
  if (lockoutPot == 1) {
    lockoutFunc();
  }


  /* Branches */
  if (d == 0 && c == 1 && e == 1 && a == 1 && b == 1){
    operationCore(modeBlockB, modeStatusBlockB);
  }
  if (d == 0 && c == 1 && e == 1 && a == 1 && b == 0){
    operationCore(modeBlockA, modeStatusBlockA);
  }
  if (d == 0 && c == 1 && e == 1 && a == 0){
    operationCore(modeBlockA, modeStatusBlockA);
  }
  if (d == 0 && c == 1 && e == 0){
    hydrolysisPot = 0;
    writeByteEEPROM( addressEEPROM, hydrolysisPotAddress, hydrolysisPot);
  }
}
