/*
 * This is an example on how to use Espalexa alongside an ESP8266WebServer.
 */ 
#include <Espalexa.h>
 #ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WebServer.h> //if you get an error here please update to ESP32 arduino core 1.0.0
#else
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif

#include <Stepper.h>

#define D0 16
#define D1 4
#define D2 5
#define D3 0

Stepper myStepper = Stepper(2038, D0, D1, D2, D3);//Connect your stepper here!

int Position = 0;
int AltePosition = 0;
int Steps = 0;
int counter;
int Hoch;
int Runter;

// prototypes
boolean connectWifi();

//callback functions
void firstLightChanged(uint8_t brightness);

// Change this!!! AND line 84 AND line 117
const char* ssid = "your wifi name";
const char* password = "your wifi password";

boolean wifiConnected = false;

Espalexa espalexa;
#ifdef ARDUINO_ARCH_ESP32
WebServer server(80);
#else
ESP8266WebServer server(80);
#endif

void setup()
{

  
  Serial.begin(115200);



    myStepper.setSpeed(10);

pinMode(D0, OUTPUT);
pinMode(D1, OUTPUT);
pinMode(D2, OUTPUT);
pinMode(D3, OUTPUT);



  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    server.on("/", HTTP_GET, [](){
    server.send(200, "text/plain", "This is an example index page your server may send.");
    });
    server.on("/test", HTTP_GET, [](){
    server.send(200, "text/plain", "This is a second subpage you may have.");
    });
    server.onNotFound([](){
      if (!espalexa.handleAlexaApiCall(server.uri(),server.arg(0))) //if you don't know the URI, ask espalexa whether it is an Alexa control request
      {
        //whatever you want to do with 404s
        server.send(404, "text/plain", "Not found");
      }
    });

    // Define your devices here.
    espalexa.addDevice("YOUR BLINDS NAME", firstLightChanged); //simplest definition, default state off

    espalexa.begin(&server); //give espalexa a pointer to your server object so it can use your server instead of creating its own
    //server.begin(); //omit this since it will be done by espalexa.begin(&server)
  } else
  {
    while (1)
    {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}
 
void loop()
{
   //server.handleClient() //you can omit this line from your code since it will be called in espalexa.loop()
   espalexa.loop();
   delay(1);
}

//our callback functions
void firstLightChanged(uint8_t brightness) {

  




if (brightness < 255 && brightness >= 3) {
                                          Serial.println(""); 
                                          Serial.print("Brightness = ");
                                          Serial.println(brightness);
                                          Position = (brightness*(29551/252));// 29551 is the number of steps that my stepper has to do, to let the blinds all the way down. Change this too if your blinds are not on the right spot at 100%
                                          Serial.print("Position = ");
                                          Serial.println(Position);
                                          }
else{
     digitalWrite(D0, LOW);
     digitalWrite(D1, LOW);
     digitalWrite(D2, LOW);
     digitalWrite(D3, LOW);
     }

if (Position != AltePosition){
                              Steps = -(Position - AltePosition);
                              AltePosition = Position;  
                              Serial.print("Steps = ");
                              Serial.println(-Steps);
                              
for (counter = 0; counter < Steps; ++counter) {
                                              myStepper.step(1);   
                                              //ESP.wdtFeed();
                                              espalexa.loop();
                                              }

for (counter = 0; counter > Steps; --counter) {
                                               myStepper.step(-1);   
                                              // ESP.wdtFeed();
                                               espalexa.loop();
                                               }   
                              } 


if(counter = Steps){
                    digitalWrite(D0, LOW);
                    digitalWrite(D1, LOW);
                    digitalWrite(D2, LOW);
                    digitalWrite(D3, LOW);
                    }
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  delay(100);
  return state;
}
