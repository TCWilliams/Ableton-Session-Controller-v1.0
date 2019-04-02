  /* Ableton session controller */

const int numberButtons = 8;   // set number of buttons. can be changed, specify pin numbers in arrays below
const int numberFaders = 10; // set number of slide volume level pots      ''      ''

int  faders [numberFaders]  = {A0, A1, A2, A3, A4, A5, A6, A7, A14, A15};  // array of slide pot pins for channel levels. specify analog i/o pins numbers here
int levelOutVals[numberFaders];    // array for mapped pot values - these will be printed to serial

int buttons[numberButtons] = {31, 32, 2, 3, 4, 5, 6, 7}; // array of buttons, specify digital pin numbers here
int buttonRead[numberButtons];   // array of button read variables
int buttonStates[numberButtons]; // array for button states

const int muxIn1 = 8; // analog in pins for multiplexers
const int muxIn2 = 9;

const int mux1s0 = 22;  // set up mux digital pin numbers for select pins
const int mux1s1 = 23;  // mux 1
const int mux1s2 = 25;

const int mux2s0 = 39;  // mux 2
const int mux2s1 = 41;
const int mux2s2 = 43;

// id's for pots in multiplexers - stored in arrays
int mux1Pots[8] = {0, 1, 2, 3, 4, 5, 6, 7};
int mux2Pots[8] = {0, 1, 2, 3, 4, 5, 6, 7};

int r0 = 0;  // variables for reading mux pins with binary values
int r1 = 0;
int r2 = 0;

int bin[] = {000, 1, 10, 11, 100, 101, 110, 111};  // binary values to identify mux pins

void setup() {
  Serial.begin(9600);   // initialize serial communications at 9600 bps:

  pinMode(mux1s0, OUTPUT);
  pinMode(mux1s1, OUTPUT);
  pinMode(mux1s2, OUTPUT);
  pinMode(mux2s0, OUTPUT);
  pinMode(mux2s1, OUTPUT);
  pinMode(mux2s2, OUTPUT);

  for (int i = 0; i < numberButtons; i++) {
    pinMode(buttons[i], INPUT);    // initialise button pins as inputs
    buttonRead[i] = LOW;           // initialise all button read to LOW
    buttonStates[i] = 0;           // initialise all button state to 'off'
  }
  for (int i = 0; i < numberFaders; i++) {
    levelOutVals[i] = 0; // initialise pot out valuse to zero
  }
}

void loop() {
  for (int i = 0; i < numberButtons; i++) {                                       // for all buttons, check for presses and update
    buttonStates[i] = monitorButton(buttons[i], buttonRead[i], buttonStates[i]);  // button state using monitorButton function
  }

  for (int i = 0; i < numberFaders; i++) {                // get mapped pot values from readAndMap function which returns
    levelOutVals[i] = readAndMapPots( faders[i], 0, 127);  // a value mapped to a range specified in the function call
  }

  readMultiplexers();

  printToSerial(); // function to print serial, just to keep this loop looking nice and tidy
}

/*
helper funtion to read buttons and switch button state when pushed
params: button id, buttonRead variable, and current button state before button push
returns: new currentState
*/
int monitorButton(int button, int buttonRead, int currentState) {
  buttonRead = digitalRead(button);
  if (buttonRead == HIGH) {                        // button has been pushed
    if (currentState == HIGH) currentState = LOW;  //  change to other state
    else currentState = HIGH;
    delay(200);                                    // delay to avoid picking up 2nd click (release) on button press
  }
  return currentState;
}
/*
helper function to take care of pot read and mapping
params: pot id, minimum map value, maximum map value
returns: mapped pot value
*/
int readAndMapPots(int pot, int mapMin, int mapMax) {
  int value = analogRead(pot);                            //read pot pin value
  int mappedValue = map(value, 0, 1023, mapMin, mapMax);  // map to specified range
  return mappedValue;                                     // return mapped value
}

/*
function to read multiplexer pins
*/
void readMultiplexers() {
  for (int i = 0; i < 8; i++) { // //loop through each channel, checking for a signal
    int row = bin[i]; // channel i = ith element in the bin array
    r0 = bitRead(row, 0); //bitRead() -> parameter 1 = binary sequence, parameter 2 = which bit to read, starting from the right most bit
    r1 = bitRead(row, 1); //channel 7 = 111, 1 = 2nd bit
    r2 = bitRead(row, 2); // third bit

    digitalWrite(mux1s0, r0); // send the bits to the digital pins
    digitalWrite(mux1s1, r1);
    digitalWrite(mux1s2, r2);
    digitalWrite(mux2s0, r0); // send the bits to the digital pins
    digitalWrite(mux2s1, r1);
    digitalWrite(mux2s2, r2);

    mux1Pots[i] = map(analogRead(muxIn1), 0, 1023, 127, 0);  // map pot vals to midi range, reverse to account for
    mux2Pots[i] = map(analogRead(muxIn2), 0, 1023, 127, 0);  // physical layout on board, store value in mux pot arrays
  }
}

/*
function to print out values to serial, in the format specified by Chuck code
*/
void printToSerial() {
  Serial.print("["); // open print format
  for (int i = 0; i < numberFaders; i++) { // print volume slider values first
    Serial.print(levelOutVals[i]);  // 8 audio channel volumes, 2 return channel volumes
    Serial.print(",");
  }

  for (int i = 0; i < 8; i++) {
    Serial.print(mux1Pots[i]);
    Serial.print(",");
    Serial.print(mux2Pots[i]);
    Serial.print(",");
  }

  for (int i = 0; i < numberButtons; i++) {  // finally print button values
    Serial.print(buttonStates[i]);  // 8 channel on/off,  3 transport
    if (i != numberButtons - 1)  Serial.print(",");  // do not print comma on final iteration, to fit expected format in Chuck
  }
  Serial.println("]");  // close print format
}

