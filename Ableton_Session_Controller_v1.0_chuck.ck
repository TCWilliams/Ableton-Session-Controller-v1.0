SerialIO serial;
10 => int numberSlidePots;
8 => int numberButtons;
16 => int numberSendPots;
string line;
string stringInts[6];
int data[34]; // holds 4 values for 4 sensore

SerialIO.list() @=> string list[];

//prints out the available serial devices
//uses "C-style" printing instead of <<< >>>
for(int i; i<list.cap(); i++){
    chout <= i <= ": " <= list[i] <= IO.newline();
}

//open serial port 2 at 9600 baud, set to output ASCII (not binary)
serial.open(0, SerialIO.B9600, SerialIO.ASCII);

// set up MIDI
// pot midi messages
MidiOut mout;

MidiMsg levelPots[numberSlidePots]; // create array of midi messages for level pots
for(0 => int i; i < numberSlidePots; i++){
    0 => levelPots[i].data3;  // initialise pot midi data to 0
}

MidiMsg sendPots[numberSendPots];
for(0 => int i; i < numberSendPots; i++){
    0 => sendPots[i].data3;  // initialise pot midi data to 0
}

MidiMsg buttons[numberButtons];  // create array of midi messages for buttons
int buttonStates[numberButtons]; // and for button states
for(0 => int i; i < numberButtons; i++){ 
    0 => buttons[i].data3;  // initialise button midi data to 0
    0 => buttonStates[i];   
}

1 => int midiOutPort;    // loopMidi is set on port 1 so send out messages on this port
mout.open(midiOutPort);  // open port

// listen for serial from arduino
fun void serialPoller(){
    while(true){
        serial.onLine() => now; //wait for arrival or serial…
        serial.getLine() => line; //store serial data in string 'line'
        //if it's "null," break out and go back to waiting for serial
        if(line$Object == null) continue;
        0 => stringInts.size;
        //if messages are 6 comma-separated numbers w/ sq. brackets at end,
        //then store the 6 strings in the stringInts array.
        if (RegEx.match("\\[([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+),([0-9]+)\\]", line, stringInts)){
            
            //now loop through and convert from ascii to int…
            //store these new ints into the "data" array
            for(1 => int i; i < stringInts.cap(); i++){
                Std.atoi(stringInts[i]) => data[i-1];
            }
        }
    }
}
// run serial poller
spork ~ serialPoller();
spork ~ midiSender(); // run MIDI sender

while(true){
      0.5::second => now;
}

fun void midiSender(){
    while (true){
        0 => int index;  // index of data array, will be used to access data during iterations of other arrays
        for (0 => int i; i < numberSlidePots; i++){
            176 => levelPots[i].data1;  // CC message
            i => levelPots[i].data2; 
            if (checkPotVal(levelPots[i].data3, data[index]) == 1){
                data[index] => levelPots[i].data3; // assign data from Serial to midi msg value
                mout.send(levelPots[i]);
            }
            index++;
        }
        for (0 => int i; i < numberSendPots; i++){
            177 => sendPots[i].data1;  // CC message
            i + 8 => sendPots[i].data2; 
            if (checkPotVal(sendPots[i].data3, data[index]) == 1){
                data[index] => sendPots[i].data3; // assign data from Serial to midi msg value
                mout.send(sendPots[i]);
            }
            index++;
        }
        for (0 => int i; i < numberButtons ; i++){  // starting at end of slide pot entries to + number of buttons
            178 => buttons[i].data1; 
            i + 24 => buttons[i].data2; 
            if(data[index] != buttonStates[i]){ // button has been pressed           
                if(buttons[i].data3 == 127){ // if currently ON               
                    0 => buttons[i].data3;   // turn OFF
                    data[index] => buttonStates[i]; // update button stae
                }            
                else {              // currently OFF
                    127 => buttons[i].data3; // turn ON
                    data[index] => buttonStates[i];  // update state
                }              
                mout.send(buttons[i]); //send msg                
            }
            index++;
        }      
        
        0.1::second => now;
     }     
}
fun int checkPotVal(int oldVal, int newVal){
    if(newVal != oldVal && newVal != oldVal +1 && newVal != oldVal +2 
            && newVal != oldVal -1 && newVal != oldVal -2)
    return 1;
    else return 0;
}

    

