//MACHINE STATE - TIME INTERVALS FOR FUNCTIONS THAT TAKE NO ARGUMENTS
/* This function returns an unsigned long int, and takes two long int variables and a function of type timeLapseA as arguments */
unsigned long int timeLapseAFunc(long int actualTime, long int timeReference, timeLapseA functionPointer) {
  if (millis() - actualTime > timeReference) {
    functionPointer();
    actualTime = millis();
    return actualTime;
  }
  else {
    return actualTime;
  }
}

//MACHINE STATE - TIME INTERVALS FOR FUNCTIONS THAT TAKE A BYTE AS AN ARGUMENT
/* This function returns an unsigned long int, and takes two long int variables and a function of type timeLapseB as arguments.
   Type timeLapseB is similar to timeLapseA but it takes an argument */
unsigned long int timeLapseBFunc(long int actualTime, long int timeReference, byte pin,  timeLapseB functionPointer) {
  if (millis() - actualTime > timeReference) {
    functionPointer(pin);
    actualTime = millis();
    return actualTime;
  }
  else {
    return actualTime;
  }

}

//MIXER FUNCTIONS
void mixerFunc() {
  if (mixerFuncSwitch == 1) {
    if (mixerStartCheck == 0) {
      mixerStartCheck = 1;
      timeMixerStop = millis();
      digitalWrite(pump1, hl(LOW));
    }
    // timeMixer = timeLapseBFunc(timeMixer, intervalMixer, pump1, digitalOutputSwitch);

    timeMixerStop = timeLapseAFunc(timeMixerStop, referencePot[2]*timeMultiplierMixer, mixerOff);
  }
}

void mixerOff() {
  digitalWrite(pump1, hl(HIGH));
  mixerStartCheck = 0;
  mixerFuncSwitch = 0;
}


//TURNS ON/OFF A DIGITAL OUTPUT
void digitalOutputSwitch(byte pin) {
  if (digitalRead(pin) == HIGH) {
    digitalWrite(pin, LOW);
  }
  else {
    digitalWrite(pin, HIGH);
  }
}

// FOR DEVELOPMENT IF YOU USE LEDS INSTEAD OF RELAYS (or if relays are wired differently)
// NOTE!!!!!!!!!!! SWAPPING CONVENTION
// ONLY USE hl when writing pins, as scada will send unswapped
// WHEN READING pins scada will swap if SWAP_HIGHS_LOWS is 1
bool hl(bool channel){
if (SWAP_HIGHS_LOWS){
  return !channel;
}
return channel;
}

// HELPER that allows convertion from conceptual on off to LOW HIGH, it's just another low high swap, hl is the other helper
bool modeToLowHigh(bool a){
  if (a) return LOW;
  return HIGH;
}

//RELAYS OFF
void relaysOff () {
  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(HIGH));
}





// FLOW METER INTERRUPT CALLBACK
void rpm (){
  FlowPulse++;  //This function measures the rising and falling edge of the hall effect sensors signal
}



//EEPROM FUNCTIONS

void writeByteEEPROM( int deviceaddress, unsigned int eeaddress, byte data ) {
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  if (Wire.endTransmission() != 0) {
    eepromFaultCheck = 1;
  }
}


byte readByteEEPROM( int deviceaddress, unsigned int eeaddress ) {
  byte rdata;
  /* First thing we begin transmission on the I2C bus connecting the microcontroller to the EEPROM */
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  /* Next we tell the EEPROM that the transmission is over.
     When we call Wire.endTransmission, The EEPROM sends us back a value indicating:
     Output   0 .. success
              1 .. length to long for buffer
              2 .. address send, NACK received
              3 .. data send, NACK received
              4 .. other twi error (lost bus arbitration, bus error, ..) */
  if (Wire.endTransmission() != 0) {
    /* Something has gone wrong when communicating with the EEPROM, so we set the related flag to one.
       This is usefull for both the microcontroller as well as for the supervisor */
    eepromFaultCheck = 1;
    return 255;
  }
  /* If communication was succesful, we want to retrieve the last control option in use before the microcontroller was restarted/turned ON */
  Wire.requestFrom(deviceaddress, 1);
  /* If the buffer is not empty continue */
  if (Wire.available()) {
    return Wire.read();
  } return 255;
}


void writeUnsignedIntoEEPROM(int deviceaddress, unsigned int eeaddress, long int number) {
  writeByteEEPROM( addressEEPROM, eeaddress, (number >> 24) & 0xFF) ;
  delay(5);
  writeByteEEPROM( addressEEPROM, eeaddress + 1, (number >> 16) & 0xFF);
  delay(5);
  writeByteEEPROM( addressEEPROM, eeaddress + 2, (number >> 8) & 0xFF);
  delay(5);
  writeByteEEPROM( addressEEPROM, eeaddress + 3, number & 0xFF);
  delay(5);
}

long int readUnsignedIntoEEPROM(int deviceaddress, unsigned int eeaddress){
  long int byte1 = readByteEEPROM(addressEEPROM, eeaddress);
  delay(50);
  long int byte2 = readByteEEPROM(addressEEPROM, eeaddress + 1);
  delay(50);
  long int byte3 = readByteEEPROM(addressEEPROM, eeaddress + 2);
  delay(50);
  long int byte4 = readByteEEPROM(addressEEPROM, eeaddress + 3);
  delay(50);
  return (byte1 << 24) + (byte2 << 16) + (byte3 << 8) + byte4;
}


//RTC functions

byte getDay() {
  DateTime now = RTC.now();
  /* if RTC is not working it will induce the arduno to reset values in flash and external EEPROM,
    as it will think a new day has begun. Hence we check if year is 2165 (RTC I2c but not working, etc..)
  */
  if (now.year() == 2165){
    return 255;
  }
  return now.dayOfTheWeek();  // Sunday == 0
}

long int getTime() {
  DateTime now = RTC.now();
  return now.unixtime(); // we check RTC not working in operationCore usung getDay
}


//FUNCTION LCD SCREEN
void funcLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.setCursor(2, 0);
  lcd.print(SKETCH_VERSION);
  lcd.setCursor(12, 0);
  lcd.print("H:");
  lcd.setCursor(14, 0);
  lcd.print(hydrolysisPot);

  lcd.setCursor(0, 3);
  lcd.print("C.OPT:");
  lcd.setCursor(7, 3);
  lcd.print(controlOptionPot);
  lcd.setCursor(11, 3);
  lcd.print("MODE:");
  lcd.setCursor(16, 3);
  lcd.print(modePot);

  lcd.setCursor(0, 1);
  lcd.print("RTC:");
  lcd.setCursor(4, 1);
  DateTime now = RTC.now();
  if (now.year() == 2165){
    lcd.print(0);
  } else{lcd.print(1);}



  lcd.setCursor(6, 1);
  lcd.print("HTMR:");
  lcd.setCursor(11, 1);
  lcd.print((getTime() - oldUnixHydrolysis)/timeMultiplierHydrolysis);


  lcd.setCursor(0, 2);
  lcd.print("NR INJ:");
  lcd.setCursor(7, 2);
  lcd.print(loadingsDone);
  lcd.setCursor(10, 2);
  lcd.print("TOTlt:");
  lcd.setCursor(16, 2);
  lcd.print((byte)totalFlow);

}

void show(byte col, byte row, char c[2], bool d){
  lcd.setCursor(col, row);
  lcd.print(c);
  lcd.setCursor(col+3, row);
  lcd.print(d);
}

//DEGUB LCD SCREEN  just change funcLCD for this in state machine 1
void debugLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("M:");
  lcd.setCursor(2, 0);
  lcd.print(modePot);
  lcd.setCursor(5, 0);
  lcd.print(loadingsDone);
  lcd.setCursor(8, 0);
  lcd.print(digitalRead(lowLevelT2));
  lcd.setCursor(11, 0);
  lcd.print("CO:");
  lcd.setCursor(14, 0);
  lcd.print(controlOptionPot);
  lcd.setCursor(16, 0);
  lcd.print("DB:");
  lcd.setCursor(19, 0);
  lcd.print(debug);

  bool a = (loadingsDone >= referencePot[7]);
  bool b = (injLoaded < referencePot[6]);
  bool c = (dataArrayV[0] < referencePot[4]);
  bool d = ((getTime() - oldUnixRecirculation) < (referencePot[8]*100));
  bool e = (modePot == 4 || modePot == 5 || modePot == 14 || modePot == 15);
  bool f = (modePot == 1 || modePot == 2 || modePot == 11 || modePot == 12);
  bool g = (injFlag == 1);
  bool h = (modePot == 6 || modePot == 7 || modePot == 16 || modePot == 17);
  bool i = (dataArrayV[1] < referencePot[5] && dataArrayV[2] < referencePot[5] && dataArrayV[3] < referencePot[5]);

  show(1,1,"A:",a);
  show(8,1,"B:",b);
  show(14,1,"C:",c);
  show(1,2,"D:",d);
  show(8,2,"E:",e);
  show(14,2,"F:",f);
  show(1,3,"G:",g);
  show(8,3,"H:",h);
  show(14,3,"I:",i);
}


//DEGUB LCD SCREEN  just change funcLCD for this in state machine 1
void debugLCD2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TMR REF:");
  lcd.setCursor(7, 0);
  lcd.print((referencePot[1]*timeMultiplierHydrolysis));

  lcd.setCursor(0, 1);
  lcd.print("TMR:");
  lcd.setCursor(4, 1);
  lcd.print((getTime() - oldUnixHydrolysis));

   lcd.setCursor(0, 2);
   lcd.print("CO:");
   lcd.setCursor(4, 2);
   lcd.print(controlOptionPot);
   lcd.setCursor(11, 2);
   lcd.print("MODE:");
   lcd.setCursor(16, 2);
   lcd.print(modePot);

  lcd.setCursor(0, 3);
  lcd.print("HYDRO:");
  lcd.setCursor(6, 3);
  lcd.print(hydrolysisPot);

}
