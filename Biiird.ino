#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

int menuItem=0;
int menuItemOld=0;
int menuLevel=0;

char* menu0[] = {"Main Menu", "Flight Control", "Waypoints", "Power"};
char* menu1[] = {"Flight Mode", "Up/Down Mode", "Move Mode", "Metrics"};
char* menu2[] = {"Drop Control", "Open/Drop", "", "Metrics"};
char* menu3[] = {"Waypoints", "Waypoint 1", "Waypoint 2", "Metrics"};
char* menu4[] = {"Power", "Controller Power", "UAV Power", "Metrics"};

char** menuitems[] = {menu0, menu1, menu2, menu3, menu4};

// Set the LCD I2C address, En, Rw, Rs, d4, d5, d6, d7, backlight, light state
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

// Creat a set of new characters
const uint8_t charBitmap[][8] = {
  { 
    0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0             }
  ,
  { 
    0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0             }
  ,
  { 
    0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0             }
  ,
  { 
    0x0, 0xc, 0x12, 0x12, 0xc, 0, 0, 0x0             }
  ,
  { 
    0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0x0             }
  ,
  { 
    0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0x0             }
  ,
  { 
    0x0, 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0x0             }
  ,
  { 
    0x0, 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0x0             }

};

const int ledblue = 13;
const int switchup = 4;
const int switchok = 2;
const int switchdown= 3;
const int switchback=5; //new switch

int lastsw = 0; //keep track of last switch pressed

String command=""; //Stores response of the HC906 Bluetooth device
int light = 0; //LCD backlight light timer

void setup()
{
  
  
  //LED
  pinMode(ledblue, OUTPUT); 

  //Switches
  pinMode(switchup, INPUT_PULLUP); 
  pinMode(switchok, INPUT_PULLUP); 
  pinMode(switchdown, INPUT_PULLUP); 
  pinMode(switchback, INPUT_PULLUP);


  // i2c LCD
  int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));

  lcd.begin(20,4);  // initialize the i2c LCD display

  //set up some special characters for fun
  for ( int i = 0; i < charBitmapSize; i++ )
  {
    lcd.createChar ( i, (uint8_t *)charBitmap[i] );
  }

  // put something on the display
  lcd.setBacklight(HIGH);
  lcd.home();                   // go home
  lcd.print("Vic UAV Controller");  
  //lcd.setCursor( 0, 1 );        // go to the next line
  //lcd.print(" ");
  delay(1000);

  // hardware serial
  Serial.begin(57600);   
  Serial.print("Hello LCD test");
}

// main program
void loop()
{
  
  // make the backlight turn off after a short period
  light--;
  if (light<0){
    lcd.setBacklight(LOW);
    light = 0;
  }

  lcd.home ();
  // Do a little animation by writing to the same location
  //for ( int i = 2; i < 4; i++ )
  //{
   // lcd.setCursor ( 0, i );
    //for ( int j = 0; j < 20; j++ )
    //{
      //lcd.print (char(random(7)));
    //}
  //}
  

  // check the switches and the serial port for messages
  
  for (int i = 0; i<300; i++){
    delay (1);

    // put any serial data onto the display
    if (Serial.available()){     
      delay(100); // wait for rest of message
      while(Serial.available()) {
        // While there is more to be read, keep reading.             
        command += (char)Serial.read();
      }

      // Switch on the backlight for a while
      light=20;
      lcd.setBacklight(HIGH);

      lcd.setCursor (0, 1);
      for ( int j = 0; j < 20; j++ )
      {
        lcd.print (' ');
      }

      // LED on
      digitalWrite(ledblue, HIGH);
      
      // repeat message to the hardware terminal
      Serial.println(command);

      // send message to the display
      lcd.setCursor (0, 1);
      lcd.print(command);
      command = ""; // No repeats

      delay(500);
      //LED off
      digitalWrite(ledblue, LOW);
    }

    // deal with the switches
    
    int sw = checkSwitches();
    if (lastsw != sw){
      lastsw = sw;
      
      if (sw == switchback){
        lcd.setCursor (0, 0);
        lcd.print("back pressed");
      }
      
     if (sw == switchup){
        menuItemOld=menuItem;
            menuItem--;
            
            if (menuItem<=0){menuItem=3;}
            lcd.setCursor (0, menuItem);
            lcd.print(">");
            menuLevel=menuItem;
            if (menuItemOld!=0){
            lcd.setCursor (0, menuItemOld);
            lcd.print(" ");
            }
      }

     if (sw == switchdown){
       // Serial.println("Switch down pressed");
        //lcd.setCursor (0, 1);
        //lcd.print("DOWN pressed        "); 
        //light=20;
        //lcd.setBacklight(HIGH);
           menuItemOld=menuItem;
            menuItem++;
            
            if (menuItem==4){menuItem=1;}
            lcd.setCursor (0, menuItem);
            lcd.print(">");
            menuLevel=menuItem;
            if (menuItemOld!=0){
            lcd.setCursor (0, menuItemOld);
            lcd.print(" ");
            }
      }

      if (sw == switchok){
        Serial.println("Switch ok pressed");
        lcd.clear();
        int i=0;
        for(i;i<4;i++){
          if(i==0){
            lcd.setCursor (0,0);
          }
          else{  
             lcd.setCursor (1, i);
          }
          lcd.print(menuitems[menuLevel][i]);
          Serial.println(menuLevel);
          //Serial.println(i);
          //Serial.println(menuitems[menuLevel][i]);
        }
         
        light=20;
        lcd.setBacklight(HIGH);
                
              
      }
    }
  }
}

/**
 * checks the switches and returns the one that is pressed.
 */
int checkSwitches(){
  // use the switch to change ports
  int val = digitalRead(switchup);
  if (val==0) return switchup;

  val = digitalRead(switchok);
  if (val==0) return switchok;

  val = digitalRead(switchdown);
  if (val==0) return switchdown;
  
  val = digitalRead(switchback);
  if (val==0) return switchback;

  return 0;
}







