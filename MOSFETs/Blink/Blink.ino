/*
  Figaro Sensor
  Pin 2 has an LED connected on MSP430 boards, has a name 'RED_LED' in the code.
  
  Hardware Required:
  * MSP-EXP430G2 LaunchPad
  
*/
  
#define VOCPWMPin 8 //(P2_0)  
int VOCAnalogPin = A5;
int VOCVal = 0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(VOCPWMPin, OUTPUT);
  pinMode(RED_LED, OUTPUT);   
  Serial.begin(9600);  
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(VOCPWMPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  VOCVal = analogRead(VOCAnalogPin); //read the input pin
  delay(2);               // wait for a second
  digitalWrite(VOCPWMPin, LOW);    // turn the LED off by making the voltage LOW
  delay(998);               // wait for a second
  Serial.println(VOCVal);
}
