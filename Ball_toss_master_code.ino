
//    The direction of the car's movement
// PWM(44)  RPWM(42)     LPWM(40)    Pins of BTN7960
// 0-255     HIGH         LOW        CW
// 0-255     LOW          HIGH       CCW
// 0         LOW          LOW        STOP
// mp3 rx d12
// mp3 tx d11
// display a5 a7
// encoder pushbutton A4
// encoder clk a3 data a2
// 5 volt motot enaa 5, IN1 a6, IN2 a1
// D10 software reset active low
// Bluetooth TX to Nano D0, Bluetooth RX to Nano D1 through voltage devider
//  Reset=A= hex=41, Go=B=hex=42,Dog: both=C=hex=43, Jax=D=hex=44, Charlie=E=hex=45  Distance: 15ft=F=hex=46, 25ft=G=hex=47, 35ft=H=hex=48
// Manual toss =I=hex=49, Automatic toss=J=hex=4A, Manual feed=K=hex=4B, Automatic feed=L=hex=4C, Shutdown=M=hex=4D

#include <LiquidCrystal_I2C.h>                                                           // liquid crystal display library
#include <Wire.h>                                                                        // for liquid crystal display
#include <DFRobotDFPlayerMini.h>                                                         // library for the MP3 player
#include <Servo.h>                                                                       // library for servo motor for ball release
#include <SoftwareSerial.h>                                                              // software serial library to communicate with mp3 player and bluetooth
#include <PinChangeInterrupt.h>                                                          // pin change interrupt library to increase number of interrupt pins available
#include <PinChangeInterruptBoards.h>                                                    // pin change interrupt library to increase number of interrupt pins available
#include <PinChangeInterruptPins.h>                                                      // pin change interrupt library to increase number of interrupt pins available
#include <PinChangeInterruptSettings.h>                                                  // pin change interrupt library to increase number of interrupt pins available
LiquidCrystal_I2C lcd(0x27,16,2);                                                        // defining the lcd addresss of 0x27 and 16 Characters and 2 lines
DFRobotDFPlayerMini player;                                                              // naming mp3 player just player
byte arrow[] = {0x00, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};                         // defines the arrow used on the display menu
SoftwareSerial bluetoothSerial(8,7);  //RX, TX communications to Bluetooth module        // defining the pins for the bluetooth serial communication lines
SoftwareSerial MP3Serial(12,11);  //RX, TX  communications to MP3 player                 // defining the pins for the MP3 player serial communication lines
Servo ballReleaseservo;                                                                  // naming the variable speed servo myservo
#define ballPresent 2                                                                    // Ball present optical switch on data pin 2
#define PWM_upper 3                                                                      // pwm for Upper Motor set to go CCW is on data pin 3
#define PWM_lower 6                                                                      // pwm for Lower Motor set to go CW is on data pin 6
#define softwareReset 10                                                                 // software reset is from data pin 10 and tied to reset line of nano
#define mp3Busy 13                                                                       // MP3 busy line is tied to data pin 13
#define ENA 5                                                                            // Enable feeder motor is tied to pwm data pin 5
#define encoderClk A2                                                                    // clk pin of rotary encoder is tied to data pin A2
#define encoderData A3                                                                   // data pin of rotary encoder is tied to data pin A3
#define rotaryEncoderPushButton A4                                                       // rotary encoder push button is tied to data pin A4
#define sdaDisplay A5                                                                    // Display I2C SDA line is tied to data line A5
#define sclDisplay A7                                                                    // Display I2C SCL line is tied to data line A7
int i;
unsigned long mytime = 0;
float period = 0.0000000000;
unsigned long starttime = 0;
float rpm = 0.0000000000;
unsigned long frequency;
unsigned long Input = 0x00;                                                              // input data from user interface
int pos = 0;
int Dog=1;                                                                               // which dog we are playing with default is 1 or both or any
int Distance=1;                                                                           // distance we are going to throw the ball default is 1 or 15 feet                                                                      // if we are going to manually control toss of the ball or not default is 1 or yes
int automaticToss=0;                                                                     // if we are going to automatically control the toss of the ball or not default is 0 or no
int automaticFeed=0;                                                                     // if we are going to automatically feed the dog or not default is 0 or no
int buttoncount=0;                                                                       // keeps track of how many times the encoder pushbutton has been pushed
int count=0;                                                                             // keeps track of how many times the clk has switched on encoder
int vol = 30;                                                                            // sets volume of mp3 player before audio amplifier.
int first = 0;                                                                           // used to show first time through the music menu
int Menu1 = 1;                                                                           // keeps track of what menu we are in
int Menu2 = 0;
int Menu3 = 0;
int playing = 1;                      // keeps track if the mp3 player is playing or not
int pointer = 0;                      // keeps track of where we are in the menu
float motorPWM=0;
int distance=15;
volatile unsigned long last_micros;    // used in timing the debounce of the button and clk and data lines
long debouncing_time = 800;           // debounce time for the button and the data and clk lines on the encoder
int ballAvailable=0;                      // variable stating ball is available





void setup() {
digitalWrite(Reset, HIGH);                                                               // initial setup of the software reset is high so no reset at powerup
bluetoothSerial.begin(9600);                                                             // setting the baud rate of the bluetooth serial communications  to 9600
MP3Serial.begin(9600);                                                                   // setting the baud rate of the MP3 player serial communications to 9600 
Serial.begin(9600);   
delay(100);                                                                              // short delay to allow baud rates to set
pinMode(ballPresent, INPUT_PULLUP);                                                      // Ball present line is set to an input with a pullup
pinMode(PWM_upper,OUTPUT);                                                               // PWM line for ball toss upper motor is set to an output 
pinMode(PWM_lower, OUTPUT);                                                              // PWM line for ball toss lower motor is set to an output
pinMode(ENA, OUTPUT);                                                                    // ENA line for dog feeder motor is set to an output                                                                  
pinMode(softwareReset, OUTPUT);                                                          // software reset line is set ot an output
pinMode(mp3Busy, INPUT_PULLUP);                                                          // MP3 busy line is set to an input with a pullup
pinMode(encoderClk, INPUT_PULLUP);                                                       // the rotary encoder clk line is set to an input with a pullup
pinMode(encoderData, INPUT_PULLUP);                                                      // the rotary encoder data line is set to an input with a pullup
pinMode(rotaryEncoderPushButton, INPUT_PULLUP);                                          // the rotary encoder push button line is set to an input with a pullup
pinMode(sdaDisplay, OUTPUT);                                                             // the display I2C sda line is set to an output
pinMode(sclDisplay, OUTPUT);                                                             // the display I2C scl line is set to an output
analogWrite(PWM_upper, 0);                                                               // doing an analog write of a 0 to the PWM line of the upper ball toss motor turns it off
analogWrite(PWM_lower, 0);                                                               // doing an analog write of a 0 to the pwm line of the lower ball toss motor turns it off
ballReleaseservo.attach(9);                                                              // ball release servo is tied to pin 9
ballReleaseservo.write(0);                                                               // positions the ball release servo to the zero position at full speed
attachInterrupt(digitalPinToInterrupt(rotaryEncoderPushButton), debounceInterrupt1, FALLING);    // sets the interrupt for the rotary encoder pushbutton to be falling
attachInterrupt(digitalPinToInterrupt(encoderClk), debounceInterrupt, FALLING);                 // sets the clk line from the encoder interrupt  to falling.
lcd.init();                                                                             // initializes the lcd
lcd.createChar(1, arrow);                                                               // creates the arrow character for the lcd
lcd.backlight();                                                                        // turns the lcd backlight on
lcd.clear();                                                                            // clears the lcd
lcd.setCursor(0,0);                                                                     // sets the cursor to position 0 and line 0 
lcd.print(" Jax and Charlie's");                                                        // prints what is between () and "" to the lcd
lcd.setCursor(0,1);                                                                     // sets the cursor to postion 0 and line 1 
lcd.print(" Ball Toss Unit ");                                                          // prints what is between () and "" to lcd
player.begin(MP3Serial);                                                                // from the library for the mp3 player to start the software serial stream
delay(300);                                                                             // just a delay to stabilize before starting the mp3 player
player.volume(vol);                                                                     // sets the mp3 player volume
player.play(1);                                                                         // tells the mp3 player to play the mp3 between the ()
delay(5000);                                                                            // delays while playing the mp3
}

void loop() {
  while(count == 0 && Menu1 == 1 && buttoncount == 0 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Rotate to");                                                              // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("advance menu");                                                           // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
  }
 while(count == 1 && Menu1 == 1 && buttoncount == 0 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("DOG is Jax");                                                          // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
 }
 while(count == 2 && Menu1 == 1 && buttoncount == 0 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("DOG is Charlie");                                                          // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
} 
 }
while(count == 3 && Menu1 == 1 && buttoncount == 0 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("DOG is Any");                                                          // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
while(count == 4 && Menu1 == 1 && buttoncount == 0 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Return to top");                                                          // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
if(count == 1 && Menu1 == 1 && buttoncount == 1 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  Input=0x44;
  Menu1=0;
  Menu2=1;
  count=0;
  buttoncount=0;
} 

if(count == 2 && Menu1 == 1 && buttoncount == 1 && Input == 0x00){
  Input=0x45;
  Menu1=0;
  Menu2=1;
  count=0;
  buttoncount=0;
}
if (count == 3 && Menu1 == 1 && buttoncount == 1 && Input == 0x00){
  Input=0x43;
  Menu1=0;
  Menu2=1;
  count=0;
  buttoncount=0;                                                                                                                                                                                                                                       
}
if(count == 4 && Menu1 == 1 && buttoncount == 1 && Input == 0x00){
  count=1;
}
if(count>=5){
  count=1;
}
if(count<=0){
  count=4;
}
while(count == 0 && Menu1 == 2 && buttoncount == 0 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Distance is 15ft");                                                          // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
while(count == 1 && Menu1 == 2 && buttoncount == 0 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Distance is 25ft");                                                          // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
while(count == 2 && Menu1 == 2 && buttoncount == 0 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Distance is 35ft");                                                          // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
while(count == 3 && Menu1 == 2 && buttoncount == 0 && Input == 0x00){                // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Return to Top");                                                          // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
if(count == 0 && Menu1 == 2 && buttoncount == 1 && Input == 0x00){
  Input=0x46;
  Menu1=0;
  Menu2=0;
  Menu3=1;
  count=0;
  buttoncount=0; 
}
if(count == 1 && Menu1 == 2 && buttoncount == 1 && Input == 0x00){
  Input=0x47;
  Menu1=0;
  Menu2=0;
  Menu3=1;
  count=0;
  buttoncount=0; 
}
if(count == 2 && Menu1 == 2 && buttoncount == 1 && Input == 0x00){
  Input=0x48;
  Menu1=0;
  Menu2=0;
  Menu3=1;
  count=0;
  buttoncount=0; 
}
if(count == 3 && Menu1 == 2 && buttoncount == 0 && Input == 0x00){
  count=0;
}
if(count>=4){
  count=0;
}
if(count<0){
  count=3;
}
while(count == 0 && Menu1 == 3 && buttoncount == 0 && Input == 0x00){                  // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Want Manual Toss");                                                       // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
while(count == 1 && Menu1 == 3 && buttoncount == 0 && Input == 0x00){                  // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Want Automatic Toss");                                                    // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
while(count == 2 && Menu1 == 3 && buttoncount == 0 && Input == 0x00){                  // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Want Manual Feed");                                                       // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
while(count == 3 && Menu1 == 3 && buttoncount == 0 && Input == 0x00){                  // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Want Automatic Feed");                                                    // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
while(count == 4 && Menu1 == 3 && buttoncount == 0 && Input == 0x00){                  // the while statement keeps us in this part of the menu until count menu or button are changed  
  lcd.clear();                                                                         // clears the lcd to prepare to print something to lcd
  lcd.setCursor(0,0);                                                                  // sets the cursor to position 0 and line 0
  lcd.print("Shutdown");                                                               // prints what is between "" and () to the lcd
  lcd.setCursor(0,1);                                                                  // moves cursor to position 0 line 1
  lcd.print("press to select");                                                        // prints what is between "" and () to the lcd
  delay(500);                                                                          // creats a delay before  playing the mp3
  if (bluetoothSerial.available()>0){                                                  // Checks to see if any serial communications from the bluetooth module.
  Input=bluetoothSerial.read();                                                        // if there is communication from the blue tooth this command reads and stores the value in variable Input
}
}
if(count == 0 && Menu1 == 3 && buttoncount == 1 && Input == 0x00){
  Input=0x49;
  Menu1=0;
  Menu2=0;
  Menu3=0;
  count=0;
  buttoncount=0; 
}
if(count == 1 && Menu1 == 3 && buttoncount == 1 && Input == 0x00){
  Input=0x4A;
  Menu1=0;
  Menu2=0;
  Menu3=0;
  count=0;
  buttoncount=0; 
}
if(count == 2 && Menu1 == 3 && buttoncount == 1 && Input == 0x00){
  Input=0x4B;
  Menu1=0;
  Menu2=0;
  Menu3=0;
  count=0;
  buttoncount=0; 
}
if(count == 3 && Menu1 == 3 && buttoncount == 1 && Input == 0x00){
  Input=0x4C;
  Menu1=0;
  Menu2=0;
  Menu3=0;
  count=0;
  buttoncount=0; 
}
if(count == 4 && Menu1 == 3 && buttoncount == 1 && Input == 0x00){
  Input=0x4D;
  Menu1=1;
  Menu2=0;
  Menu3=0;
  count=0;
  buttoncount=0; 
}
if(count>=5){
  count=0;
}
if(count<0){
  count=4;
}

if(Input == 0x41){     //Reset                                                           // if Hex code 0x41 comes from bluetooth this command calls the function Reset() to reset the nano
  Reset();                                                                               // calls Reset function
}
 if(Input == 0x4D){    //Shutdown                                                        // if Hex code 0x4D comes from the bluetooth this command calls the function Shutdown() to shutdown all motors
  Shutdown();                                                                            // calls Shutdown function
}
if (Input == 0x42){    //Go                                                              // if Hex code 0x42 comes from the bluetooth this command calls the function Go() to start the throwing and feeding
  Go();                                                                                  // calls Go function
}
if(Input == 0x43){                                                                       //Setting dog equal to both or 1
  Dog=1;
}
if(Input == 0x44){                                                                       //Setting dog equal to Jax or 2
  Dog=2;
}
if(Input == 0x45){                                                                       //Setting dog equal to Charlie or 3
  Dog=3;
}
if(Input == 0x46){                                                                       //Setting Distance equal to 15 feet or 1
  Distance=1;
  motorPWM=25;
  distance=15;
}
if(Input == 0x47){                                                                      //Setting Distance equal to 25 feet or 2
  Distance=2;
  motorPWM=31;
  distance=25;
}
if(Input == 0x48){                                                                      //Setting Distance equal to 35 feet or 3
  Distance=3;
  motorPWM=35;
  distance=35;
}
if(Input == 0x49){                                                                      //Setting manualToss equal to 1 or on
//  manualToss();
}
if(Input == 0x4A){                                                                      //Setting manualToss=0 or off and automaticToss=1 or on
  automaticToss=1;
}
if(Input == 0x4B){                                                                      //jump to manualFeed sub
  feedDog();
}
if(Input == 0x4C){                                                                      //Setting automaticFeed=1 or on
  automaticFeed=1;
}

delay(200);                                                                           // short delay to allow things to settle


Input=0x00;                                                                             // sets Input equal to Hex 0x00
}

void timing(){
  mytime = millis();
 // period = mytime-starttime;
 // frequency = (1/(period)*1000);
 // rpm=frequency * 60;
  starttime = mytime;
  Serial.print(i);
 Serial.println(mytime);
//Serial.println(period);
//Serial.println(starttime);
//Serial.println(mytime);
//Serial.println(frequency);
}

void Go(){
  while(automaticToss==1){
  ballAvailable=digitalRead(ballPresent);
  while(ballAvailable == 1 && Dog == 1){
   player.volume(vol);
  player.play(2);     // play(2) is where is the ball boy get the ball
   delay(5500);
   ballAvailable=digitalRead(ballPresent); 
  }
  while(ballAvailable == 1 && Dog == 2){
   player.volume(vol);
  player.play(3);     // play(2) is where is the ball Jax get the ball
   delay(5500);
   ballAvailable=digitalRead(ballPresent); 
  }
  while(ballAvailable == 1 && Dog == 3){
   player.volume(vol);
  player.play(4);     // play(2) is where is the ball Charlie get the ball
   delay(5500);
   ballAvailable=digitalRead(ballPresent); 
  }
  if(ballAvailable==0 && Dog == 1){
    player.volume(vol);
  player.play(3);     // play Good boyl
   delay(5500); 
   if(automaticFeed==1){
    feedDog();
   }
  }
  if(ballAvailable==0 && Dog == 2){
    player.volume(vol);
  player.play(3);     // play Good boy Jax
   delay(5500);
   if(automaticFeed==1){
    feedDog();
   } 
  }
   if(ballAvailable==0 && Dog == 3){
    player.volume(vol);
  player.play(3);     // play Good boy Charlie
   delay(5500);
   if(automaticFeed==1){
    feedDog();
   } 
  }
  delay(10000);
analogWrite(PWM_upper, motorPWM);
analogWrite(PWM_lower, motorPWM);
delay(5000);
Serial.print("I am throwing the ball");
Serial.print(distance);
Serial.println("Feet");
bluetoothSerial.print(" I am throwing the ball");
bluetoothSerial.print(distance);
bluetoothSerial.println("Feet");
delay(3000);
if(Dog==1){
 player.volume(vol);
  player.play(3);     // Get the Ball boy
   delay(5500); 
}
if(Dog==2){
 player.volume(vol);
  player.play(3);     // Get the Ball Jax
   delay(5500); 
}
if(Dog==3){
 player.volume(vol);
  player.play(3);     // Get the Ball Charlie
   delay(5500); 
}
analogWrite(PWM_upper, 0);
analogWrite(PWM_lower, 0);
delay(5000);
}
}
void Shutdown(){
analogWrite(PWM_upper, 0); 
analogWrite(PWM_lower, 0);
analogWrite(ENA, 0);
Serial.println("Stop");
 bluetoothSerial.println(" I am stopping the ball thrower & the feeder");
}
void feedDog(){
  analogWrite(ENA, 150);
  delay(2500);
  analogWrite(ENA, 0);
}
void Reset(){
  digitalWrite(softwareReset, LOW);
}
void debounceInterrupt1() {    // debounce program for encoder pushbutton
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    buttonhasbeenpushed();
    last_micros = micros();
  }
}

  void buttonhasbeenpushed(){   //program if encoder pushbutton has been pushed.
  if(digitalRead(mp3Busy) == 1){  // if mp3 is busy it will not allow buttoncount to be increased
  buttoncount++;
}
  }

void debounceInterrupt(){       //debounce program for encoder clk line
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    changeOnclk();
    last_micros = micros();
  }
}

  void changeOnclk(){  //checks for change on encoder clk line
  if(digitalRead(mp3Busy) == 1){  //if mp3 is playing it will not allow count to increase
  if(digitalRead(encoderData)== 1){
    count++;
  }
  else{
    count --;
  }
  }
  }
