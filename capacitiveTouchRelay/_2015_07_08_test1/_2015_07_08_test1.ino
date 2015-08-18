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

CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);        // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil
//CapacitiveSensor   cs_4_5 = CapacitiveSensor(4,5);        // 10 megohm resistor between pins 4 & 6, pin 6 is sensor pin, add wire, foil
//CapacitiveSensor   cs_4_8 = CapacitiveSensor(4,8);        // 10 megohm resistor between pins 4 & 8, pin 8 is sensor pin, add wire, foil
int relayPin = 7;
unsigned long keyPrevMillis = 0;
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
   digitalWrite(relayPin, HIGH); //set Relay to initially HIGH = lock

}

// called when button is kept pressed for less than 2 seconds
void shortKeyPress() {
    Serial.println("short");
    digitalWrite(relayPin, HIGH); //make sure relay is high = lock
}

// called when button is kept pressed for more than 2 seconds
void longKeyPress() {
    Serial.println("long");
    digitalWrite(relayPin, LOW); //set Relay to low to unlock and disconnect Relay
}

// called when key goes from not pressed to pressed
void keyPress() {
    Serial.print("key press");
    longKeyPressCount = 0;
}

void keyRelease(){
    Serial.println("key release");
    
    if (longKeyPressCount >= longKeyPressCountMax){
      longKeyPress();
    }
    else {
      shortKeyPress();
    }
  
}

void loop()                    
{
    
    //Check capacitive sensor every 100ms (keySampleIntervalMs)
    if (millis() - keyPrevMillis >= keySampleIntervalMs){
      keyPrevMillis = millis();
      PORTB ^= B00100000; //Toggle LED to test Only toggles Bit 5 
      
      //Get average first reading during initialization time
      //Initialization time = 10s (100 * 100ms = 10,000ms = 10s)
      if (initializationPeriod < 100){
        total = total - firstReadings[index];
          firstReadings[index] = cs_4_2.capacitiveSensor(30); //read Key state
          Serial.print(firstReadings[index]);
          total = total + firstReadings[index];
          index = index + 1;
          
          if (index >= numReadings)
            index = 0;
            
          average = total / numReadings;
          initializationPeriod++;
          //firstTimeToRead = 0;
          //triggerAmount = average + triggerCapacitanceAmount;
          Serial.print("\t");
          Serial.println(average);
      }
      //Now we know the average first reading. 
      //Our triggerAmount will just be triggerCapacitanceAmount (100) above the average reading 
      //If capacitive reading got above this triggerAmount, then we assume someone is touching the sensor     
      else {
        triggerAmount = average + triggerCapacitanceAmount;
        long currKeyState = cs_4_2.capacitiveSensor(30); //read Key state
        if ((prevKeyState < triggerAmount) && (currKeyState > triggerAmount)){
          keyPress(); //Relative measurement. Capacitance amount has to be > prevKeyState by triggerCapacitiveAmount in order to trigger.
        }
        else if ((prevKeyState > triggerAmount) && (currKeyState < triggerAmount)){
          keyRelease();
        }
        else if (currKeyState > triggerAmount){
          longKeyPressCount++;
        }
        
        prevKeyState = currKeyState;
        Serial.print(average);
        Serial.print("\t");
        Serial.print(triggerAmount);
        Serial.print("\t");
        Serial.print(longKeyPressCount);
        Serial.print("\t");                    // tab character for debug window spacing
        Serial.println(currKeyState);                    

      }
      
      
    }
      
/*      
      long start = millis();
      long total1 = cs_4_2.capacitiveSensor(30);
      
      if (total1 > 200){
        digitalWrite(relayPin, HIGH); //connect Relay
      }
      else digitalWrite(relayPin, LOW);
    Serial.print(millis() - start);        // check on performance in milliseconds
    Serial.print("\t");                    // tab character for debug window spacing

    Serial.println(total1);                  // print sensor output 1
 /*
    }
    
    /*
    Capacitive sensor turns on relay when signal is high. Blocking code, delay for 100ms
    long start = millis();
    long total1 =  cs_4_2.capacitiveSensor(30);
    //long total2 =  cs_4_5.capacitiveSensor(30);
    //long total3 =  cs_4_8.capacitiveSensor(30);

    if (total1 > 200){
      digitalWrite(relayPin, HIGH); //connect Relay
    }
    else digitalWrite(relayPin, LOW);
    
    Serial.print(millis() - start);        // check on performance in milliseconds
    Serial.print("\t");                    // tab character for debug window spacing

    Serial.println(total1);                  // print sensor output 1
    //Serial.print("\t");
    //Serial.print(total2);                  // print sensor output 2
    //Serial.print("\t");
    //Serial.println(total3);                // print sensor output 3

    delay(100);                             // arbitrary delay to limit data to serial port 
   */
   
   
   /*
   //Test Relay Pin
   digitalWrite(relayPin, HIGH);
   Serial.println("hi");
   delay(1000);
   digitalWrite(relayPin, LOW);
   Serial.println("low");
   delay(1000);
   */
}


/*Port Manipulation
https://www.arduino.cc/en/Reference/PortManipulation
//NOTES
//0000,     0000
//7 6 13 12, 11 10 9 8
DDRB = DDRB | B00100000; //Set Arduino pin 13 (LED) to output, rest to input
void loop() {
  PORTB = PORTB | B00100000; //Sets digital pin 13 high
//  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
//  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  PORTB &= ~B00100000; //Sets digital pin 13 low
  delay(100);               // wait for a second
}   
   */

