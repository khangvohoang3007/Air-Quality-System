//Paste your Blynk config here
#define BLYNK_TEMPLATE_ID "...."
#define BLYNK_TEMPLATE_NAME "SENSOR NODE1"
#define BLYNK_AUTH_TOKEN "....."
//////////////////////////////////////////////////
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h> 

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include <DHT.h>
#include "MQ135.h"

#include <BlynkSimpleEsp8266.h>

#define DHTPIN 14     
#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 32, &Wire, -1);

const int pushButton = 2;     
const int ledPin =  0;        
int displayState = 0;
int blinkOled = 0;
int ledState = LOW; 
unsigned long startTime = 0;


float t, h;
float air_quality;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "type here";  // type your wifi name
char pass[] = "type here";  // type your wifi password

BlynkTimer timer;

// Data of temp icon
static const unsigned char PROGMEM drop_iconTemp[] = 
{
  B00000011, B10000000,
  B00000110, B11000000,
  B00000100, B01000000,
  B00000101, B01000000,
  B00000101, B01000000,
  B00000101, B01000000,
  B00000101, B01000000,
  B00000101, B01000000,
  B00001101, B01100000,
  B00001001, B00100000,
  B00011011, B10110000,
  B00010011, B10010000,
  B00011011, B10110000,
  B00001000, B00100000,
  B00001110, B11100000,
  B00000011, B10000000
};

// Data of HUD icon
static const unsigned char PROGMEM drop_iconHud[] = 
{
  B00000010, B00000000,
  B00000110, B00000000,
  B00001111, B00000000,
  B00011111, B10000000,
  B00011111, B10000000,
  B00111111, B11000000,
  B00111111, B11000000,
  B01111111, B11100000,
  B01111111, B11100000,
  B01111111, B11100000,
  B01111111, B11100000,
  B01111110, B11100000,
  B01111100, B01100000,
  B00111110, B11000000,
  B00011111, B10000000,
  B00001111, B00000000
};

// Data of airQuality icon
static const unsigned char PROGMEM drop_iconMq135[] = 
{
  B00000000, B00000000,
  B00001110, B00111000,
  B00011111, B01111100,
  B00111111, B11111110,
  B01111111, B11111111,
  B01111110, B00111111,
  B01111110, B00111111,
  B01111000, B00001111,
  B01111000, B00001111,
  B00111000, B00001110,
  B00011110, B00111100,
  B00001110, B00111000,
  B00000111, B11110000,
  B00000011, B11100000,
  B00000001, B11000000,
  B00000000, B10000000
};


//Sub-Function Delay Using Timer
void delay_millis(unsigned long delayTime) 
{
  unsigned long startTime = millis();  
  
  if (millis() - startTime >= delayTime) 
  {
    //Do nothing
  }
}


//Sub-Function to Read MQ-135 and DHT11 Sensors & Send to Blynk
void sendSensor()
{

  h = dht.readHumidity();  //Read the Humidity
  t = dht.readTemperature(); // Read the Temperature

  //Blynk set up for Temp and Hud
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  //Read MQ135 and send to Blynk
  MQ135 gasSensor = MQ135(A0);
  air_quality = gasSensor.getPPM();
  Blynk.virtualWrite(V2, air_quality);
}


//Sub-Function debouncing button
void debounce_button()
{     
  if (!digitalRead(pushButton)) //If button was pressed
  {

    delay_millis(20); //waiting...
    if (!digitalRead(pushButton)) //Check button again
    {
      //change displayState to display on OLED
      displayState++;
      if(displayState==4) displayState=0;
      
      while(!digitalRead(pushButton)); //Wait button not pressed
    }
  }
}

void setup() 
{

  //Set baud rate
  Serial.begin(115200);

  //Button and Led setup
  pinMode(pushButton, INPUT);  
  pinMode(ledPin, OUTPUT);     

  //displayState = 0 to let oled always off when we turn sensor node ON
  displayState = 0;

  //Initialize OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  //Display connecting to "WIFI NAME"
  display.clearDisplay(); 
  display.setCursor(0,0);  
  display.setTextSize(1); 
  display.setTextColor(WHITE); 
  display.println("Connecting to"); 
  display.setTextSize(1); 
  display.setCursor(0,15); 
  display.print(ssid);
  display.display();

  //Connect Wifi
  WiFi.begin(ssid, pass);

  //Check if wifi are connected
  while (WiFi.status() != WL_CONNECTED)
  {
    //Delay to see if Wifi has done connection, if not ...
    delay(500);

    //Blink "..." on OLED
    if (blinkOled == 0) 
    {
      display.setTextColor(WHITE);
      display.setCursor(80,0); 
      display.print("...");
      display.display();

      blinkOled=1;
    } 
    else 
    {

      display.setTextColor(BLACK);
      display.setCursor(80,0); 
      display.print("...");
      display.display();

      blinkOled=0;
    }
  }


  //Connect to Blynk sever
  Blynk.begin(auth, ssid, pass);
  //Initialize DHT11
  dht.begin();
  timer.setInterval(30000L, sendSensor);

  //Check LED warning
  digitalWrite(ledPin,HIGH);

  //Annoucing successfully connected to WiFi
  display.clearDisplay();
  display.setCursor(25,0);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("WIFI CONNECTED");
  display.display();

  delay(4000);
}


void loop() 
{
  //Initialize Blynk
  Blynk.run();
  timer.run();

  //Call all Sub-Function
  debounce_button();
  sendSensor();


  if(air_quality > 150) //If air_quality value larger than 150
  {
    
    //Using timer to Blink LED, to intervene when the air_quality value changes
    if (millis() - startTime >= 200) 
    {
      // save the last time you blinked the LED
      startTime = millis(); 

      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) 
      {
        ledState = HIGH;
      } 
      else 
      {
        ledState = LOW;
      }

      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
    }


    //Send the event to Blynk
    Blynk.logEvent("pollution_alert","BAD AIR");
    
    //Display BIG "BAD AIR" text on OLED and it's ppm value
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(24, 0); ///x,y
    display.println("BAD AIR"); 
    display.setTextSize(1);
    display.drawBitmap(20, 15, drop_iconMq135, 16, 16, WHITE); 
    display.setCursor(40, 20); ///x,y 
    display.print(": ");

    display.print(air_quality);
    display.setTextSize(1);
    display.println(" PPM");
    display.display();
    delay_millis(500); 

    //Let displayState=3 to display again AIR QUALITY screen 
    //If air_quality value are not in danger anymore
    displayState = 3;
    
  }
  else
  {
    
    //Turn off warning LED
    digitalWrite(ledPin,LOW);

    //OFF the oled
    if (displayState == 0) 
    {
      //Clear all OLED
      display.clearDisplay(); 
      display.display();
    }  

    //Display Temperature 
    if (displayState == 1) 
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(35, 0); ///x,y
      display.println("TEMPERATURE");
      display.drawBitmap(30, 15, drop_iconTemp, 16, 16, WHITE); //Draw icon 16x16
      display.setCursor(45, 20); ///x,y
      display.print(": ");
      display.print(t);
      display.setTextSize(1);
      display.cp437(true);
      display.write(167);
      display.setTextSize(1.5);
      display.println(" C");
      display.display();
      delay_millis(500); 
    }  

    //Display Hudmidity 
    if (displayState == 2) 
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(40, 0); ///x,y
      display.println("HUMIDITY"); 
      display.drawBitmap(30, 15, drop_iconHud, 16, 16, WHITE); //Draw icon 16x16
      display.setCursor(45, 20); ///x,y
      display.print(": "); 
      display.print(h);
      display.print(" %");
      display.setTextSize(1);
      display.display(); 
      delay_millis(500);
    }  

    //Display air quality  
    if (displayState == 3) 
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(31, 0); ///x,y
      display.println("AIR QUALITY"); 
      display.drawBitmap(20, 15, drop_iconMq135, 16, 16, WHITE); //Draw icon 16x16
      display.setCursor(40, 20); ///x,y 
      display.print(": ");

      display.print(air_quality);
      display.setTextSize(1);
      display.println(" PPM");
      display.display();
      delay_millis(500); 
    }  
  }
}