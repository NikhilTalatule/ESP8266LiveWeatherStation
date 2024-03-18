#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#ifndef STASSID
#define STASSID "Fast-2.4G"
#define STAPSK  "Superfast10"
#endif
#define BLYNK_TEMPLATE_ID "TMPLqack202H"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#include <Adafruit_Sensor.h>
#define BLYNK_PRINT Serial
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
#define I2C_SCL 12
#define I2C_SDA 13

float dst, bt, bp, ba;
char dstmp[20], btmp[20], bprs[20], balt[20];
bool bmp085_present = true;
const char* ssid = STASSID;
const char* password = STAPSK;
char auth[] = "d6dRgrGb1Bu1Bj3R9Qt7njfaM07FbBfA";
#define DHTPIN 2
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

  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V10, bp);
  Blynk.virtualWrite(V11, ba);
  Blynk.virtualWrite(V12, bt);
  Blynk.virtualWrite(V13, dst);
  Blynk.virtualWrite(V14, dp);
}
void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
     Serial.begin(115200);
  Blynk.begin(auth, ssid, password);
  dht.begin();
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(10);
  timer.setInterval(1000L, sendSensor);
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
   ArduinoOTA.setPassword("nikhil");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
}
