/* 
 *  My Blynk Auth Token : PnmuIlSc6iRNS39aZMggUSous2Va_ohG
 *  Email work with this : ddtankfight@gmail.com
 *  URL updating (PUT) from IFTTT: http://188.166.206.43/PnmuIlSc6iRNS39aZMggUSous2Va_ohG/update/V0  -  FAN
 *                                http://188.166.206.43/PnmuIlSc6iRNS39aZMggUSous2Va_ohG/update/V1   -  LIGHT
 *                                
 *  Command Collections:
 *  FAN:
- turn on my fan
- it is too hot
- my fan on please

- turn off my fan
- it is too cold
- my fan off please

 *  LIGHT:
- turn on my light
- it is too dark
- my light on please

- turn off my light
- it is too bright
- my light off please

 *  TURN ON/OFF ALL:
- working time
- i need to work
- turn on all my things

- sleeping time
- i am going to outside
- turn off all my things

*/

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Ticker.h>

Ticker ticker;

#define VPIN_BUTTON_1    V0     // FAN
#define VPIN_BUTTON_2    V1     // LIGHT

char auth[] = "PnmuIlSc6iRNS39aZMggUSous2Va_ohG";

int switch_1 = 13;  //D7
int switch_2 = 15;  //D8

int control_1 = 0;   //D3
int control_2 = 2;   //D4
int smartConfig_light = 14; //D5

unsigned int longPressTime = 5000;
boolean smartConfigStart =  false;
boolean stateDEVICE_control_1 = false;
boolean stateDEVICE_control_2 = false;

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  stateDEVICE_control_1 = param.asInt();
  digitalWrite(control_1, stateDEVICE_control_1);
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  stateDEVICE_control_2 = param.asInt();
  digitalWrite(control_2, stateDEVICE_control_2);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("MY WORKSPACE PROJECT - Google Home_Blynk_IFTTT");
  
  pinMode(switch_1, INPUT);
  pinMode(switch_2, INPUT);
  pinMode(control_1, OUTPUT);
  pinMode(control_2, OUTPUT);
  pinMode(smartConfig_light, OUTPUT);

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);
  delay(8000);
  
  if(!WiFi.isConnected())
  {
    startSmartConfig();
  }
  else
  {
    digitalWrite(smartConfig_light, HIGH);
    Serial.println("WIFI CONNECTED");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  
  //Blynk.begin(auth, ssid, pass);
  Blynk.config(auth);
}

void loop()
{
  Blynk.run();
  boolean check_Button_1 = isButton_Click(switch_1);
  boolean check_Button_2 = isButton_Click(switch_2);

  if (check_Button_1)
    {
      Serial.println("\nSwitch 1 - Clicked!");
      digitalWrite(control_1, !stateDEVICE_control_1);
      Blynk.virtualWrite(VPIN_BUTTON_1, !stateDEVICE_control_1);
      stateDEVICE_control_1 = !stateDEVICE_control_1;
    }

  if (check_Button_2)
    {
      Serial.println("\nSwitch 2 - Clicked!");
      digitalWrite(control_2, !stateDEVICE_control_2);
      Blynk.virtualWrite(VPIN_BUTTON_2, !stateDEVICE_control_2);
      stateDEVICE_control_2 = !stateDEVICE_control_2;
    }
  
  delay(50);
}

int isButton_Click(int GPIO_to_read)
{
    int out = 0;
    unsigned int timer;
    timer = millis();
    while (digitalRead(GPIO_to_read) == 1)
    {
        delay(20);
        if (millis() - timer > longPressTime)
        {
          Serial.println("Starting smart config ...");
          startSmartConfig();
        }
        else
        out = 1;
    }
    return out;
}

void blinking()
{
  bool state = digitalRead(smartConfig_light);
  digitalWrite(smartConfig_light,!state);
}

//SmartConfig:
boolean startSmartConfig(){
  int t = 0;
  Serial.println("Smart Config Start");
  WiFi.beginSmartConfig();
  delay(500);
  ticker.attach(0.1, blinking);
  while(WiFi.status() != WL_CONNECTED){
    t++;
    Serial.print(".");
    delay(500);
    if(t > 120){
      Serial.println("Smart Config Fail");
    smartConfigStart = false;
    ticker.attach(0.5, blinking);
      delay(3000);
      exitSmartConfig();
      return false;
    }
  }
  smartConfigStart = true;
  Serial.println("WIFI CONNECTED");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.SSID());
  exitSmartConfig();
  return true;
}

void exitSmartConfig(){
  WiFi.stopSmartConfig();
  ticker.detach();
  digitalWrite(smartConfig_light, LOW);
}
