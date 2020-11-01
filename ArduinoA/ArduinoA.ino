//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>

#define joyX A0       //joystick pin 1
#define joyY A1       //joystick pin 2

//create an RF24 object
RF24 radio(7, 8);  // CE, CSN
LiquidCrystal_I2C lcd(0x3f,  16,2);  // Set the LCD I2C address, if it's not working try 0x27.

const byte address[6] = "00001";    //address for the radio to use

//struct for radio data sending
struct Data{
  int x;          //xPos 
  int y;          //yPos
  bool disabled;  //isProgramDisabled
  int potTrim;    //the value of the trim potentiometer
} data;

int trimMap;    //this is where we map the trim pot to

int buttonPin = 2; //pun the push button is on
int state = HIGH;  //required for the button debouncing

long time = 0;              //required for the button debouncing
long debounce = 200;        //time to wait for button clicking
 bool updateScreen = false; //used in the screens 100ms interrupt
 float value = 156250000;   //this is used in the math for 100ms screen refresh     
/*---------------------------------------------------------------------Setup functions------------------------------------------------------------------*/
//this is the screen setup routine
//this inits the screen, turns on the backlight
//debug functionality commented out. 
void screenSetup(){
  lcd.init();
  lcd.backlight();
  // lcd.setCursor(0,0);
  // lcd.print("Starting");
  // lcd.setCursor(0,1);
  // lcd.print("This is working");
  Serial.println("screen setup Success");
  lcd.clear();
}
//function for settting up the joystick
void joystickSetup(){
    pinMode(buttonPin, INPUT_PULLUP); //setup the pin for the button
    attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonInterrupt, RISING); //put the button interrupt on the rising edge
    Serial.println("joystick setup Success");
}
//setup the radio
void radioSetup(){
  radio.begin();                    //start the radio
  radio.openWritingPipe(address);   //set the address
  radio.setDataRate(RF24_250KBPS);  //set the data rate
  radio.setPALevel(RF24_PA_MIN);    //set the power required
  radio.enableAckPayload();         //enable AckPayload
  radio.setRetries(5,5);            // delay, count for retries
  radio.stopListening();            //stop the radio from listening so it can broadcast
  Serial.println("radio setup Success");
}
/*---------------------------------------------------------------------Loop functions------------------------------------------------------------------*/
//this is the screen refresh loop
//we do a lot of going back to clear specific parts of the screen
// so we don't have to keep calling lcd.clear
void screenContents(){
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
    lcd.print("on ");
  }
  else if(data.disabled == true){
    lcd.print("off");

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
  lcd.print(trimMap);
  updateScreen = false;       //set this back to false to begin the timer for refresh again
}
//collect the data for the joystick
void joystickFunction(){
  data.x = analogRead(joyX);
  data.y = analogRead(joyY);
 
}
//used for the button interrupt to stop everything from working when the button is clicked
void handleButtonInterrupt(){
   reading = digitalRead(buttonPin);
    if(reading == HIGH && millis() - time > debounce){
      state = !state;
      time = millis();
    }
    if(state == HIGH){
      data.disabled = true;
    }
    else{
      data.disabled = false;
    }
}
/*---------------------------------------------------------------------Timer processes------------------------------------------------------------------*/
//main setup function
void setup() {
  Serial.begin(9600);   //start serial at 9600 baud
  screenSetup();        //run screen setup  
  joystickSetup();      //run joystick setup
  radioSetup();         //run radio setup
  noInterrupts();       //disable interrupts
  TCCR1A = 0;           //init value
  TCCR1B = 0;           //init value
  TCNT1 = value;                        // preload timer
  TCCR1B |= (1 << CS10)|(1 << CS12);    // 1024 prescaler 
  TIMSK1 |= (1 << TOIE1);               // enable timer overflow interrupt ISR
  interrupts();          //reenable interrupts
}
//i only vaguely understand how this works
ISR(TIMER1_OVF_vect)                    // interrupt service routine for overflow
{
  TCNT1 = value;                        // preload timer
  updateScreen = true;                  // tell the program it can update the screen now
}
void loop() {
  joystickFunction();       //get joystick data
  if(updateScreen){         //if its time update the screen
    screenContents();
  }
  bool result;              //check in with radio to write data
  result = radio.write(&data, sizeof(data));
  if(result){               //if the data sends
    if (radio.isAckPayloadAvailable() ) { //check for for AckPayload
          radio.read(&data.potTrim, sizeof(data.potTrim)); //pull the trim pot data
          trimMap = map(data.potTrim, 0,1023,0,180);       //map it to a real value
      }
  }else{
    Serial.print("tx failed"); //else its broken 
  }
}
