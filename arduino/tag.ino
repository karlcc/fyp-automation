//tag
#include <SoftwareSerial.h>

// Define which pins you will use on the Arduino to communicate with RF 
#define RXPIN_RF 5
#define TXPIN_RF 4

//baud rate RF module
#define BAUD 9600

#define S1 10

#define BUZ 12

SoftwareSerial rf(RXPIN_RF, TXPIN_RF);


//data init
const char* name= "1";
int temp = 0;   
int bat = 0; 
int rawValue = 0; 
float lat=0;
float lon=0;
float vel=0;
String dates="***********";   
char inByte = 'a';       
boolean sw1=0;
boolean buzzer=0;

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

int ledPin=13;
int sensorPin=2;
static double oldValue=0;
static double oldChange=0;
double alpha=0.48;
int period=20;
double change=0.0;
double delta=1.8;

// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, must be seeded! 
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

//PROTOTYPEs
void establishContact();

//int reading;

void setup()
{
  rf.begin(BAUD);
  //buzzer
  pinMode(BUZ, OUTPUT);  
  digitalWrite(BUZ, LOW);
  // two monitor ports
  pinMode(S1, INPUT);
  pinMode(ledPin,OUTPUT);
  Serial.begin(9600);
  //analogReference(INTERNAL); 
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
}

void loop()
{
  int rawValue=analogRead(sensorPin);
  //Signal=rawValue;
  double value=alpha*oldValue+(1-alpha)*rawValue;
  Signal=value;
  change=value-oldValue;

  if (change<delta&&oldChange>delta){
    QS = true;                      // reset the Quantified Self flag for next time    
  }
  digitalWrite(ledPin,(change<delta&&oldChange>delta));
  sendDataToProcessing('S', Signal);     // send Processing the raw Pulse Sensor data
  sendDataToProcessing('B',BPM);   // send heart rate with a 'B' prefix
  sendDataToProcessing('Q',IBI);   // send time between beats with a 'Q' prefix

  oldValue=value;
  oldChange=change;
  
  // read the pushbutton input pin:
  buttonState = digitalRead(10);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      buttonPushCounter++;
      //Serial.println("on");
      //Serial.print("number of button pushes:  ");
      //Serial.println(buttonPushCounter);
    } 
    else {
      // if the current state is LOW then the button
      // wend from on to off:
      //Serial.println("off"); 
    }
  }
  // save the current state as the last state, 
  //for next time through the loop
  lastButtonState = buttonState;

  
  // turns on the LED every four button pushes by 
  // checking the modulo of the button push counter.
  // the modulo function gives you the remainder of 
  // the division of two numbers:
  if (buttonPushCounter % 2 == 0) {
    name= "1";
    //Serial.println(name);
  } else {
    name= "2";
    //Serial.println(name);
  }


  delay(period);

  rf.listen(); //waiting for data comand from arduino server
  if (rf.available() > 0) {
    inByte = rf.read();
    //Serial.println(inByte); 
    switch (inByte){	
    case '0': // Buzzer OFF
      digitalWrite(BUZ, LOW);
      buzzer=0;
      rf.print('K');
      rf.flush();
      break;
    case '1': // Buzzer ON
      digitalWrite(BUZ, HIGH);
      buzzer=1;	
      rf.print('K');
      rf.flush();
      break;
    case 'b': // Buzzer Status
      //rf.println(buzzer);
      if (buzzer) {
        rf.println("OPEN");
      }
      else {
        rf.println("CLOSED");
      }
      rf.flush();
      break;
    default: // JSON request
      // read analog input:
      //reading = analogRead(A0);
      //temp = reading / 9.31;
      temp = ((5.0 * analogRead(A1) * 100.0) / 1024)-5;
      //Serial.println(temp);
      //rawValue=analogRead(A1);
      //Serial.println(BPM);
      bat = map(analogRead(A1),0,1023,0,50);     
      sw1 = digitalRead(S1);
      // send sensor values:
      rf.listen();// Change to RF
      //rf.print("{\"a0\":"); 
      rf.print("{\"id\":");
      rf.print(name);    
      rf.print(",\"a0\":"); 
      rf.print(temp);
      rf.print(",\"a1\":");
      rf.print(BPM);  
      //rf.print(",\"b1\":");
      //rf.print(Signal);  
      rf.println("}");//END JSON  
      //delay(200);   
      rf.flush();
      break;	
    }//end of switch
  }//end of IF available char
}
void sendDataToProcessing(char symbol, int data ){
  Serial.print(symbol);                // symbol prefix tells Processing what type of data is coming
  Serial.println(data);                // the data to send culminating in a carriage return
}









