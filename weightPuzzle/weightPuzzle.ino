//Time spent: 9:50pm - 10:15. 25min- Button and LED code is up
const int buttonPin = 7;
const int ledPin = 6;

//variables will change
int buttonState = 0; 

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  //read the state of the pushbutton
  buttonState = digitalRead(buttonPin);
  if (buttonState == 0){
    digitalWrite(ledPin, HIGH);
    Serial.println(buttonState);
  }
  else {
    digitalWrite(ledPin, LOW);
  }
  
  delay(10);
  

}
