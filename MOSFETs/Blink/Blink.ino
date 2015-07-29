/*
  Figaro Sensor
  Pin 2 has an LED connected on MSP430 boards, has a name 'RED_LED' in the code.
  
  Hardware Required:
  * MSP-EXP430G2 LaunchPad
  
*/
  
#define VOCPWMPin 8 //(P2_0)  
int VOCAnalogPin = A5;
int VOCVal = 0;
unsigned long previousMillis = 0;
unsigned int state = 0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(VOCPWMPin, OUTPUT);
  pinMode(RED_LED, OUTPUT);   
  Serial.begin(9600);  
}

// the loop routine runs over and over again forever:
void loop() {
  unsigned long currentMillis = millis();
  
  if(state == 0 && (currentMillis - previousMillis >= 998)){ //After 998ms, make VOCPWMPin high for 2ms
    digitalWrite(VOCPWMPin, HIGH);   // turn VOCPWMPin on for 2ms
    state = 1;
    VOCVal = analogRead(VOCAnalogPin); //read the input pin
    previousMillis = currentMillis;
    Serial.println(VOCVal);  
  }

  if(state == 1 && (currentMillis - previousMillis >= 2)){ //After 2ms, make VOCPWMPin low for 998
    digitalWrite(VOCPWMPin, LOW); //turn VOCPWMPin off
    state = 0;
    previousMillis = currentMillis;
  }
    
  
}
