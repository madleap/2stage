void stateMachine() {
  switch (stateMachineCheck) {

    /* We are only going through state number 0 and its sub-states at the beginning to set up variables for proper system performance.
       The state in which the microcontroller will spend most of the time is actually state 1. */
    case 0:
      /* This machine state is further divided in sub-states */
      startUpCheck();
      break;

    /* This state acts like a timer/manager for the other states. The machine will always come back here
       in between excecuting the other state machines. This is mainly a state of idling */
    case 1:
      /* It updates the value of this timer.
         If time is due, it resets timer and excecutes the pointer function (in this case a lambda function that updates the machine state value)*/
      timeStateMachine2 = timeLapseAFunc(timeStateMachine2, intervalStateMachine2, []() {
        stateMachineCheck = 2;
      });
      if (stateMachineCheck == 2) break;

      /* It updates the value of this timer.
         If time is due, it resets timer and excecutes the pointer function (in this case a lambda function that updates the machine state value)*/
      timeStateMachine3 = timeLapseAFunc(timeStateMachine3, intervalStateMachine3, []() {
        stateMachineCheck = 3;
      });
      if (stateMachineCheck == 3) break;

      /* It updates the value of this timer.
         If time is due, it resets timer and excecutes the pointer function (in this case a lambda function that updates the machine state value)*/
      timeStateMachine4 = timeLapseAFunc(timeStateMachine4, intervalStateMachine4, []() {
        stateMachineCheck = 4;
      });
      if (stateMachineCheck == 4) break;

      /* It updates the value of this timer.
         If time is due, it resets timer and excecutes the pointer function (in this case a lambda function that updates the machine state value)*/
      timeStateMachine5 = timeLapseAFunc(timeStateMachine5, intervalStateMachine5, []() {
        stateMachineCheck = 5;
      });
      if (stateMachineCheck == 5) break;

      /* It updates the value of this timer.
         If time is due, it resets timer and excecutes the pointer function (in this case a lambda function that updates the machine state value)*/
      timeStateMachine6 = timeLapseAFunc(timeStateMachine6, intervalStateMachine6, []() {
        stateMachineCheck = 6;
      });
      if (stateMachineCheck == 6) break;

      /* It updates the value of this timer.
         If time is due, it resets timer and excecutes the pointer function (in this case either debugLCD or funcLCD)*/
      if (DEVELOPMENT) {
        timeLCD = timeLapseAFunc(timeLCD, intervalLCD, debugLCD2);
      } else {
        timeLCD = timeLapseAFunc(timeLCD, intervalLCD, funcLCD); //funcLCD
      }

      /* It updates the value of this timer.
         If time is due, it resets timer and excecutes the pointer function (in this case a lambda function that updates the machine state value)*/
      timeStateMachine7 = timeLapseAFunc(timeStateMachine7, intervalStateMachine7, []() {
        stateMachineCheck = 7;
      });
      if (stateMachineCheck == 7) break;

      break;

    /* This state deals with serial communication with the supervisor. */
    case 2:
      /* It is divided in sub-states*/
      serialComm();
      break;

    /* It checks analog inputs and records the status of digital outputs */
    case 3:
      fastIOs();
      break;

    /* Checks digital temperature sensors. */
    case 4:
      /* This machine state is further divided in sub-states */
      digitalTemperatureSensor();
      break;
    case 5:
      flowMeter();
      // stateMachineCheck = 1;
      break;
    /* Control flow tree.
       According to the current control option, the status of the lockout and any ongoing falut,
       it will decide what operating mode (machine state) the system needs to enter */
    case 6:
      decideControlOption();
      break;
    case 7:
      humidityTemperature();
      break;
  }
}


//FUNCTION: STATE MACHINE 0
void startUpCheck() {
  byte buff;
  switch (stateMachine0Check) {
    /* Establish communication with EEPROM and retrieve contol option*/
    case 0:
      controlOptionPot = readByteEEPROM(addressEEPROM, controlOptionAddress);
      if (controlOptionPot == 255) {
        /* If the value received seems to be normal, adopt these values as control option.
          For normal we mean not equal to 255, which is a value for an empty/damaged EEPROM address */
        /* If the value is corrupted we default to control option 22 which is a safe IDLE state */
        controlOptionPot = 22;
      }
      totalFlow = readByteEEPROM(addressEEPROM, totalFlowAddress);
      if (totalFlow == 255) {
        controlOptionPot = 22;
      }
      loadingsDone = readByteEEPROM(addressEEPROM, loadingsDoneAddress);
      if (loadingsDone == 255) {
        controlOptionPot = 22;
      }
      lastDayRecorded = readByteEEPROM(addressEEPROM, lastDayRecordedAddress);
      if (lastDayRecorded == 255) {
        controlOptionPot = 22;
      }

      /* Initialize next pot to current unix time if RTC is working */
      oldUnixRecirculation = getTime();

      hydrolysisPot = readByteEEPROM(addressEEPROM, hydrolysisPotAddress);
      if (lastDayRecorded == 255) {
        controlOptionPot = 22;
      }
      oldUnixHydrolysis = readUnsignedIntoEEPROM( addressEEPROM, hydrolysisTimerAddress);
      delay(5);

      if (ZERO_OUT_INJ_VALUES) {
        loadingsDone = 0;
        totalFlow = 0;
      }


      /* set the sub-state value so in the next cycle we enter the next sub-state instead of the current one*/
      stateMachine0Check = 1;
      break;

    /* In the following sub-states we want to retreive the reference values stored in the EEPROM.
       Because it takes time to retrieve each one of them, we want the microcontroller to carry on and be able to perform other activities in the meanwhile:
       for instance, manage interrupts: NOTE at this stage we are in state 0 so the microcontroller isn't reading sensor values or communicating with the
       supervisor via serial communication.
       Also note that, because the values to retrieve are many, it will take several machine cycles to obtain them all.
       We will keep returning to this state and move back and forth rom sub-state 1 to sub-state 3 until completion */

    /* In this sub-state we check if we are done collecting data*/
    case 1:
      /* The counter has been initialized to zero, and incremented by 1 each time we succesfully recovered a value from the EEPROM.
         If the value of the counter is smaller then the number of variables we want to retrieve, it means there are still values to be retrieved */
      if (stateMachine0Counter < sizeof(referencePot)) {
        /* We are using EEPROM addresses from 0 to lenght of reference variable array.
           In case any address in this range gets damaged, this will need to rearranged into something else (for example initialize the counter from
           an address above the damaged one. PLEASE watchout that these new addresses are not already being used */
        eeaddress = stateMachine0Counter;
        /* set the sub-state so in the next cycle we enter the next sub-state instead of the current one */
        stateMachine0Check = 2;
      }
      else {
        /* set the sub-state so in the next cycle we jump to sub-state 4 instead of the current one */
        stateMachine0Check = 4;
      }
      break;

    /* In this sub-state we check if enough time has passed for the EEPROM to collect data and send it back to us.
       The first time we enter this sub-state we haven't actually asked the EEPROM any data, this is a sacrificial one, we just wait for nothing! */
    case 2:
      timeEEPROM = timeLapseAFunc(timeEEPROM, intervalEEPROM, []() {
        /* Enough time has passed so we set the sub-state to 3. Thus in the next cycle we enter the next sub-state instead of the current one*/
        stateMachine0Check = 3;
      });
      break;

    /* Here we ask the EEPROM for data from a specific address, where the reference value we want is stored */
    case 3:
      I2CBuffer[stateMachine0Counter] = readByteEEPROM(addressEEPROM, eeaddress);
      stateMachine0Counter++;
      stateMachine0Check = 1;
      break;

    /* in this sub-state we check the data and save it into the microcontroller's dynamic memory */
    case 4:
      /* If values in the buffer seem to be normal, adopt these values as reference values.
         For normal we mean not equal to 255, which is a value for an empty/damaged EEPROM address*/
      for (byte i = 0; i < sizeof(referencePot); i++) {
        if (I2CBuffer[i] != 255) {
          referencePot[i] = I2CBuffer[i];
        }
        else {
          /* If you find any value to be equal to 255, referencepot already has a preprogrammed value */
          eepromFaultCheck = 1;
        }
      }
      /* set the sub-state so in the next cycle we enter the next sub-state instead of the current */
      stateMachine0Check = 5;
      break;

    /* In this final sub-state of machine state 0, we will initialize the variables related with sensor
       values to some fake values as we still haven't read any sensor value yet! */
    case 5:
      /* Set machine state variable to 1 so next time we enter stateMachine function we go to machine state 1 */
      stateMachineCheck = 1;

      /* Set machine state variables to zero in case we restart the board and something goes wrong.
         This should be totally unnecessary (MODIFY! check and remove from code). */
      stateMachine0Check = 0;
      stateMachine0Counter = 0;

      /* Assign fake values to variables so the microcontroller won't assume there's any ongoing fault (i.d. voltage = 255, pressure = 255) */
      dataArrayV[0] = 60;
      dataArrayV[1] = 0;
      dataArrayV[2] = 0;
      dataArrayV[3] = 0;
      dataArrayV[4] = 90;
      dataArrayV[5] = 0;
      dataArrayV[6] = 0;
      dataArrayV[7] = 0;
      dataArrayV[8] = 0;
      dataArrayV[9] = 0;
      dataArrayV[10] = 0;
      break;
  }
}

//FUNCTION: STATE MACHINE 2
void serialComm() {
  /* WATCHOUT, this state has its own sub-states: sub-state 0, sub-state 1, sub-state 2. Moreover, sub-states 1 and 2 are further sub-divided into sub-states.
     This is needed to avoid using serial communication in other functions while doing EEPROM operations.
     In fact, as we have already seen in state machine 0, EEPROM operations take time to be executed:
      - we don't want to hold the entire microcontroller up while we are waiting data;
      - we don't want to use serial communication in other functions before we have completed the previous query, otherwise data from different
        serial functions will be mixed in the buffer making communication meaningless */
  switch (stateMachine2Check) {

    /* This sub-state is where all the serial functions live, with exception of EEPROM read/write functions related to reference values.
       These functions are coded partly in this state, and partly in sub-states 1 and 2 */
    case 0:
      /* This is where we pick up the commands sent by the supervisor */
      assessPot = Serial.read();

      /* Assess what we received */
      switch (assessPot) {
        case 238:
          function238();
          break;
        case 239:
          swapHighsLows();
          break;
        /* Serial output function. It sends the supervisor the value of the current control option */
        case 240:
          serialCommOutputC();
          break;

        /* Serial output function. It sends the supervisor the last recorded value of the variables */
        case 241:
          serialCommOutputV();
          break;

        /* Serial output function. It sends the supervisor the last recorded value of the state of the actuators */
        case 242:
          serialCommOutputS();
          break;

        /* Serial output function. It sends the supervisor the value of the current operating mode */
        case 243:
          serialCommOutputM();
          break;

        /* It writes reference values into EEPROM */
        case 244:
          /* the other half of the code is in a dedicated sub-state of state 2: case 1 */
          function244();
          break;

        /* Write Gas valve switch. It is used to turn the gas hob ON or OFF */
        case 245:
          function245();
          break;

        /* It writes the control option. It is used to select a control option from the supervisor */
        case 246:
          function246();
          break;

        /* Serial output function. It sends the supervisor the value of the references read from the EEPROM */
        case 247:
          serialCommOutputR();
          break;

        /* serial comm test -- ping */
        case 248:
          function248();
          break;

        /* EEPROM test. It is used by the supervisor to check wether the device is working properly */
        case 249:
          function249();
          break;

        /* Generic Write switch */
        case 250:
          function250();
          break;

        /* We enter this state when the serial buffer is empty */
        case 255:
          /* Set machine state variable to 1 so next time we enter stateMachine function we go to state 1 instead of the current state*/
          stateMachineCheck = 1;
          break;
      }
      break;

    /* dedicated sub-state for writing reference values into the EEPROM.
       For more info on each individual command, check machine state number 0*/
    case 1:
      switch (stateMachine2Sub1Check) {
        case 0:
          if (stateMachine2Sub1Counter < sizeof(referencePot)) {
            eeaddress = stateMachine2Sub1Counter;
            timeEEPROM = millis();
            stateMachine2Sub1Check = 1;
          }
          else {
            stateMachine2Sub1Check = 3;
          }
          break;
        case 1:
          timeEEPROM = timeLapseAFunc(timeEEPROM, intervalEEPROM, []() {
            stateMachine2Sub1Check = 2;
          });
          break;
        case 2:
          writeByteEEPROM( addressEEPROM, eeaddress, I2CBuffer[stateMachine2Sub1Counter] );
          stateMachine2Sub1Counter++;
          stateMachine2Sub1Check = 0;
          break;
        case 3:
          for (byte i = 0; i < sizeof(referencePot); i++) {
            referencePot[i] = I2CBuffer[i];
          }
          Serial.println(244);
          byte  localBuffer = random(0, 255);
          Serial.println(localBuffer);
          Serial.println(crcSingle(localBuffer));
          stateMachine2Check = 0;
          stateMachineCheck = 1;
          stateMachine2Sub1Check = 0;
          stateMachine2Sub1Counter = 0;
          break;
      }
      break;

    /* dedicated sub-state for reading reference values from the EEPROM.
       For more info on each individual command, check machine state number 0*/
    case 2:
      switch (stateMachine2Sub2Check) {
        case 0:
          if (stateMachine2Sub2Counter < sizeof(referencePot)) {
            eeaddress = stateMachine2Sub2Counter;
            stateMachine2Sub2Check = 1;
          }
          else {
            stateMachine2Sub2Check = 3;
          }
          break;
        case 1:
          timeEEPROM = timeLapseAFunc(timeEEPROM, intervalEEPROM, []() {
            stateMachine2Sub2Check = 2;
          });
          stateMachineCheck = 1;

          break;
        case 2:
          I2CBuffer[stateMachine2Sub2Counter] = readByteEEPROM(addressEEPROM, eeaddress);
          stateMachine2Sub2Counter++;
          stateMachine2Sub2Check = 0;
          break;
        case 3:
          for (byte i = 0; i < sizeof(referencePot); i++) {
            Serial.println(I2CBuffer[i]);
          }
          Serial.println(crcArray(I2CBuffer, sizeof(I2CBuffer)));
          stateMachine2Sub2Counter = 0;
          stateMachine2Sub2Check = 0;
          stateMachine2Check = 0;
          stateMachineCheck = 1;
          break;
      }
  }
}

//FUNCTION: STATE MACHINE 3
void fastIOs() {
  switch (stateMachine3Check) {
    case 0:
      analogInputs();
      break;
    case 1:
      digitalOutputs();
      break;
  }
}

//FUNCTION: STATE MACHINE 4
void digitalTemperatureSensor() {
  switch (stateMachine4Check) {
    case 0:
      if (DEVELOPMENT) {
      } else {
        sensorsA.requestTemperatures(); // Send the command to get temperatures
        sensorsB.requestTemperatures(); // Send the command to get temperatures
        sensorsC.requestTemperatures(); // Send the command to get temperatures
        sensorsD.requestTemperatures(); // Send the command to get temperatures
        sensorsE.requestTemperatures(); // Send the command to get temperatures
        sensorsF.requestTemperatures(); // Send the command to get temperatures
        sensorsH.requestTemperatures(); // Send the command to get temperatures
      }
      timeStateMachine4 = millis();
      stateMachine4Check = 1;
    case 1:
      timeDallasTemp = timeLapseAFunc(timeDallasTemp, intervalDallasTemp, []() {
        stateMachine4Check = 2;
      });
      /* needed for multitasking as case 2 and case 4 are very long waits and arduino would be doing nothing otherwise */
      stateMachineCheck = 1;
      break;
    case 2:
      if (DEVELOPMENT) {
      } else {
        //        dataArrayV[0] = (byte) errorsTempSensor((byte) sensorsA.getTempCByIndex(0));
        //        dataArrayV[1] = (byte) errorsTempSensor((byte) sensorsB.getTempCByIndex(0));
        //        dataArrayV[2] = (byte) errorsTempSensor((byte) sensorsC.getTempCByIndex(0));
        //        dataArrayV[3] = (byte) errorsTempSensor((byte) sensorsD.getTempCByIndex(0));
        //        dataArrayV[4] = (byte) errorsTempSensor((byte) sensorsE.getTempCByIndex(0));
        //        dataArrayV[5] = (byte) errorsTempSensor((byte) sensorsF.getTempCByIndex(0));
        //        dataArrayV[6] = (byte) errorsTempSensor((byte) sensorsH.getTempCByIndex(0));

        dataArrayV[0] = (byte) filterTempSensor((byte) sensorsA.getTempCByIndex(0), dataArrayV[0]);
        dataArrayV[1] = (byte) filterTempSensor((byte) sensorsB.getTempCByIndex(0), dataArrayV[1]);
        dataArrayV[2] = (byte) filterTempSensor((byte) sensorsC.getTempCByIndex(0), dataArrayV[2]);
        dataArrayV[3] = (byte) filterTempSensor((byte) sensorsD.getTempCByIndex(0), dataArrayV[3]);
        dataArrayV[4] = (byte) filterTempSensor((byte) sensorsE.getTempCByIndex(0), dataArrayV[4]);
        dataArrayV[5] = (byte) filterTempSensor((byte) sensorsF.getTempCByIndex(0), dataArrayV[5]);
        dataArrayV[6] = (byte) filterTempSensor((byte) sensorsH.getTempCByIndex(0), dataArrayV[6]);

      }

      stateMachineCheck = 1;
      stateMachine4Check = 0;
  }
}

//FILTER FOR DALLAS TEMPERATURE SENSOR
/* If an error occurs the digital sensor throws an error. Most common errors are nr 85 and nr 129.
   Unfortunately this is a poor choice of values, especially the value 85, which falls into the operating
   range of the sensor. In order to avoid false reading triggering meaningless faults, we exclude these numbers
   by-passing them with the previous value recorded (the second argument byte b) */
float filterTempSensor(byte a, byte b) {
  if ( a != 85 && a != 129 ) {
    return a;
  }
  else {
    return b;
  }
}

float errorsTempSensor(byte a) {
  switch (a) {
    case 85:
      return 240;
    case 129:
      return 250;
  }
  return a;
}


//FUNCTION: STATE MACHINE 5
void flowMeter() {
  switch (stateMachine5Check) {
    case 0:
      if (DEVELOPMENT) {
        stateMachineCheck = 1; // use potentiometer instead
      } else {
        timeStateMachine5 = millis();
        FlowPulse = 0;      //Set NbTops to 0 ready for calculations
        stateMachine5Check = 1;
      }
      break;
    case 1:
      timeFlowmeter  = timeLapseAFunc(timeFlowmeter, intervalFlowmeter, []() {
        stateMachine5Check = 2;
      });
      /* needed for multitasking as case 2 and case 4 are very long waits and arduino would be doing nothing otherwise */
      stateMachineCheck = 1;
      break;
    case 2:
      flowsensorfix[++flowsensorfixIndex % 20] = FlowPulse;
      long sum = 0L ;  // sum will be larger than an item, long for safety.
      float avg = 0.0;
      for (int i = 0 ; i < 20 ; i++) {
        sum += flowsensorfix[i];
      }
      avg = (sum / 20);  // average.

      for (int i = 0 ; i < 20 ; i++) {
        if ((avg * 1.7) < flowsensorfix[i]) {
          break;
        }
      }

      flowCalc = (float(FlowPulse) * 2.25); //2.25 ml per pulse
      flowCalc = flowCalc / 10; //ml to dl
      flowCalc = flowCalc * (100 / intervalFlowmeter); //convert interval reading to dl/s

      for (byte k = 0; k < sizeof(injModes); k++) {
        // if we are in one of the modes dealing with injections, we want to add the currently injected liquid to total
        if (modePot == injModes[k]) {
          totalFlow = totalFlow + flowCalc / 100; // convert flowCalc from dl to litres
          injLoaded = injLoaded + flowCalc / 100;
        }
      }

      dataArrayV[9] = (byte)flowCalc;
      dataArrayV[10] = (byte)totalFlow;
      stateMachineCheck = 1;
      stateMachine5Check = 0;
  }
}

//FUNCTION: STATE MACHINE 6
void decideControlOption() {

  /* We set what state we want to enter in the next cycle here */
  stateMachineCheck = 1;

  switch (controlOptionPot) {

    /* Control options 0 to 4 represent the control flow trees for several combinations of renewable energy technologies.
       No matter the path, they all lead to state machines called modes */
    case 0: controlOption0();
      break;
    case 1: controlOption1();
      break;
    case 2: controlOption2();
      break;
    case 3: controlOption3();
      break;
    case 4: controlOption4();
      break;

    /* Venting tests: these are simple functions turning sets of actuators ON and OFF, all managed by a local timer function */
    case 5: controlOption5();
      break;
    case 6: controlOption6();
      break;
    case 7: controlOption7();
      break;
    case 8: controlOption8();
      break;
    case 9: controlOption9();
      break;

    /* Actuator tests: these are simple functions turning only an actuator ON and OFF per time*/
    case 10: controlOption10();
      break;
    case 11: controlOption11();
      break;
    case 12: controlOption12();
      break;
    case 13: controlOption13();
      break;

    /* Flaring options (manually burn gas during testing and commissioning)*/
    case 14: controlOption14();
      break;
    case 15: controlOption15();
      break;

    case 16: controlOption16();
      break;
    case 17: controlOption17();
      break;
    case 18: controlOption18();
      break;

    /* Actuator tests: these are simple functions turning only an actuator ON and OFF per time*/
    case 19: controlOption19();
      break;
    case 20: controlOption20();
      break;
    case 21: controlOption21();
      break;

    /* A simple function that turns all actuators OFF.
       It is used in commissioning mode on idle and to terminate venting and actuators functions*/
    case 22: controlOption22();
      break;
  }
}

//FUNCTION: STATE MACHINE 7
void humidityTemperature() {
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(temp) || isnan(humidity)) {
  } else {
    dataArrayV[7] = byte(temp);
    dataArrayV[8] = byte(humidity);
  }
  stateMachineCheck = 1;
}
