/* It writes reference values into EEPROM */
void function244() {
  int k;
  long int locTime = millis();

  /* It might take a few milliseconds to receive the stream. If time is over or the buffer has received all the values then continue,
  otherwise remain inside the while loop and do nothing */
  while ((millis() - locTime) < intervalStateMachine2 && Serial.available() <= sizeof(referencePot)) {
  }

  /* Store values received */
  for (k = 0; k < sizeof(referencePot); k++) {
    I2CBuffer[k] = Serial.read();
  }

  /* Store the crc value received from the supervisor */
  byte crcPot = Serial.read();
  /* If the crc we have received from the supervisor is equal to the one we calculated based on the values received: */
  if (crcPot == crcArray(I2CBuffer, sizeof(I2CBuffer))) {
    /* As it will take a series of cycles to write to the EEPROM,
       to keep the code neater we are going to stay inside state machine 2, but move to case 1
       of its outer switch case */
    stateMachine2Check = 1;
  }
}



/* Write Gas valve switch. It is used to turn the gas hob ON or OFF */
void function245() {
  Serial.println(245);
  gasValvePot = Serial.read();

  /* Send status of gas valve and a crc to confirm we received instruction and acted upon */
  Serial.println(gasValvePot);
  Serial.println(crcSingle(gasValvePot));
}



/* It writes the control option. It is used to select a control option from the supervisor */
void function246() {
  byte potCheck;
  Serial.println(246);
  previousControlOptionPot = controlOptionPot;
  potCheck = Serial.read();
  for (byte c = 0; c < numControlOptions + 1; c++) {
    if (potCheck == c) {
      controlOptionPot = potCheck;
      Serial.println(controlOptionPot);
      Serial.println(crcSingle(controlOptionPot));
      if (previousControlOptionPot != controlOptionPot) {
        /* Here we save a new control value into EEPROM.
          In so doing, if aruino goes through a reset, it will continue from where it has left.
          Because it's a single value, we don't need a dedicated state machine to deal with this. */
        writeByteEEPROM( addressEEPROM, controlOptionAddress, potCheck );
        lockoutPot = 0;
        stateMachineCheck = 6;
      }
      break;
    }
  }
}



/* Serial communication test */
void function248() {
  long int locTime = millis();

  /* It might take a few milliseconds to receive the stream. If time is over or the buffer has received all the values then continue,
  otherwise remain inside the while loop and do nothing */
  while ((millis() - locTime) < 1 && Serial.available() <= 3) {
  }

  byte  localBuffer = Serial.read();
  byte localCrc = Serial.read();

  /* If the crc we have received from the supervisor is equal to the one we calculated based on the values received: */
  if (localCrc == crcSingle(localBuffer)) {
    Serial.println(248);
    /* just generate a random number to be sent and to create a crc value out of it */
    localBuffer = random(0, 255);
    Serial.println(localBuffer);
    Serial.println(crcSingle(localBuffer));
  }
}


/* EEPROM test.
   it is used by the supervisor to check wether the device is working properly */
void function249() {
  Serial.println(249);
  byte  localBuffer = Serial.read();
  byte buff;
  switch (localBuffer) {
    case 0:
      Wire.beginTransmission(addressEEPROM);
      if (Wire.endTransmission() != 0) {
        eepromFaultCheck = 1;
      }
      else {
        eepromFaultCheck = 0;
      }
      Serial.println(eepromFaultCheck);
      Serial.println(crcSingle(eepromFaultCheck));
      break;
    case 1:
      buff = readByteEEPROM(addressEEPROM, controlOptionAddress);
      Serial.println(buff);
      Serial.println(crcSingle(buff));
      break;
    case 2:
      /* As it will take a series of cycles to read from the EEPROM,
         to keep the code neater we are going to stay inside state machine 2,
         but move to its sub-machine number 2 (case=2)*/
      stateMachine2Check = 2;
      break;

  }
}

/* Generic Write switch. It is used to turn a given actuator ON or OFF */
void function250() {
  byte actuatorIndex;
  char pins[]={pump1, heater1, separator};
  actuatorIndex = Serial.read();
  bool actuatorCommand = Serial.read();
  if (SWAP_HIGHS_LOWS){
    digitalWrite(pins[actuatorIndex], actuatorCommand);
    } else{
    digitalWrite(pins[actuatorIndex], !actuatorCommand);
    }
  Serial.println(250);
  /* Send status of gas valve and a crc to confirm we received instruction and acted upon */
  Serial.println(actuatorIndex);
  Serial.println(digitalRead(pins[actuatorIndex]));
  byte locArray[] = {actuatorIndex, digitalRead(pins[actuatorIndex])};
  Serial.println(crcArray(locArray, sizeof(locArray)));
}

/* Serial output function. 238
 */
void function238() {
  Serial.println(238);
  if (hydrolysisPot == 0 && hydrolysisPot != 255 ){
    hydrolysisPot = 2; // it will be changed into 1 in control option6 after writing to eeprom
  }
  Serial.println(238);
  Serial.println(crcSingle(238));
}


/* Serial output function. 239
   It sends the supervisor the value of the current control option */
void swapHighsLows() {
  Serial.println(239);
  Serial.println(SWAP_HIGHS_LOWS);
  Serial.println(crcSingle(SWAP_HIGHS_LOWS));
}

/* Serial output function.
   It sends the supervisor the value of the current control option */
void serialCommOutputC() {
  Serial.println(240);
  Serial.println(controlOptionPot);
  Serial.println(crcSingle(controlOptionPot));
}

/* Serial output function.
   It sends the supervisor the last recorded value of the variables */
void serialCommOutputV() {
  Serial.println(241);

  /* Send values of variables stored in dataArray to the supervisor */
  for (byte count = 0; count < numVariables; count++) {
    Serial.println(dataArrayV[count]);
  }

  /* Send a crc */
  // debug2 = crcArray(dataArrayV, sizeof(dataArrayV));
  Serial.println(crcArray(dataArrayV, sizeof(dataArrayV)));
}

/* Serial output function.
   It sends the supervisor the last recorded value of the state of the actuators */
void serialCommOutputS() {
  Serial.println(242);
  for (byte count = 0; count < numDigitalOutputs; count++) {
    Serial.println(dataArrayS[count]);
  }

  Serial.println(crcArray(dataArrayS, sizeof(dataArrayS)));
}

/* Serial output function.
   It sends the supervisor the value of the current operating mode */
void serialCommOutputM() {
  Serial.println(243);
  Serial.println(modePot);
  Serial.println(crcSingle(modePot));
}

/* Serial output function.
   It sends the supervisor the value of the references read from the EEPROM */
void serialCommOutputR() {
  byte k;
  Serial.println(247);
  for (k = 0; k < sizeof(referencePot); k++) {
    Serial.println(referencePot[k]);
  }
  Serial.println(crcArray(referencePot, sizeof(referencePot)));
}



/* CYCLIC REDUNDANCY CHECK
   Needed to check wether data received from serial communication is correct.
   It creates a byte from a byte */
byte crcSingle(byte a) {
  float operat;
  float b = (float) a;

  /* Just some meaninless math. Some algorith is run in the supervisor */
  operat += (b * 300);
  operat += b;
  operat /= (b + 50);

  return (byte)operat;
}


/* CYCLIC REDUNDANCY CHECK
   Needed to check wether data received from serial communication is correct.
   It creates a byte from an array */
byte crcArray(byte a[], byte sizeA) {
  float b[sizeA];
  int i;
  float operat = 0.0;

  /* Just some meaninless math. Same algorith is run in the supervisor */
  for (i = 0; i < sizeA; i++) {
    b[i] = (a[i]) * ((float)i + 1);
    operat += b[i];
  }

  operat = operat / 100;

  if (operat > 255) {
    operat = (byte) a[2];
    return operat;
  }

  return (byte) operat;
}
