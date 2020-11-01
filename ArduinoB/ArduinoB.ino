#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//create an RF24 object
RF24 radio(7, 8);  // CE, CSN
Servo myServo;  // create servo object to control a servo
//address through which two modules communicate.
const byte address[6] = "00001";
int pos = 0;    // variable to store the servo position
int trimMap;    //map to hold the trim data
int motorMap;   //map to hold the motor data
int potPin = 2; //the pin the potentiometer is on
int ledPin = 10;
int servoPin = 4;
int motorPin = 3;
//struct for radio comms
struct Data{
  int x;
  int y;
  bool disabled;
  int potTrim;
} data;
struct Data transferData;

//setup function
void setup() {
  Serial.begin(9600); //start serial
  radio.begin();      //start radio

  //lots of radio startup stuff thats also explained in the other file
  radio.openReadingPipe(1,address); 
  radio.setPALevel(RF24_PA_MIN);   
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();
  radio.writeAckPayload(1,&data.potTrim, sizeof(data.potTrim));
  Serial.println("radio setup successful");
  radio.startListening();
  //
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  myServo.attach(servoPin);
}
//main loop
void loop() {
  delay(5);       //recommended for radio
  if(radio.available()){    //if radio message available
     radio.read(&transferData, sizeof(transferData)); //read the data
      data.x = transferData.x;    //this is a transfer from a holder value that fixed a bug
      data.y = transferData.y;
      data.disabled = transferData.disabled;
      data.potTrim = analogRead(potPin);   //read the trimPot
      delay(5);                            //more delays
      radio.writeAckPayload(1,&data.potTrim, sizeof(data.potTrim));   //write a return payload
      trimMap = map(data.potTrim, 0,1023,0,180);  //map the data for the default servo pos
      pos = map(data.x,0,1023,0,180);             //map the data for the current servo pos
      motorMap = map(data.y, 0,1023,0,255);       //map the motor speed

  } 
  if(!data.disabled){         //if everything is disabled
    digitalWrite(ledPin, HIGH);   //either turn on or off the led
  }else{
    digitalWrite(ledPin, LOW);
  }
  if(data.disabled){          //same for the motors and servo.
    myServo.write(trimMap); 
    analogWrite(motorPin,0);  //either sets them to 0 or a pos based on the trim pot
  }else{
    myServo.write(pos); 
    analogWrite(motorPin,motorMap);
  } 
  
  
}
