volatile int rate[10];                    // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI
volatile int P =512;                      // used to find peak in pulse wave, seeded
volatile int T = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 512;                // used to find instant moment of heart beat, seeded
volatile int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;      // used to seed rate array so we startup with reasonable BPM

void interruptSetup(){ 
  // Initializes Timer2 to throw an interrupt every 2mS.
  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER 
  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED      
} 


// THIS IS THE TIMER 2 INTERRUPT SERVICE ROUTINE. 
// Timer 2 makes sure that we take a reading every 2 miliseconds
ISR(TIMER2_COMPA_vect){                         // triggered when Timer2 counts to 124
  cli();    
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

 if (N > 250){    
  if (QS == true){
    IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
    lastBeatTime = sampleCounter;               // keep track of time for next pulse

    if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
      secondBeat = false;                  // clear secondBeat flag
      for(int i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
        rate[i] = IBI;                      
      }
    }

    if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
      firstBeat = false;                   // clear firstBeat flag
      secondBeat = true;                   // set the second beat flag
      sei();                               // enable interrupts again
      return;                              // IBI value is unreliable so discard it
    }  

    // keep a running total of the last 10 IBI values
    word runningTotal = 0;                  // clear the runningTotal variable    

    for(int i=0; i<=8; i++){                // shift data in the rate array
      rate[i] = rate[i+1];                  // and drop the oldest IBI value 
      runningTotal += rate[i];              // add up the 9 oldest IBI values
    }

    rate[9] = IBI;                          // add the latest IBI to the rate array
    runningTotal += rate[9];                // add the latest IBI to runningTotal
    runningTotal /= 10;                     // average the last 10 IBI values 
    BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
    QS = false;                              // set Quantified Self flag 
    // QS FLAG IS NOT CLEARED INSIDE THIS ISR
  }
  }

  if (N > 2500){                           // if 2.5 seconds go by without a beat
    thresh = 512;                          // set thresh default
    P = 512;                               // set P default
    T = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
  }

  sei();                                   // enable interrupts when youre done!
} // end isr
