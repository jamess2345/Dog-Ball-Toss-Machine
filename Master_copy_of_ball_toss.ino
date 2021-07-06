#include <DFRobotDFPlayerMini.h>                // library for the MP3 player
#include <SoftwareSerial.h>                     // Software serial for extra pins
#include <Servo.h>                              // Servo h file
#include <LiquidCrystal_I2C.h>                  // liquid crystal display library
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27,16,2);               // defining the lcd
Servo myservo;                                  // create servo object to control the ball drop servo
static const uint8_t PIN_MP3_TX = 12; // Connects to module's RX       // tx line for software serial
static const uint8_t PIN_MP3_RX = 11; // Connects to module's TX      // rx line for software serial
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);
char bluetoothreceivedCommand = 0x00;                    // received command from bluetooth and encoder
int pos = 0;                                    // variable to store the ball drop servo position
#define RPWM_upper 3                            // Pin for Pulse width modulation to upper ball toss motor
#define LPWM_lower 6                            // Pin for Pulse width modulation to lower ball toss motor
#define ballPresent A1                           // Ball present sensor tied to digital pin A1
#define feederMotorpin1  4                     // feeder motor pin 1 on digital pin 4
#define feederMotorpin2  5                     // feeder motor pin 2 on digital pin 5
#define resetPin 10                            // software reset pin
DFRobotDFPlayerMini player;                    // Naming MP3 player as player
byte arrow[] = {0x00, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};      // defines the arrow used on the display menu
int vol = 30;                                  // allows to change the volume of the mp3 player
int dogPlayingwith =3;                         // dog playing with 0 means Jax, 1 Charlie, 2 both
int distance = 0;                              // variable to hold program in while statement until a distance has been entered.





void setup() {
digitalWrite(resetPin, HIGH);                   // sets reset pin high
Serial.begin(9600);                             // Setting the baud rate of the serial communications for the serial panel.  
softwareSerial.begin(9600);                     // Setting baud rate of softwareserial for MP3 player
delay(100);                                     // 100 millisecond delay to allow the baud rates to set.
pinMode(RPWM_upper,OUTPUT);                     // Setting the upper motor control pin 3 to an output
pinMode(LPWM_lower,OUTPUT);                     // Setting the lower motor control pin 6 to an output
pinMode(ballPresent, INPUT);                   // Setting the ballPresent sensor line to an intput.
pinMode(feederMotorpin1, OUTPUT);               // feeder motor pin 1 set to an output
pinMode(feederMotorpin2, OUTPUT);               // feeder motor pin 2 set to an output
pinMode(resetPin, OUTPUT);                      // sets reset pin to an OUTPUT
analogWrite(RPWM_upper, 0);                     // shutting upper motor off
analogWrite(LPWM_lower, 0);                     // shutting lower motor off
myservo.attach(9);                              // attaches the ball drop servo on pin 9 to the servo object
player.begin(softwareSerial);                   // sets Serial for MP3 player
}

void loop() {

  myservo.write(85);                           // close the ball servo to hold the ball.
  while(dogPlayingwith == 3){                  // While statement to hold here until bog you want ot play with is entered.
    Serial.println("dog");                     // Prints the dog you entered.
    Serial.println(dogPlayingwith); 
Serial.println("Enter the dog you want to play with");
  lcd.init();                                 // initialize the lcd 
  lcd.backlight();                            // print the message to the LCD
 lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Enter your dogs");
  lcd.setCursor(1,1);
  lcd.print(" name ");
   delay(300);
if(Serial.available()>0){                       //Checking to see if any commands coming in ifon bluetooth
bluetoothreceivedCommand=Serial.read();         // Putting command received from bluetooth into variable bluetoothreceivedCommand
Serial.println("receiving command");
Serial.write(bluetoothreceivedCommand);         // Writing the received command back out to the bluetooth device
}
if(bluetoothreceivedCommand == 0x6E){
  dogPlayingwith = 0;                           // sets dog to Jax.
}
if(bluetoothreceivedCommand == 0x6F){
  dogPlayingwith = 1;                           // sets dog to Charlie
}
if(bluetoothreceivedCommand == 0x6D){
  dogPlayingwith = 2;                           // sets dog to both
}
if(bluetoothreceivedCommand == 0x6C){           // if received command is hex 0x6C this is a manual feed
  feed();
}
if(bluetoothreceivedCommand == 0x6B){           // if received command is hex 0x6B this is a reset.
  resetnow();
}
  }
   Serial.println("dog");                       // prints dog you are going to play with
    Serial.println(dogPlayingwith); 
  while(distance == 0){                         //holds in while loop until distance you want to throw is entered.
Serial.println("Enter the distance you want to throw");
lcd.init();                                    // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
 lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Enter the throw");
  lcd.setCursor(1,1);
  lcd.print(" distance ");
   delay(300);
//delay(300);
if(Serial.available()>0){                              // Checking to see if any commands coming in on bluetooth
bluetoothreceivedCommand=Serial.read();                // Putting command received from bluetooth into variable bluetoothreceivedCommand
Serial.println("receiving command");
Serial.write(bluetoothreceivedCommand);                  // Writing the received command back out to the bluetooth device
}
if(bluetoothreceivedCommand == 0x71){                    // if received command is hex 0x71 we call subroutine to set speed for ball toss motors to throw 15 feet
  distance = 1;
  fifteenfeet();
}
if(bluetoothreceivedCommand == 0x77){                   // if received command is hex 0x77 we call subroutine to set speed for ball toss motors to throw 25 feet.
  distance = 1;
  twentyfivefeet();
}
if(bluetoothreceivedCommand == 0x65){                   // if received command is hex 0x65 we call subroutine to set speed for ball toss motors to throw 35 feet.
  distance = 1;
  thirtyfivefeet();
}
if(bluetoothreceivedCommand == 0x6C){                  // if received command is hex 0x6C this is a manual feed
  feed();
}
if(bluetoothreceivedCommand == 0x6B){                 // if received command is hex 0x6B this is a reset.
  resetnow();
}

if(Serial.available()>0){                             // Checking to see if any commands coming in on bluetooth
bluetoothreceivedCommand=Serial.read();                // Putting command received from bluetooth into variable bluetoothreceivedCommand
Serial.println("receiving command");
Serial.write(bluetoothreceivedCommand);                  // Writing the received command back out to the bluetooth device
}
if(bluetoothreceivedCommand == 0x6C){                  // if received command is hex 0x6C this is a manual feed
  feed();
}
if(bluetoothreceivedCommand == 0x6B){                 // if received command is hex 0x6B this is a reset.
  resetnow();
}

Serial.print("bluetoothreceivedCommand");       
Serial.println(dogPlayingwith);
delay(200);

}
}


void fifteenfeet(){                                    // This is the routine to set speed of ball toss motors to toss ball 15 feet
  while(distance ==1){
if(analogRead(ballPresent) <= 100){                    // if ball present sensor value is less then threshold do what is in the if statement.
  if(Serial.available()>0){                            // Checking to see if any commands coming in on bluetooth
bluetoothreceivedCommand=Serial.read();                // Putting command received from bluetooth into variable bluetoothreceivedCommand
Serial.println("receiving command");
Serial.write(bluetoothreceivedCommand);               // Writing the received command back out to the bluetooth device
}
if(bluetoothreceivedCommand == 0x6C){                  // if received command is hex 0x6C this is a manual feed
  feed();
}
if(bluetoothreceivedCommand == 0x6B){                 // if received command is hex 0x6B this is a reset.
  resetnow();
}
analogWrite(RPWM_upper, 0);                                               // keeps upper ball toss motor off
analogWrite(LPWM_lower, 0);                                               // keeps lower ball toss motor off
lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("No Ball is");
  lcd.setCursor(1,1);
  lcd.print("present");
   delay(3000);
if(dogPlayingwith == 0){
player.volume(vol);
  player.play(1);                                                         // File 1 is Eli voice saying get the ball Jax.
  delay(2500);
}
if(dogPlayingwith == 1){
  player.volume(vol);
  player.play(2);                                                         // file 2 is Eli saying get the ball Charlie
  delay(2500);
}

if(dogPlayingwith == 2){
  player.volume(vol);
  player.play(3);                                                         // file 3 is Eli saying get the ball boys
  delay(2500);
}
}
if(analogRead(ballPresent) >= 150){          // if ball is present in hopper we feed the dog and praise him for putting the ball in the hopper. we allow time for dog to eat
if(Serial.available()>0){                    // Checking to see if any commands coming in on bluetooth
bluetoothreceivedCommand=Serial.read();                // Putting command received from bluetooth into variable bluetoothreceivedCommand
Serial.println("receiving command");
Serial.write(bluetoothreceivedCommand);                // Writing the received command back out to the bluetooth device
}
if(bluetoothreceivedCommand == 0x6C){                  // if received command is hex 0x6C this is a manual feed
  feed();
}
if(bluetoothreceivedCommand == 0x6B){                 // if received command is hex 0x6B this is a reset.
  resetnow();
}
lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Ball present");
  lcd.setCursor(1,1);
  lcd.print("Throwing 15 ft");
   delay(300);

digitalWrite(feederMotorpin1, HIGH);                                      // to come up to speed and then we move the ball drop servo to the open position to drop the ball into the chute. Then we have
digitalWrite(feederMotorpin2, LOW);
delay(300);                                                               // Eli telling the dog to go get the ball.  Then we close the ball drop servo and turn off the upper and lower ball toss motors.
digitalWrite(feederMotorpin1, LOW);
digitalWrite(feederMotorpin2, HIGH);                                      // We repeat this process for the routines to toss other distances as well.
delay(300);
digitalWrite(feederMotorpin1,LOW);
digitalWrite(feederMotorpin2, LOW);
delay(3000);

if(dogPlayingwith == 0){
  player.volume(vol);
  player.play(4);                                                         // file 4 is Eli saying good boy Jax
  delay(2500);
}
if(dogPlayingwith == 1){
  player.volume(vol);
  player.play(5);                                                         // file 5 is Eli saying good boy Charlie
  delay(2500);
}
if(dogPlayingwith == 2){
  player.volume(vol);
  player.play(6);
  delay(2500);                                                            // file 6 is Eli saying good boys
}                                                                         // Then we turn on the upper and lower ball toss motors to the speed need to throw the ball 15 feet. We allow time for the motors

analogWrite(RPWM_upper, 40);
analogWrite(LPWM_lower, 40);
Serial.println(" I am throwing the ball fifteen feet");
delay(3000);                                                               // Set delay to amount of time needed to bring motors up to speed.
myservo.write(0);                                                         //opens the ball drop servo to drop the ball.
if(dogPlayingwith == 0){
  player.volume(vol);
  player.play(7);                                                         // file 7 is Eli saying get the ball Jax
  delay(2500);
}
if(dogPlayingwith == 1){
  player.volume(vol);
  player.play(8);                                                         // file 8 is Eli saying get the ball Charlie
  delay(2500);
}
if(dogPlayingwith == 2){
  player.volume(vol);
  player.play(9);
  delay(2500);                                                            // file 9 is Eli saying get the ball boys
}        
delay(6000);                                                               // delay to allow time to toss the ball.
myservo.write(90);                                                          // closes the ball drop servo.
analogWrite(RPWM_upper, 0);
analogWrite(LPWM_lower, 0);
}
}
}
void twentyfivefeet(){                                   // This is the routine to throw the ball 25 feet
  while(distance ==1){
if(analogRead(ballPresent) <= 100){         // keeping ball toss motors turned off as long as the ball present switch is low indicating there is no ball in hopper
  // mySerial.listen();
  if(Serial.available()>0){                    // Checking to see if any commands coming in on bluetooth
bluetoothreceivedCommand=Serial.read();                // Putting command received from bluetooth into variable bluetoothreceivedCommand
Serial.println("receiving command");
Serial.write(bluetoothreceivedCommand);                  // Writing the received command back out to the bluetooth device
}
if(bluetoothreceivedCommand == 0x6C){                  // if received command is hex 0x6C this is a manual feed
  feed();
}
if(bluetoothreceivedCommand == 0x6B){                 // if received command is hex 0x6B this is a reset.
  resetnow();
}
lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("No Ball");
  lcd.setCursor(1,1);
  lcd.print("present");
   delay(3000);
analogWrite(RPWM_upper, 0);
analogWrite(LPWM_lower, 0);
if(dogPlayingwith == 0){
player.volume(vol);
  player.play(1);                                                         // File 1 is Eli voice saying get the ball Jax.
  delay(2500);
}
if(dogPlayingwith == 1){
  player.volume(vol);
  player.play(2);                                                         // file 2 is Eli saying get the ball Charlie
  delay(2500);
}

if(dogPlayingwith == 2){
  player.volume(vol);
  player.play(3);                                                         // file 3 is Eli saying get the ball boys
  delay(2500);
}
}
if(analogRead(ballPresent) >= 150){
  if(Serial.available()>0){                    // Checking to see if any commands coming in on bluetooth
bluetoothreceivedCommand=Serial.read();                // Putting command received from bluetooth into variable bluetoothreceivedCommand
Serial.println("receiving command");
Serial.write(bluetoothreceivedCommand);                  // Writing the received command back out to the bluetooth device
}
if(bluetoothreceivedCommand == 0x6C){                  // if received command is hex 0x6C this is a manual feed
  feed();
}
if(bluetoothreceivedCommand == 0x6B){                 // if received command is hex 0x6B this is a reset.
  resetnow();
}
 lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Ball present");
  lcd.setCursor(1,1);
  lcd.print("Throwing 25 ft");
   delay(300);

digitalWrite(feederMotorpin1, HIGH);
digitalWrite(feederMotorpin2, LOW);
delay(300);
digitalWrite(feederMotorpin1, LOW);
digitalWrite(feederMotorpin2, HIGH);
delay(300);
digitalWrite(feederMotorpin1,LOW);
digitalWrite(feederMotorpin2, LOW);
delay(3000);

if(dogPlayingwith == 0){
  player.volume(vol);
  player.play(4);                                                         // file 4 is Eli saying good boy Jax
  delay(2500);
}
if(dogPlayingwith == 1){
  player.volume(vol);
  player.play(5);                                                         // file 5 is Eli saying good boy Charlie
  delay(2500);
}
if(dogPlayingwith == 2){
  player.volume(vol);
  player.play(6);                                                         // file 6 is Eli saying good boys
  delay(2500);
}                                    
delay(3000);
analogWrite(RPWM_upper, 70);                                             //Should be 70 for both upper and lower for throwing ball 25 feet
analogWrite(LPWM_lower, 70);
Serial.println(" I am throwing the ball 25 feet");
delay(3000);                                                               // Set delay to amount of time needed to bring motors up to speed.

myservo.write(0);                                                         //opens the ball drop servo to drop the ball.
if(dogPlayingwith == 0){
  player.volume(vol);
  player.play(7);                                                         // file 7 is Eli saying get the ball Jax
  delay(2500);
}
if(dogPlayingwith == 1){
  player.volume(vol);
  player.play(8);                                                         // file 8 is Eli saying get teh ball Charlie
  delay(2500);
}
if(dogPlayingwith == 2){
  player.volume(vol);
  player.play(9);
  delay(2500);                                                            // file 9 is Eli saying get the ball boys
}        
delay(3000);                                                               // delay to allow time to toss the ball.
myservo.write(90);                                                          // closes the ball drop servo.
analogWrite(RPWM_upper, 0);
analogWrite(LPWM_lower, 0);
}
}
}

void thirtyfivefeet(){
  while(distance ==1){
if(analogRead(ballPresent) <= 100){         // keeping ball toss motors turned off as long as the ball present switch is low indicating there is no ball in hopper
  if(Serial.available()>0){                    // Checking to see if any commands coming in on bluetooth
bluetoothreceivedCommand=Serial.read();                // Putting command received from bluetooth into variable bluetoothreceivedCommand
Serial.println("receiving command");
Serial.write(bluetoothreceivedCommand);                  // Writing the received command back out to the bluetooth device
}
if(bluetoothreceivedCommand == 0x6C){                  // if received command is hex 0x6C this is a manual feed
  feed();
}
if(bluetoothreceivedCommand == 0x6B){                 // if received command is hex 0x6B this is a reset.
  resetnow();
}
lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("No Ball");
  lcd.setCursor(1,1);
  lcd.print("present");
   delay(3000);
analogWrite(RPWM_upper, 0);
analogWrite(LPWM_lower, 0);
if(dogPlayingwith == 0){
player.volume(vol);
  player.play(1);                                                         // File 1 is Eli voice saying get the ball Jax.
  delay(2500);
}
if(dogPlayingwith == 1){
  player.volume(vol);
  player.play(2);                                                         // file 2 is Eli saying get the ball Charlie
  delay(2500);
}

if(dogPlayingwith == 2){
  player.volume(vol);
  player.play(3);                                                         // file 3 is Eli saying get the ball boys
  delay(2500);
}
}
if(analogRead(ballPresent) >= 150){
   //mySerial.listen();
  if(Serial.available()>0){                    // Checking to see if any commands coming in on bluetooth
bluetoothreceivedCommand=Serial.read();                // Putting command received from bluetooth into variable bluetoothreceivedCommand
Serial.println("receiving command");
Serial.write(bluetoothreceivedCommand);                  // Writing the received command back out to the bluetooth device
}
if(bluetoothreceivedCommand == 0x6C){                  // if received command is hex 0x6C this is a manual feed
  feed();
}
if(bluetoothreceivedCommand == 0x6B){                 // if received command is hex 0x6B this is a reset.
  resetnow();
}
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Ball present");
  lcd.setCursor(1,1);
  lcd.print("Throwing 35 ft");
   delay(300);

digitalWrite(feederMotorpin1, HIGH);
digitalWrite(feederMotorpin2, LOW);
delay(300);
digitalWrite(feederMotorpin1, LOW);
digitalWrite(feederMotorpin2, HIGH);
delay(300);
digitalWrite(feederMotorpin1,LOW);
digitalWrite(feederMotorpin2, LOW);
delay(3000);

if(dogPlayingwith == 0){
  player.volume(vol);
  player.play(4);                                                         // file 4 is Eli saying good boy Jax
  delay(2500);
}
if(dogPlayingwith == 1){
  player.volume(vol);
  player.play(5);                                                         // file 5 is Eli saying good boy Charlie
  delay(2500);
}
if(dogPlayingwith == 2){
  player.volume(vol);
  player.play(6);                                                         // file 6 is Eli saying good boys
  delay(2500);
}                                    
delay(3000);
analogWrite(RPWM_upper, 100);                                             //Should be 70 for both upper and lower for throwing ball 25 feet
analogWrite(LPWM_lower, 100);
Serial.println(" I am throwing the ball 35 feet");
delay(3000);                                                               // Set delay to amount of time needed to bring motors up to speed.

myservo.write(0);                                                         //opens the ball drop servo to drop the ball.
if(dogPlayingwith == 0){
  player.volume(vol);
  player.play(7);                                                         // file 7 is Eli saying get the ball Jax
  delay(2500);
}
if(dogPlayingwith == 1){
  player.volume(vol);
  player.play(8);                                                         // file 8 is Eli saying get teh ball Charlie
  delay(2500);
}
if(dogPlayingwith == 2){
  player.volume(vol);
  player.play(9);
  delay(2500);                                                            // file 9 is Eli saying get the ball boys
}        
delay(3000);                                                               // delay to allow time to toss the ball.
myservo.write(90);                                                          // closes the ball drop servo.
analogWrite(RPWM_upper, 0);
analogWrite(LPWM_lower, 0);
}
}
}


void feed(){                                                          // This is the routine for manual feeding dog
 // softwareSerial.listen();
digitalWrite(feederMotorpin1, HIGH);
digitalWrite(feederMotorpin2, LOW);
delay(300);
digitalWrite(feederMotorpin1, LOW);
digitalWrite(feederMotorpin2, HIGH);
delay(300);
digitalWrite(feederMotorpin1,LOW);
digitalWrite(feederMotorpin2, LOW);
Serial.println("I am feeding the dog");
delay(3000);
}

void resetnow(){                                                      // This is the routine to software reset the ball thrower
 // softwareSerial.listen();
 Serial.println("resetting");
  digitalWrite(resetPin, LOW);
  delay(2000);
}
