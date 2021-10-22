/*
nothing new
 */


////////////////////////////////////////// INCLUDES /////////////////////////////////////////////


#define SKETCH_VERSION "V4.1"

/* Libraries needed to use the dallas digital temperature sensor */
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

/*Library for humidity and temperature sensor AM2301*/
#include "DHT.h"

/*Library for RTC clock DS 1307 I2C RTC*/
#include "RTClib.h"
RTC_DS1307 RTC;

/* Library needed to use the I2C LCD screen */
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


/* Define digital pin for AM2301 */
#define DHTPIN 53
#define DHTTYPE DHT21   // AM2301
DHT dht(DHTPIN, DHTTYPE);

/* Define digital pins used for the one wire bus.
   Although each bus could be used by several sensors, we are using
   one sensor for each bus */
#define ONE_WIRE_BUSA 32 //0 digester top left
#define ONE_WIRE_BUSB 31 //1 digester bottom right
#define ONE_WIRE_BUSC 50 //2 digester pad left
#define ONE_WIRE_BUSD 26 //3 digester pad right
#define ONE_WIRE_BUSE 45 //4 tank T2
#define ONE_WIRE_BUSF 38 //5 tank T1 top
#define ONE_WIRE_BUSH 49 //5 tank T1 bottom
OneWire oneWireA(ONE_WIRE_BUSA);
OneWire oneWireB(ONE_WIRE_BUSB);
OneWire oneWireC(ONE_WIRE_BUSC);
OneWire oneWireD(ONE_WIRE_BUSD);
OneWire oneWireE(ONE_WIRE_BUSE);
OneWire oneWireF(ONE_WIRE_BUSF);
OneWire oneWireH(ONE_WIRE_BUSH);
DallasTemperature sensorsA(&oneWireA);
DallasTemperature sensorsB(&oneWireB);
DallasTemperature sensorsC(&oneWireC);
DallasTemperature sensorsD(&oneWireD);
DallasTemperature sensorsE(&oneWireE);
DallasTemperature sensorsF(&oneWireF);
DallasTemperature sensorsH(&oneWireH);


////////////////////////////////////////// VARIABLES /////////////////////////////////////////////



// DECLARE AND INITIALIZE VARIABLES FOR DEBUGGING PURPOSES
byte debug = 0;
byte debug2 = 0;
float debug3 = 0;
bool DEVELOPMENT = LOW;
bool SWAP_HIGHS_LOWS = 0; // use 1 with LEDs simulating relays in development
#define ACTUATOR_CHANNELS OUTPUT
bool ZERO_OUT_INJ_VALUES = 0; // instead of reading values saved in eeprom, override with zeros

// DECLARE AND INITIALIZE DIGITAL PINS
const byte pump1 = 17;
const byte heater1 = 2;
const byte separator = 3;
const byte pump3 = 4;
const byte valve3 = 5;
const byte valve4 = 6;
const byte heater2 = 7;
const byte lowLevelT2 = 13;


int flowsensor = 18;    //input


// DECLARE AND INITIALIZE ANALOG PINS - use potentiometers in development instead of digital sensors
const byte tempDigesterTopLeftPin = 1; // [°C]
const byte tempDigesterBottomRightPin = 2; // [°C]
const byte tempDigesterPadLeftPin = 3; // [°C]
const byte tempDigesterPadRightPin = 4;
const byte tempBufferTankPin = 5; // [°C]
const byte HydrolisysTankTopPin = 6; // [°C]
const byte HydrolisysTankBottomPin = 7; // [°C]
const byte RoomTemperaturePin = 8; // [°C]
const byte RoomHumidityPin = 9; // [%]
const byte FlowMeterPin = 10; // [dl/s]



// DECLARE AND INITIALIZE CONSTANTS. THE VALUE OF THESE VARIABLES WILL NEVER CHANGE AT RUNTIME

/* The next variable defines the number of readings needed to produce an average sensor value */
const int numSamples = 5;
/* The next variable defines the number of control variables, it's the lenght of the dataArray */
const byte numVariables = 11;
/*  The next variable defines the number of digital outputs plus other status variables (CHECK MANUAL) */
const byte numDigitalOutputs = 8;
/*  The next variable defines the number of control options available */
const byte numControlOptions = 22;
/* The next variable holde the I2C address of the EEPROM device */
int addressEEPROM = 0x50;
/* The next variable defines the address where the control option will be stored in the EEPROM.
   Choose an address which doesn't fall withing the range of the addresses allocated for reference values */
unsigned int controlOptionAddress = 3000;
unsigned int loadingsDoneAddress = 3001;
unsigned int totalFlowAddress = 3002;
unsigned int lastDayRecordedAddress = 3003;
unsigned int hydrolysisPotAddress = 2000;
unsigned int hydrolysisTimerAddress = 2001; // reserve next 1 as it's a unsigned int



const byte ACTUATORS_IN_MODES = 5;
const char actuatorsInModes[ACTUATORS_IN_MODES]={heater1,pump3,valve3,valve4,heater2};
const byte GROUP_SIZE = 10;  // IMPORTANT, if you add more modes per group change this (note, they MUST have the same size)

const bool m0[] = {0,0,0,0,0};
const bool m1[] = {0,1,0,0,0};
const bool m2[] = {0,1,0,0,1};
const bool m3[] = {0,0,0,0,1};
const bool m4[] = {0,0,1,0,0};
const bool m5[] = {0,0,1,0,1};
const bool m6[] = {0,1,1,0,1};
const bool m7[] = {0,1,1,0,0};
const bool m8[] = {0,0,0,1,0};
const bool m9[] = {0,0,0,1,1};
const bool* automaticA[GROUP_SIZE]={m0,m1,m2,m3,m4,m5,m6,m7,m8,m9};
byte modeBlockA[]={0,1,2,3,4,5,6,7,8,9};
bool modeStatusBlockA[GROUP_SIZE][ACTUATORS_IN_MODES];

const bool m10[] = {1,0,0,0,0};
const bool m11[] = {1,1,0,0,0};
const bool m12[] = {1,1,0,0,1};
const bool m13[] = {1,0,0,0,1};
const bool m14[] = {1,0,1,0,0};
const bool m15[] = {1,0,1,0,1};
const bool m16[] = {1,1,1,0,1};
const bool m17[] = {1,1,1,0,0};
const bool m18[] = {1,0,0,1,0};
const bool m19[] = {1,0,0,1,1};
bool* automaticB[GROUP_SIZE]={m10,m11,m12,m13,m14,m15,m16,m17,m18,m19};
byte modeBlockB[]={10,11,12,13,14,15,16,17,18,19};
bool modeStatusBlockB[GROUP_SIZE][ACTUATORS_IN_MODES];

/* Needed for lockout breaker - safety and functioning
Note we can't just read digital pins because we have also semi automatic and manual control options
and it is easier to have everything here instead of having to remember which control option is automatic.
*/
byte digesterHeaterModes[] = {2,3,5,6,9,12,13,15,16,19};
byte hydrolysisHeaterModes[] = {10,11,12,13,14,15,16,17,18,19};
byte injModes[] = {6,7,16,17}; // also needed to know when totalFlow has to be increased by flowCount



// DECLARE AND INITIALIZE TIMERS, THESE MUST BE UNSIGNED LONG TO PERFORM DURING CLOCK ROLLOVER, USED FOR TIMELAPSE FUNCTION

/* It is used in the state that deals with serial */
unsigned long const int intervalStateMachine2 = 20; //20
unsigned long int timeStateMachine2 = 0;

/* It is used in the state that deals with analog input */
unsigned long const int intervalStateMachine3 = 500;
unsigned long int timeStateMachine3 = 0;

/* It is used in the state that deals with digital temp sensors */
unsigned long const int intervalStateMachine4 = 1400;
unsigned long int timeStateMachine4 = 0;

/* It is used in the state that deals with digital gas level sensor */
unsigned long const int intervalStateMachine5 = 100;
unsigned long int timeStateMachine5 = 0;

/* It is used in the state that deals with control flow related decision */
unsigned long const int intervalStateMachine6 = 50;
unsigned long int timeStateMachine6 = 0;

/* It is used in the state that deals with control flow related decision */
unsigned long const int intervalStateMachine7 = 1000;
unsigned long int timeStateMachine7 = 0;


/* if intervalStateMachine4 > intervalDallasTemp, it will only take two loop cycles to execute state machine 4 */
unsigned long const int intervalDallasTemp = 1200;
unsigned long int timeDallasTemp = 0;

/* if intervalStateMachine5 > intervalMeter, it will only take two loop cycles to execute state machine 5 */
unsigned long const int intervalFlowmeter = 50;
unsigned long int timeFlowmeter = 0;


/* It is used in any sub-state that deals with EEPROM's reading/writing operations of blocks of variables */
unsigned long const int intervalEEPROM = 5;
unsigned long int timeEEPROM = 0;
unsigned long int intervalLockout = 300000;
unsigned long int timeLockout = 0;

/* needed to control macerator pump for operations and commissioning*/
unsigned long const int intervalRecirculationP1 = 30000; // for commissioning
unsigned long int timeRecirculationP1 = 0; // for commissioning
// unsigned long const int intervalMixer = 3000; // time between start and stop
// unsigned long int timeMixer = 0;
// unsigned long const int intervalMixerStart = 3000;
unsigned long int timeMixerStart = 0;
// unsigned long const int intervalMixerStop = 3000;
unsigned long int timeMixerStop = 0;

unsigned long const int intervalLCD = 1000;
unsigned long int timeLCD = 0;

/*  we save hrs in bytes and we convert it to seconds */
long int timeMultiplierRecirculation = 3600;
/*  we save hrs in bytes and we convert it to seconds */
long int timeMultiplierHydrolysis = 3600;
/*  we save minutes in bytes and we convert it to milliseconds */
long int timeMultiplierMixer = 60000;



// DECLARE AND INITIALIZE VARIABLES CALLED "POTS", THESE VARIABLES ARE USED FOR CONVENIENCE AND ACT AS TEMPORARY STORAGE

/*Needed for RCT */
long int oldUnixRecirculation = 0;
byte lastDayRecorded = 255;
long int oldUnixHydrolysis = 0;
byte hydrolysisPot = 0; // make it byte because eeprom throws a 255 if address is broken or empty

/*Needed for flow meter */
volatile int FlowPulse; //measuring the rising edges of the signal
float flowCalc;
float injLoaded = 0; // quantity of liquid loaded during injection, needed to stop pump at every injection
float totalFlow = 0; // total flow injected in a day
byte loadingsDone = 255;
bool injFlag = 1; // Set to 1 if you want to start with injection

byte eepromFaultCheck = 0;
byte gasValvePot = 0;
byte controlOptionPot;
volatile byte previousControlOptionPot;

/*  The next variable holds the current operating mode.
    Needed to let SCADA know what background image to load according to what mode of the control option is being executed */
volatile byte modePot = 0;

/*  The next variable holds the operating mode the system will switch to after lockout is unlocked.
    Needed for lockoutCheck function to compare actual mode with current mode and determine the state locked/unlocked */
volatile byte nextModePot = 0;

/*  The next variable holds the status of the lockout.
    Needed in those states called "modes" */
byte lockoutPot = 0;

/* The next variable let's us reset the lockout timer in case of an emergency,
   so that we don't need to wait the timer to go off to take actions */
bool resetLockoutPotA = 0;

/* The next variable let's us reset the lockout timer in case of an emergency,
   so that we don't need to wait the timer to go off to take actions */
bool resetLockoutPotB = 0;
bool mixerStartCheck = 0;

/* Next variable is needed to activate mixer function, it is set to 1 every intervallMixerStart */
byte mixerFuncSwitch = 1;

/* Next variable is needed to know status pump 1 - hacky but easier than working out HIGH LOW swaps in digital read  */
bool pump1Status = 0;

/* Next variable is needed for serial communication.
   It will hold the byte number identifying the serial function that the supervisor wants us to execute */
byte assessPot;

/* The next is an array holding readings from the gas level sensor */
byte distance[numSamples];

/* Next, initialize and declare references for sensor values.
   This is needed as backup in case I2C line is faulty or when starting the microcontroller for the first time ever (eeprom will be empty)*/
byte referencePot[9] = {55, 7, 2, 8, 37, 50, 5, 0, 2};
/* This array will hold the values of the reference variables retrieved from the EEPROM */
byte I2CBuffer[17];

/* The next variable is used to temporary store the address of the specific variable we want to retrieve from the EEPROPM */
unsigned int eeaddress;

/* The next variable is used in data collection function and serial output function*/
volatile byte dataArrayV[numVariables];

/* The next variable is used in data collection function and serial output function */
volatile byte dataArrayS[numDigitalOutputs];

/* Following, a series of declaration and initializations of variables needed to guide us through the different machine states */
/* Main machine state  */
byte stateMachineCheck = 0;

/* Startup sub-state*/
byte stateMachine0Check = 0;
byte stateMachine0Counter = 0;

/* Serial communication sub-state */
byte stateMachine2Check = 0;
byte stateMachine2Sub1Check = 0;
byte stateMachine2Sub1Counter = 0;
byte stateMachine2Sub2Check = 0;
byte stateMachine2Sub2Counter = 0;

/* Fast I/Os sub-state*/
byte stateMachine3Check = 0;

/* Digital temperature sensor sub-state */
byte stateMachine4Check = 0;
byte stateMachine4Counter = 0;

/* Digital gas level sensor sub-state */
byte stateMachine5Check = 0;
byte stateMachine5Counter = 0;


// DECLARE FUNCTION POINTERS AND WRITE THEIR RELATED PROTOTYPE LINES.
/* For more info on function pointers visit http://www.gammon.com.au/callbacks */

/* Next we are declaring a callback function type, this one takes no arguments and returns nothing */
typedef void (*timeLapseA)();

/* Prototype line for timeLapseFunc, which has a callback argument */
unsigned long int timeLapseAFunc(long int actualTime, long int timeReference, timeLapseA functionPointer);

/* Next we are declaring a callback function type, this one takes one argument and returns nothing */
typedef void(*timeLapseB)(byte a);

/* Prototype line for timeLapseFuncPin function, which has a callback argument */
unsigned long int timeLapseBFunc(long int actualTime, long int timeReference, byte pin, timeLapseB functionPointer);


float flowsensorfix[20]={0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0};
int flowsensorfixIndex=0;



////////////////////////////////////////// CORE /////////////////////////////////////////////


void setup() {

  if (DEVELOPMENT) {
    /*Speed up things for testing purposes in development*/
    intervalLockout = 10000; //millisecs
    timeMultiplierRecirculation = 40; //secs
    timeMultiplierHydrolysis = 30; //secs
    timeMultiplierMixer = 1000; //millisecs
  }

  /* Configure the specified pin to behave either as an input or an output.*/
  pinMode(pump1, ACTUATOR_CHANNELS);
  pinMode(heater1, ACTUATOR_CHANNELS);
  pinMode(separator, ACTUATOR_CHANNELS);
  pinMode(pump3, ACTUATOR_CHANNELS);
  pinMode(valve3, ACTUATOR_CHANNELS);
  pinMode(valve4, ACTUATOR_CHANNELS);
  pinMode(heater2, ACTUATOR_CHANNELS);

  pinMode(flowsensor, INPUT); //initializes digital pin 2 as an input
  
  attachInterrupt(digitalPinToInterrupt(flowsensor), rpm, RISING); //and the interrupt is attached
  sei();            //Enables interrupts

  pinMode(lowLevelT2, INPUT);

  /* begin serial communication, configure baudrate */
  Serial.begin(115200);

  /* Initialize the lcd for 20 chars 4 lines, and turn on backlight */
  lcd.begin(20, 4);
  lcd.backlight();

  /* Start communication on one wire busses*/
  sensorsA.begin();
  sensorsB.begin();
  sensorsC.begin();
  sensorsD.begin();
  sensorsE.begin();
  sensorsF.begin();

  /* set up non blocking option for each one wire bus */
  sensorsA.setWaitForConversion(false);
  sensorsB.setWaitForConversion(false);
  sensorsC.setWaitForConversion(false);
  sensorsD.setWaitForConversion(false);
  sensorsE.setWaitForConversion(false);
  sensorsF.setWaitForConversion(false);

  /* needed for serial communication test */
  randomSeed(analogRead(0));

  /* needed for RTC */
  RTC.begin(); // load the time from your computer.
  if (! RTC.isrunning())
  {
    /* We will use this to detect if RTC is working or not, used in getTime and getDay
    It throws out the following instead of the real values:
    165/165/2165 165:165:85
    2090164689
    0*/
    RTC.adjust(DateTime(__DATE__, __TIME__));

  }

  /* needed for AM2301 */
  dht.begin();

  digitalWrite(pump1, hl(HIGH));
  digitalWrite(heater1, hl(HIGH));
  digitalWrite(separator, hl(HIGH));
  digitalWrite(pump3, hl(HIGH));
  digitalWrite(valve3, hl(HIGH));
  digitalWrite(valve4, hl(HIGH));
  digitalWrite(heater2, hl(HIGH));

  for (byte k = 0; k < GROUP_SIZE;k++){
    for (byte i = 0; i < sizeof(actuatorsInModes);i++){
      modeStatusBlockA[k][i]=automaticA[k][i];
      modeStatusBlockB[k][i]=automaticB[k][i];
      }
    }

}

void loop() {
  /* We will constantly call this function at every loop cycle.
     The microcontroller will enter one single state per loop cycle, and thanks to the enormous amount of cycles per second,
     this will make it lool like it's multitasking */

 stateMachine();
}
