const int dirPin = 2;
const int stepPin = 5;

const int STEPS_PER_REV = 200;

void setup() {
  // put your setup code here, to run once:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(dirPin,HIGH);

  for(int x=0; x<(STEPS_PER_REV*2); x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(1000);    
  }

  delay(1000);

  digitalWrite(dirPin,LOW);

  for(int x=0; x<(STEPS_PER_REV*2); x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(1000);    
  }

  delay(1000);
  
}
