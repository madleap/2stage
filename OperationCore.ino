void operationCore(byte modeBlock[GROUP_SIZE], bool modeStatusBlock[GROUP_SIZE][ACTUATORS_IN_MODES]) {
  bool a = (loadingsDone == referencePot[7] || loadingsDone > referencePot[7]);
  bool b = (injLoaded < referencePot[6]);
  bool c = (dataArrayV[0] < referencePot[4]);
  bool d = ((getTime() - oldUnixRecirculation) < (referencePot[8]*timeMultiplierRecirculation));
  bool e = (modePot == 4 || modePot == 5 || modePot == 14 || modePot == 15);
  bool f = (modePot == 1 || modePot == 2 || modePot == 11 || modePot == 12);
  bool g = (injFlag == 1);
  bool h = (modePot == 6 || modePot == 7 || modePot == 16 || modePot == 17);
  bool i = (dataArrayV[1] < referencePot[5] && dataArrayV[2] < referencePot[5] && dataArrayV[3] < referencePot[5]);


  // RTC CLOCK CHECKS:
  byte currentDay = getDay();
  /* override value so we stay in recirculation modes which are a safe default*/
  if (currentDay == 255){
    d = 1;
  }
  /* check if a new day has started, if so save new day into EEPROM and reset all injection pots
   we also check if RTC clock is throwing a default error value (check getDay)*/
  if (lastDayRecorded != currentDay && currentDay != 255 ) {
    lastDayRecorded = currentDay;
    writeByteEEPROM( addressEEPROM, lastDayRecordedAddress, currentDay);
    delay(5);
    totalFlow = 0;
    writeByteEEPROM( addressEEPROM, totalFlowAddress, 0);
    delay(5);
    loadingsDone = 0;
    writeByteEEPROM( addressEEPROM, loadingsDoneAddress, 0);
  }

  // TANK T2 CHECKS
  /* if tank T2 is empty just recirculate and take care of heating T3
   we end up here if the switch clicks during injection (lockout break). system will stay in previous mode with pump off
   for the remaining of lockout duration */
  bool t2Empty = digitalRead(lowLevelT2);
  // First enter an idle mode so we give V3 time to close (in case lowLevelT2Stop() kicked in close to the end of lockout timer)
  if (t2Empty && (modePot == 6 || modePot == 7 || modePot == 16 || modePot == 17)) {
    modeFunc(modeBlock[0], modeStatusBlock[0]);
    return;
  }
  // then for as long as there's no liquid in T2 recirculate and heat digester
  if (t2Empty && (modePot == 0 || modePot == 10 || modePot == 1 || modePot == 11 || modePot == 2 || modePot == 12)) {
    if (c==1 && i==1){
      modeFunc(modeBlock[2], modeStatusBlock[2]);
    }
    if (c==1 && i==0){
      modeFunc(modeBlock[1], modeStatusBlock[1]);
    }
    if (c==0){
      modeFunc(modeBlock[1], modeStatusBlock[1]);
    }
    return;
  }


  /* Branch with overflow and activities after injections are completed */
  if (a==1 && c==1 && i==1){
    modeFunc(modeBlock[2], modeStatusBlock[2]);
  }
  if (a==1 && c==1 && i==0){
    modeFunc(modeBlock[1], modeStatusBlock[1]);
  }
  if (a==1 && c==0){
    modeFunc(modeBlock[1], modeStatusBlock[1]);
  }


  /* Branch with injection */
  if (a==0 && g==1 && e==1 && c==1 && i==1){
    modeFunc(modeBlock[6], modeStatusBlock[6]);
  }
  if (a==0 && g==1 && e==1 && c==1 && i==0){
    modeFunc(modeBlock[7], modeStatusBlock[7]);
  }
  if (a==0 && g==1 && e==1 && c==0){
    modeFunc(modeBlock[7], modeStatusBlock[7]);
  }
  if (a==0 && g==1 && e==0 && h==1 && b==1 && c==1 && i==1){
    modeFunc(modeBlock[6], modeStatusBlock[6]);
  }
  if (a==0 && g==1 && e==0 && h==1 && b==1 && c==1 && i==0){
    modeFunc(modeBlock[7], modeStatusBlock[7]);
  }
  if (a==0 && g==1 && e==0 && h==1 && b==1 && c==0){
    modeFunc(modeBlock[7], modeStatusBlock[7]);
  }
  if (a==0 && g==1 && e==0 && h==1 && b==0 ){
    writeByteEEPROM( addressEEPROM, totalFlowAddress, (byte)totalFlow );
    debug = 9;
    injFlag = 0; // it will move to following branches
  }


  /* Branch digester idling: Allow time between recirculation and injection so that V3 can open fully */
  if (a==0 && g==1 && e==0 && h==0 && c==1 && i==1){
    modeFunc(modeBlock[5], modeStatusBlock[5]);
  }
  if (a==0 && g==1 && e==0 && h==0 && c==1 && i==0){
    modeFunc(modeBlock[4], modeStatusBlock[4]);
  }
  if (a==0 && g==1 && e==0 && h==0 && c==0){
    modeFunc(modeBlock[4], modeStatusBlock[4]);
  }

  /* Branch digester idling: Allow time between injection and recirculation so that V3 can close fully*/
  // if (a==0 && g==0 && h==1){
  //   modeFunc(modeBlock[0], modeStatusBlock[0]);
  // }

  if (a==0 && g==0 && h==1 && c==1 && i==1){
    modeFunc(modeBlock[3], modeStatusBlock[3]);
  }
  if (a==0 && g==0 && h==1 && c==1 && i==0){
    modeFunc(modeBlock[0], modeStatusBlock[0]);
  }
  if (a==0 && g==0 && h==1 && c==0){
    modeFunc(modeBlock[0], modeStatusBlock[0]);
  }

  /* Branch with recirculation */
  if (a==0 && g==0 && h==0 && d==1 && c==1 && i==1){
    modeFunc(modeBlock[2], modeStatusBlock[2]);
  }
  if (a==0 && g==0 && h==0 && d==1 && c==1 && i==0){
    modeFunc(modeBlock[1], modeStatusBlock[1]);
  }
  if (a==0 && g==0 && h==0 && d==1 && c==0){
    modeFunc(modeBlock[1], modeStatusBlock[1]);
  }
  if (a==0 && g==0 && h==0 && d==0 && f==1 && c==1 && i==1){
    modeFunc(modeBlock[9], modeStatusBlock[9]);
  }
  if (a==0 && g==0 && h==0 && d==0 && f==1 && c==1 && i==0){
    modeFunc(modeBlock[8], modeStatusBlock[8]);
  }
  if (a==0 && g==0 && h==0 && d==0 && f==1 && c==0){
    modeFunc(modeBlock[8], modeStatusBlock[8]);
  }
  if (a==0 && g==0 && h==0 && d==0 && f==0){
    injFlag = 1;
    loadingsDone ++;
    writeByteEEPROM( addressEEPROM, loadingsDoneAddress, (byte)loadingsDone);
    injLoaded = 0;
    oldUnixRecirculation = getTime();
  }
}
