//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>

#define joyX A0
#define joyY A1

//create an RF24 object
RF24 radio(7, 8);  // CE, CSN
LiquidCrystal_I2C lcd(0x3f,  16,2);  // Set the LCD I2C address, if it's not working try 0x27.

const byte address[6] = "00001";
struct Data{
  int x;
  int y;
  bool disabled;
  int potTrim;
} data;

struct Data transferData;
int buttonPin = 2;
int state = HIGH;
int previous = LOW;
int reading;
long time = 0;
long debounce = 200;

/*---------------------------------------------------------------------Setup functions------------------------------------------------------------------*/
//this is the screen setup routine
void screenSetup(){
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Starting");
  lcd.setCursor(0,1);
  lcd.print("This is working");
  //Serial.println("screen setup Success");
  lcd.clear();
}
void joystickSetup(){
    pinMode(buttonPin, INPUT_PULLUP);
     // Serial.println("joystick setup Success");
}
void radioSetup(){
  radio.begin();
  //set the address
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.enableAckPayload();
  radio.setRetries(5,5); // delay, count
  radio.stopListening();
}
/*---------------------------------------------------------------------Loop functions------------------------------------------------------------------*/
//this is the screen refresh loop
//todo we should work out the 100ms thing here maybe
void screenContents(){
  //lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("x:");
  lcd.setCursor(2,0);
  lcd.print("    ");
  lcd.setCursor(2,0);
  lcd.print(data.x);
  lcd.setCursor(6,0);
  lcd.print("status:");
  lcd.setCursor(13,0);
  if(data.disabled == false){
    lcd.print("off");
  }
  else if(data.disabled == true){
    lcd.print("on ");

  }
  lcd.setCursor(0,1);
  lcd.print("y:");
  lcd.setCursor(2,1);
  lcd.print("    ");
  lcd.setCursor(2,1);
  lcd.print(data.y);
  lcd.setCursor(6,1);
  lcd.print("Trim:");
  lcd.setCursor(11,1);
  lcd.print("    ");
  lcd.setCursor(11,1);
  lcd.print(data.potTrim);
  delay(100);
}
void joystickFunction(){
  data.x = analogRead(joyX);
  data.y = analogRead(joyY);
  reading = digitalRead(buttonPin);
    if(reading == HIGH && previous == LOW && millis() - time > debounce){
      if (state == HIGH){
        state = LOW;
      }
      else{
        state = HIGH;
      }
      time = millis();
    }
    if(state == HIGH){
      data.disabled = true;
    }
    else{
      data.disabled = false;
    }
    previous = reading;
}
/*---------------------------------------------------------------------Actual processes------------------------------------------------------------------*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  screenSetup();
  joystickSetup();
  radioSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  joystickFunction();
  screenContents();
  bool result;
  result = radio.write(&data, sizeof(data));
  if(result){
    if (radio.isAckPayloadAvailable() ) {
          radio.read(&data.potTrim, sizeof(data.potTrim));
      }
  }else{
    Serial.print("tx failed");
  }
  delay(100);
}