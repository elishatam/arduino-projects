#include <HX711.h>

//Time spent: 9:50pm - 10:15. 25min- Button and LED code is up
//Sat, 8/29. 2:45pm-4:30 = 1:45 - tested out load cell
//Sun, 8/30. 1hr - set up test setup
//Tues, 9/1. 1hr- Solder pins on HX711 and crimp pins to wire

typedef enum { FIRSTSTATE,
               SETTARGETWEIGHT,
               CHECKWEIGHT
} STATE_t;

#define calibration_factor 210000.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DOUT 3 //Define pin number for DOUT of weight sensor
#define CLK 2  //Define pin number for CLK of weight sensor

const int buttonPin = 7;
const int ledPin = 6;

//variables will change
int buttonState = 0; 
int lastButtonState = 0;

HX711 scale(DOUT, CLK);
STATE_t myState; //Current state
STATE_t dummyState; //Used to prevent switching states within one while loop iteration

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  //Read the state of the last pushbutton
  lastButtonState = digitalRead(buttonPin);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  myState = FIRSTSTATE;
}

void loop() {
  float targetWeight, weight;
  
  dummyState = myState;

  switch (dummyState){
    case FIRSTSTATE:
      //Read the state of the pushbutton
      buttonState = digitalRead(buttonPin);  
      Serial.print(scale.get_offset());
      Serial.print("\t");
      Serial.println(buttonState);
      if (buttonState == 0){ //When pressed, buttonState = 0
        myState = SETTARGETWEIGHT;
      }
      break;

    case SETTARGETWEIGHT:
      digitalWrite(ledPin, HIGH);
      weight = scale.get_units(); //in lbs
      Serial.println(weight, 2);
      if (buttonState == 0){
        targetWeight = weight;
      }
      break;
      
  }
  
  
  
  
  

/*  //read the state of the pushbutton
  buttonState = digitalRead(buttonPin);
  if (buttonState == 0){
    digitalWrite(ledPin, HIGH);
    Serial.println(buttonState);
  }
  else {
    digitalWrite(ledPin, LOW);
  }
  
  delay(10);
*/
/*
  float weight;
  weight = scale.get_units(); //in lbs
  Serial.println(weight, 2);
*/
 

}
