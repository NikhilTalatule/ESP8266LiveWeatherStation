#define BLYNK_TEMPLATE_ID "TMPLqack202H"         //blynk template id.
#define BLYNK_DEVICE_NAME "Quickstart Template"  //blynk template id name.
#include <Adafruit_Sensor.h>                     //sensor library.
#define BLYNK_PRINT Serial                       // shows the blynk logo and connection satus.
#include <DHT.h>                                 //dht11 library
#include <ESP8266WiFi.h>                         // It provide node mcu with wifi routines for connection to wifi & etc.
#include <BlynkSimpleEsp8266.h>                  //this is a blynk library to handle connectivity,authentication in cloude, and commands processing between Blynk app, Cloud, and hardware.
#include <Wire.h>                                //It is used to communicate with I2C/TWI devices
#include <Adafruit_BMP085.h>                     //bmp180 sensor library

Adafruit_BMP085 bmp;
#define I2C_SCL 12  //gpio pin 12
#define I2C_SDA 13  //gpio pin 13

float dst, bt, bp, ba;
char dstmp[20], btmp[20], bprs[20], balt[20];
bool bmp085_present = true;

char auth[] = "your authorization token";  //blynk authorization token
char ssid[] = "your wifi name";            //wifi name
char pass[] = "wifi password";             //wifi password
#define DHTPIN 2                           //gpio pin 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);  //Defining the pin and the dhttype
BlynkTimer timer;
void sendSensor() {

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature();


  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  double gamma = log(h / 100) + ((17.62 * t) / (243.5 + t));
  double dp = 243.5 * gamma / (17.62 - gamma);

  float bp = bmp.readPressure();
  float ba = bmp.readAltitude();
  float bt = bmp.readTemperature();
  float dst = bmp.readSealevelPressure();

  Blynk.virtualWrite(V5, h);  //virtual pin in blynk app of data stream
  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V10, bp);
  Blynk.virtualWrite(V11, ba);
  Blynk.virtualWrite(V12, bt);
  Blynk.virtualWrite(V13, dst);
  Blynk.virtualWrite(V14, dp);
}
void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  dht.begin();                   //initiate dht 11
  Wire.begin(I2C_SDA, I2C_SCL);  // initiate the Wire library and also we need to initiate the serial communication because we will use the Serial Monitor to show the data from the sensor
  delay(10);
  timer.setInterval(1000L, sendSensor);  //send sensor data ever 1 sec.
}

void loop() {
  Blynk.run();
  timer.run();
}