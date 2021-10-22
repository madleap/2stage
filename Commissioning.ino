//ALL STAGES - heaters
void controlOption1() {
  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(LOW));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(LOW));
}

//GROUPS - STAGE 1 pump1 and heater 1
void controlOption2() {
  digitalWrite(heater1, hl(LOW));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(HIGH));
  timeRecirculationP1 = timeLapseBFunc(timeRecirculationP1, intervalRecirculationP1, pump1, digitalOutputSwitch);  //Turns solar pump ON/OFF
}

//GROUPS - STAGE 1 pump 1
void controlOption3() {
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(HIGH));
  timeRecirculationP1 = timeLapseBFunc(timeRecirculationP1, intervalRecirculationP1, pump1, digitalOutputSwitch);  //Turns solar pump ON/OFF
}

//GROUPS - STAGE 2 pump 3 and heater
void controlOption4() {
  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(LOW));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(LOW));
}

//N/A
void controlOption5() {
}

// //N/A IN USE AS OPERATION CONTROL OPTION
// void controlOption6() {
// }

// //N/A IN USE AS OPERATION CONTROL OPTION
// void controlOption7() {
// }

// //N/A IN USE AS OPERATION CONTROL OPTION
// void controlOption8() {
// }

// //N/A IN USE AS OPERATION CONTROL OPTION
// void controlOption9() {
// }

//Actuator test – pump1
void controlOption10() {
  digitalWrite(pump1, hl(LOW));
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(HIGH));
}

//Actuator test - heater1
void controlOption11() {
  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(LOW));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(HIGH));
}

//Actuator test –
void controlOption12() {
  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(LOW));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(HIGH));
}

//Actuator test –
void controlOption13() {
  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(LOW));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(HIGH));
}

//Actuator test
void controlOption14() {
  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(LOW));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(HIGH));
}

//Actuator test
void controlOption15() {
  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(LOW));
  digitalWrite(heater2, hl(HIGH));
}

//Actuator test
void controlOption16() {
  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(LOW));
}

//N/A
void controlOption17() {
}

//N/A
void controlOption18() {
}

//Actuator test – immersion heater
void controlOption19() {
  // digitalWrite(pumpSolar, hl(HIGH));
  // digitalWrite(pumpPrimary, hl(HIGH));
  // digitalWrite(pumpBoiler, hl(HIGH));
  // digitalWrite(immHeater, hl(LOW));
  // digitalWrite(digesterMixer, hl(HIGH));
  // digitalWrite(gasValve, hl(HIGH));
}

//Actuator test – biogas – valve
void controlOption20() {
  // digitalWrite(pumpSolar, hl(HIGH));
  // digitalWrite(pumpPrimary, hl(HIGH));
  // digitalWrite(pumpBoiler, hl(HIGH));
  // digitalWrite(immHeater, hl(HIGH));
  // digitalWrite(digesterMixer, hl(HIGH));
  // digitalWrite(gasValve, hl(LOW));
}

//Actuator test – digesterMixer relay
void controlOption21() {
  // digitalWrite(pumpSolar, hl(HIGH));
  // digitalWrite(pumpPrimary, hl(HIGH));
  // digitalWrite(pumpBoiler, hl(HIGH));
  // digitalWrite(immHeater, hl(HIGH));
  // digitalWrite(digesterMixer, hl(LOW));
  // digitalWrite(gasValve, hl(HIGH));
}


//System startup - commissioning mode on idle
void controlOption22() {
idle();
}
