//FUNCTION DATA MATRIX AND MODE ARRAY UPDATE
/* Stores values read from analog sensors and manipulated by their relative functions */
void analogInputs() {
  /* In development use dummy function to read from potentiometers.
     In production use temperature digital sensors, thus use a different machine state */
  if (DEVELOPMENT) {
    dataArrayV[0] = Dummy(tempDigesterTopLeftPin);
    dataArrayV[1] = Dummy(tempDigesterBottomRightPin);
    dataArrayV[2] = Dummy(tempDigesterPadLeftPin);
    dataArrayV[3] = Dummy(tempDigesterPadRightPin);
    dataArrayV[4] = Dummy(tempBufferTankPin);
    dataArrayV[5] = Dummy(HydrolisysTankTopPin);
    dataArrayV[6] = Dummy(HydrolisysTankBottomPin);
    dataArrayV[7] = Dummy(RoomTemperaturePin);
    dataArrayV[8] = Dummy(RoomHumidityPin);
    dummyFlow();
    dataArrayV[9] = byte(flowCalc);
    dataArrayV[10] = byte(totalFlow);
  }
  /* set global variable so next time we enter machine state number 3 we skip analogInputs and we go to digitalOutputs */
  stateMachine3Check = 1;
}


/* Stores values of the status of digital outputs */
void digitalOutputs() {
  dataArrayS[0] = digitalRead(pump1);
  dataArrayS[1] = digitalRead(heater1);
  dataArrayS[2] = digitalRead(separator);
  dataArrayS[3] = digitalRead(pump3);
  dataArrayS[4] = digitalRead(valve3);
  dataArrayS[5] = digitalRead(valve4);
  dataArrayS[6] = digitalRead(heater2);
  dataArrayS[7] = lockoutCheck();

  /* set global variable so next time we enter machine state number 3 we go to analogInputs*/
  stateMachine3Check = 0;
  /* set global variable so next time we enter stateMachine function we go to machine state 1 */
  stateMachineCheck = 1;

}




//DUMMY functions
/* Just a dummy function to translate readings from a 10k potentiometer */
byte Dummy(byte a) {
  float average = 0;
  int tempPot = analogRead(a);
  average += tempPot;
  average *= 90;
  average /= 1023;
  byte b = byte(average);
  return b;
}


byte dummyFlow(){
     if (Dummy(FlowMeterPin)<50) {
      flowCalc = 0.0;
      return totalFlow;
      }
      for (byte k=0; k< sizeof(injModes);k++){
        if (modePot == injModes[k]){
        totalFlow = totalFlow + 0.12;
        injLoaded = injLoaded + 0.12;
      }
        }
      flowCalc = 12.0;
  return totalFlow;
}
