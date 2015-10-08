#include <HX711.h>

//Time spent: 9:50pm - 10:15. 25min- Button and LED code is up
//Sat, 8/29. 2:45pm-4:30 = 1:45 - tested out load cell
//Sun, 8/30. 1hr - set up test setup
//Tues, 9/1. 1hr- Solder pins on HX711 and crimp pins to wire
//Thur, 9/3. 1hr - program state machine and debug
//Sat, 9/13. 2hr - mechanical prep
//Sat, 9/20. 5hr - soldering. 2hr on Friday, 3hr Saturday

//Port Manipulation: https://www.arduino.cc/en/Reference/PortManipulation
//B (digital pin 8 to 13)
//C (analog input pins)
//D (digital pins 0 to 7)
//Internal eeprom: http://tronixstuff.com/2011/03/16/tutorial-your-arduinos-inbuilt-eeprom/
//Smoothing: https://www.arduino.cc/en/Tutorial/Smoothing
//Running Average Library: http://playground.arduino.cc/Main/RunningAverage
//Hold time: http://jmsarduino.blogspot.com/2009/05/click-for-press-and-hold-for-b.html

typedef enum { FIRSTSTATE,
               SETTARGETWEIGHT,
               WAITINGFORWEIGHTREMOVAL,
               WAITINGFORPROPERWEIGHT,
               UNLOCK
} STATE_t;

#define calibration_factor_one 106000.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DOUT_ONE 11 //Define pin number for DOUT of weight sensor
#define CLK_ONE 10  //Define pin number for CLK of weight sensor
#define calibration_factor_two 102000.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DOUT_TWO 9 //Define pin number for DOUT of weight sensor
#define CLK_TWO 8 //Define pin number for CLK of weight sensor
#define calibration_factor_three 107000.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DOUT_THREE A0 //Define pin number for DOUT of weight sensor
#define CLK_THREE A1 //Define pin number for CLK of weight sensor
#define calibration_factor_four 107000.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DOUT_FOUR A2 //Define pin number for DOUT of weight sensor
#define CLK_FOUR A3 //Define pin number for CLK of weight sensor
#define calibration_factor_five 107500.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DOUT_FIVE A4 //Define pin number for DOUT of weight sensor
#define CLK_FIVE A5 //Define pin number for CLK of weight sensor

#define holdTime 200 // ms hold period: how long to wait for press+hold event
#define matchingRange 0.03

const int buttonPin = 3;
const int ledPin = 4;
const int relayPin = 2;
const int arduinoLEDPin = 13;
const unsigned long ledIntervalMs = 500;

//variables will change
int currentButtonState = 0; 
int lastButtonState = 0;
unsigned long ledPrevMillis = 0;
unsigned long buttonPrevMillis = 0;

float targetWeightOne = 0;
float weightOne;
float targetWeightTwo = 0;
float weightTwo;
float targetWeightThree = 0;
float weightThree;
float targetWeightFour = 0;
float weightFour;
float targetWeightFive = 0;
float weightFive;
int matchingVal = 0;
int lastMatchingVal = 0;
unsigned long matchingTime;

HX711 scaleOne(DOUT_ONE, CLK_ONE);
HX711 scaleTwo(DOUT_TWO, CLK_TWO);
HX711 scaleThree(DOUT_THREE, CLK_THREE);
HX711 scaleFour(DOUT_FOUR, CLK_FOUR);
HX711 scaleFive(DOUT_FIVE, CLK_FIVE);

STATE_t myState; //Current state

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(arduinoLEDPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(arduinoLEDPin, LOW);
  digitalWrite(relayPin, HIGH); //set Relay to initially HIGH = lock
  Serial.begin(9600);

  //Read the state of the last pushbutton
  lastButtonState = digitalRead(buttonPin);
  scaleOne.set_scale(calibration_factor_one); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scaleOne.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  scaleTwo.set_scale(calibration_factor_two); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scaleTwo.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  scaleThree.set_scale(calibration_factor_three); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scaleThree.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  scaleFour.set_scale(calibration_factor_four); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scaleFour.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  scaleFive.set_scale(calibration_factor_five); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scaleFive.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0


  myState = FIRSTSTATE;
}

void loop() {
  unsigned long currentMillis = millis();
  STATE_t dummyState = myState; //Used to prevent switching states within one while loop iteration
  
  switch (dummyState){
    case FIRSTSTATE:
      digitalWrite(ledPin, HIGH);
      //Read the state of the pushbutton
      currentButtonState = digitalRead(buttonPin);  
      Serial.print(scaleOne.get_offset());
      Serial.print("\t");
      Serial.print(scaleTwo.get_offset());
      Serial.print("\t");
      Serial.print(scaleThree.get_offset());
      Serial.print("\t");
      Serial.print(scaleFour.get_offset());
      Serial.print("\t");
      Serial.print(scaleFive.get_offset());
      Serial.print("\t");
      Serial.println(currentButtonState);
      if ((currentButtonState) == 0 && (lastButtonState == 1)){ //Falling edge. When pressed, buttonState = 0
        myState = SETTARGETWEIGHT;
        currentButtonState == 1; //Reset
        buttonPrevMillis = millis();
      }
      break;

    case SETTARGETWEIGHT:
      //digitalWrite(ledPin, HIGH);
      //currentMillis = millis();
      if (currentMillis - ledPrevMillis >= ledIntervalMs){
        ledPrevMillis = currentMillis;
        toggleLED();
      }
      targetWeightOne = scaleOne.get_units(); //in lbs
      targetWeightTwo = scaleTwo.get_units(); //in lbs
      targetWeightThree = scaleThree.get_units(); //in lbs
      targetWeightFour = scaleFour.get_units(); //in lbs
      targetWeightFive = scaleFive.get_units(); //in lbs

      Serial.print(targetWeightOne, 2);
      Serial.print("\t");
      Serial.print(targetWeightTwo, 2);
      Serial.print("\t");
      Serial.print(targetWeightThree, 2);
      Serial.print("\t");
      Serial.print(targetWeightFour, 2);
      Serial.print("\t");
      Serial.println(targetWeightFive, 2);


      currentButtonState = digitalRead(buttonPin); 

      //Wait before reading button again
      if (currentMillis - buttonPrevMillis >= 2000){
        if ((currentButtonState) == 0 && (lastButtonState == 1)){ //Falling edge. When pressed, buttonState = 0
          myState = WAITINGFORWEIGHTREMOVAL;
          currentButtonState == 1; //Reset
        }
      }
      break;

    case WAITINGFORWEIGHTREMOVAL:
      digitalWrite(ledPin, LOW);
      Serial.print("WAITINGFORWEIGHTREMOVAL");
      Serial.print("\t");
      digitalWrite(ledPin, LOW); //Make sure LED is off
      readWeights();
      printWeights();


      //Check to make sure weight does not match targetWeight for the holdTime
      //If so, go to WAITINGFORPROPERWEIGHT state
      matchingVal = isMatching(weightOne, targetWeightOne, weightTwo, targetWeightTwo, weightThree, targetWeightThree, weightFour, targetWeightFour, weightFive, targetWeightFive);
      if (matchingVal == 0 && lastMatchingVal == 1){
        matchingTime = millis();
      }

      if (matchingVal == 0 && lastMatchingVal == 0 && (millis() - matchingTime) > long(holdTime)){
        myState = WAITINGFORPROPERWEIGHT;
      }
      lastMatchingVal = matchingVal;      

      
      break;

    case WAITINGFORPROPERWEIGHT:
      digitalWrite(ledPin, HIGH);
      Serial.print("WAITINGFORPROPERWEIGHT");
      Serial.print("\t");
      lock();
      readWeights();
      printWeights();
      
      //if (isMatching(weight, targetWeight)){
      //  myState = UNLOCK;
      //}
      //Check to make sure weight matches targetWeight for the holdTime
      //If so, UNLOCK 
      matchingVal = isMatching(weightOne, targetWeightOne, weightTwo, targetWeightTwo, weightThree, targetWeightThree, weightFour, targetWeightFour, weightFive, targetWeightFive);
      if (matchingVal == 1 && lastMatchingVal == 0){
        matchingTime = millis();
      }

      if (matchingVal == 1 && lastMatchingVal == 1 && (millis() - matchingTime) > long(holdTime)){
        myState = UNLOCK;
      }
      lastMatchingVal = matchingVal;
      break;

    case UNLOCK:
      Serial.print("UNLOCK");
      Serial.print("\t");
      unlock();
      readWeights();
      printWeights();
      
      //if (!isMatching(weight, targetWeight)){
      //  myState = WAITINGFORPROPERWEIGHT;
      //}
      //Check to make sure weight does not match targetWeight for the holdTime. 
      //If so, go to WAITINGFORPROPERWEIGHT state
      matchingVal = isMatching(weightOne, targetWeightOne, weightTwo, targetWeightTwo, weightThree, targetWeightThree, weightFour, targetWeightFour, weightFive, targetWeightFive);
      if (matchingVal == 0 && lastMatchingVal == 1){
        matchingTime = millis();
      }

      if (matchingVal == 0 && lastMatchingVal == 0 && (millis() - matchingTime) > long(holdTime)){
        myState = WAITINGFORPROPERWEIGHT;
      }
      lastMatchingVal = matchingVal;      
      break;
  }

  
}

int isMatching(float weightOne, float targetWeightOne, float weightTwo, float targetWeightTwo, float weightThree, float targetWeightThree, float weightFour, float targetWeightFour, float weightFive, float targetWeightFive){
  if ((abs(weightOne-targetWeightOne) < matchingRange) && (abs(weightTwo-targetWeightTwo) < matchingRange) && (abs(weightThree-targetWeightThree) < matchingRange) && (abs(weightFour-targetWeightFour) < matchingRange) && (abs(weightFive-targetWeightFive) < matchingRange)){  //it matches
    return 1;
  }
  return 0;  
}

void toggleLED(void){
  DDRD ^= B00010000; //Toggle ledPin
  //DDRB ^= B00000001;
  
}

void unlock(void){
  digitalWrite(arduinoLEDPin, HIGH);
  digitalWrite(relayPin, LOW); //set Relay to LOW = unlock
}

void lock(void){
  digitalWrite(arduinoLEDPin, LOW);
  digitalWrite(relayPin, HIGH); //set Relay to HIGH = lock
}

void readWeights(void){
  weightOne = scaleOne.get_units(); //in lbs
  weightTwo = scaleTwo.get_units();
  weightThree = scaleThree.get_units();
  weightFour = scaleFour.get_units();
  weightFive = scaleFive.get_units();
}

void printWeights(void){
  Serial.print(targetWeightOne);
  Serial.print("\t");
  Serial.print(weightOne);      
  Serial.print("\t");
  Serial.print(targetWeightTwo);
  Serial.print("\t");
  Serial.print(weightTwo);
  Serial.print("\t");
  Serial.print(targetWeightThree);
  Serial.print("\t");
  Serial.print(weightThree);
  Serial.print("\t");
  Serial.print(targetWeightFour);
  Serial.print("\t");
  Serial.print(weightFour);
  Serial.print("\t");
  Serial.print(targetWeightFive);
  Serial.print("\t");
  Serial.println(weightFive);
}



