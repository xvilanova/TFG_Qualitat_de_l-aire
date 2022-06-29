//Programas
#include "00_Secrets.h"


//Llibreries
//
#include <WiFi.h>
#include <Adafruit_AHT10.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <SoftwareSerial.h>
#include "PMS.h"
#include <Adafruit_ADS1X15.h>

int PM1 = 0;
int PM10 = 0;
int PM25 = 0;
int Temperatura = 0;
int Humitat = 0;
int NO2_raw = 0;
int Voltage_no2 = 0;
bool Dades = false;

Adafruit_AHT10 aht;// AHT10
Adafruit_ADS1115 ads;//ADS1115

SoftwareSerial pmsSerial(3, 1);

PMS pms(pmsSerial);
PMS::DATA data;


char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;



void setup() {
  Serial.begin(9600);  //Initialize serial
  pmsSerial.begin(9600);

  ////////////// WiFi //////////////////
  WiFi.mode(WIFI_STA);

  ////////////// ThingSpeak //////////////////
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  ////////////// AHT10 //////////////////
  if (! aht.begin()) {
    Serial.println("Could not find AHT10? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 found");

  /////////////ADS1115////////////


  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

}

void loop() {

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  if (pms.read(data))
  {
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);
    //return PM_AE_UG_1_0
    PM1 = data.PM_AE_UG_1_0;
    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);
    PM25 = data.PM_AE_UG_2_5;
    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);
    PM10 = data.PM_AE_UG_10_0;
    Serial.println();

    Dades = true;
  }
  if (Dades == true  )
  {
    AHT10();
    Ads();
    // set the fields with the values
    ThingSpeak.setField(1, Temperatura);
    ThingSpeak.setField(2, NO2_raw);
    ThingSpeak.setField(3, PM25);
    ThingSpeak.setField(4, PM10);
   


    Dades = false;
    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));

    }

    delay(30000); // Wait 20 seconds to update the channel again

  }
}


//void Raw_to_ug/m3() {
//
////1 raw to voltage
//Voltage_no2
//
//
//}
