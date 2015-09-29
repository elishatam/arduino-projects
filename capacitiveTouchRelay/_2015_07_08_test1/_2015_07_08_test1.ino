#include <CapacitiveSensor.h>

#include <CapacitiveSensor.h>
/*Elisha's time spent:
 - 3 hrs researching different sensors - capacitive, pressure,
 - 4 hrs trying to get wireless way to activate capacitive sensor
 - 1 hr soldering and figuring
 - 1hr - timing, non blocking code
 - 4:12
*/

/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10 megohm between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50 kilohm - 50 megohm. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 * Best results are obtained if sensor foil and wire is covered with an insulator such as paper or plastic sheet
 */
 
 //Pin 4 = Send pin, connect to 1M
 //Pin 2 = Receive pin, connect to 1k
 //Relay
 //     - digitalWrite(relayPin, LOW); = Pin 1 (red wire after fuse) and 3 (yellow wire) of Relay are not continuous. = Elisha assumes unlocked 
 //     - digitalWrite(relayPin, HIGH); = Pin 1 (red wire after fuse) and 3 (yellow wire) of Relay are continuous. = Elisha assumes locked
 //     - After initialization, HIGH = LOCK 
 //     - After touching for 10sec, LOW = UNLOCK

typedef enum { INITIALIZATIONSTATE,
               WAITINGFORTOUCH,
               UNLOCK
} STATE_t;



CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);        // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil
int relayPin = 7;
int ledPin = 13;
unsigned long keyPrevMillis = 0;
unsigned long unlockPrevMillis = 0;
const unsigned long keySampleIntervalMs = 100;
const unsigned long triggerCapacitanceAmount = 50;
unsigned int triggerAmount;
byte longKeyPressCountMax = 100;    // 100 (longkeyPressCountMax) * 100 (loops every 100ms) = 10,000ms
byte longKeyPressCount = 0;

//Smoothing tutorial https://www.arduino.cc/en/Tutorial/Smoothing
const int numReadings = 100;
long firstReadings[numReadings];
unsigned int index =0;
unsigned int total = 0;
unsigned int average = 0;
unsigned int initializationPeriod = 0;


unsigned int prevKeyState = 0; //to start off, capacitive sensor state is 0, <200.

STATE_t myState; //Current state
STATE_t dummyState; //Used to prevent switching states within one while loop iteration


void setup()                    
{
  //Debug
  DDRB = DDRB | B00100000; //Set Arduino pin 13 (LED) to output, rest to input
  //
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    firstReadings[thisReading] = 0;
    
   pinMode(relayPin, OUTPUT);
   cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
   Serial.begin(9600);
   lock();
   myState = INITIALIZATIONSTATE;

}

void lock() {
    Serial.println("Locked");
    digitalWrite(relayPin, HIGH); //make sure relay is high = lock
    digitalWrite(ledPin, HIGH);
}

// called when touched for >10seconds
void unlock() {
    Serial.println("Unlocked the lock");
    digitalWrite(relayPin, LOW); //set Relay to low to unlock and disconnect Relay
    digitalWrite(ledPin, LOW);
}

// called when state goes from not touched to touched
void beginTouch() {
    Serial.print("Begin Touch");
    longKeyPressCount = 0;
}


void loop() {
  unsigned long currentMillis;
  currentMillis = millis();
  dummyState = myState;
  
  //Check capacitive sensor every 100ms (keySampleIntervalMs)
  if (currentMillis - keyPrevMillis >= keySampleIntervalMs){
    keyPrevMillis = currentMillis;

    switch (dummyState){
      case INITIALIZATIONSTATE:{
        //Get average first reading during initialization time
        //Initialization time = 10s (100 * 100ms = 10,000ms = 10s)
        if (initializationPeriod < 100){
            Serial.print("INIT");
            Serial.print("\t");
            total = total - firstReadings[index];
            firstReadings[index] = cs_4_2.capacitiveSensor(30); //read sensor
            Serial.print(firstReadings[index]);
            total = total + firstReadings[index];
            index = index + 1;
          
            if (index >= numReadings)
              index = 0;
            
            average = total / numReadings;
            initializationPeriod++;
            Serial.print("\t");
            Serial.println(average);
        }
        else {
            myState = WAITINGFORTOUCH;
        }
        break;
      }
      case WAITINGFORTOUCH:{
        //Now we know the average first reading. 
        //Our triggerAmount will just be triggerCapacitanceAmount (50) above the average reading 
        //If capacitive reading got above this triggerAmount, then we assume someone is touching the sensor     
        triggerAmount = average + triggerCapacitanceAmount;
        long currKeyState = cs_4_2.capacitiveSensor(30); //read sensor state
        if ((prevKeyState < triggerAmount) && (currKeyState > triggerAmount)){
          beginTouch(); //Relative measurement. Capacitance amount has to be > prevKeyState by triggerCapacitiveAmount in order to trigger.
        }
        else if (currKeyState > triggerAmount){
          longKeyPressCount++;
        }
        if (longKeyPressCount >= longKeyPressCountMax){
          myState = UNLOCK;
          unlockPrevMillis = millis();
          longKeyPressCount = 0;
        }
        
        prevKeyState = currKeyState;
        Serial.print("WAITINGFORTOUCH");
        Serial.print("\t");
        Serial.print(average);
        Serial.print("\t");
        Serial.print(triggerAmount);
        Serial.print("\t");
        Serial.print(longKeyPressCount);
        Serial.print("\t");                 
        Serial.println(currKeyState); 
        break;
     }
     case UNLOCK:{
        Serial.print("UNLOCK");
        Serial.print("\t");
        unlock(); 
        if (millis() - unlockPrevMillis >= 10000){
          lock(); 
          myState = WAITINGFORTOUCH;
          unlockPrevMillis = millis();
        }
        break;
     }
    }
  }
}


