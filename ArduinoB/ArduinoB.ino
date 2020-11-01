#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//create an RF24 object
RF24 radio(7, 8);  // CE, CSN
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
//address through which two modules communicate.
const byte address[6] = "00001";
int pos = 0;    // variable to store the servo position
int trimMap;
int motorMap;
//the pin the potentiometer is on
int potPin = 2;
struct Data{
  int x;
  int y;
  bool disabled;
  int potTrim;
} data;
struct Data transferData;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1,address);
  radio.setPALevel(RF24_PA_MIN);   
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();
  radio.writeAckPayload(1,&data.potTrim, sizeof(data.potTrim));
  Serial.println("radio setup successful");
  radio.startListening();
  pinMode(10, OUTPUT);
  pinMode(3, OUTPUT);
  myservo.attach(4);
}

void loop() {
  delay(5);
  // put your main code here, to run repeatedly:
  if(radio.available()){
     radio.read(&transferData, sizeof(transferData));
      data.x = transferData.x;
      data.y = transferData.y;
      data.disabled = transferData.disabled;
      data.potTrim = analogRead(potPin);
      delay(5);
      radio.writeAckPayload(1,&data.potTrim, sizeof(data.potTrim));
      trimMap = map(data.potTrim, 0,1023,0,180);
      pos = map(data.x,0,1023,0,180);
      motorMap = map(data.y, 0,1023,0,255);

  } 
  if(!data.disabled){
    digitalWrite(10, HIGH);
  }else{
    digitalWrite(10, LOW);
  }
  if(data.disabled){
    myservo.write(trimMap); 
    analogWrite(3,0);
  }else{
    myservo.write(pos); 
    analogWrite(3,motorMap);
  } 
  
  
}
