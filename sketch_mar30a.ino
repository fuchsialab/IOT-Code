#include <Wire.h>
#include <DHT.h>
#include "FirebaseESP8266.h"	
#include <ESP8266WiFi.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000

PulseOximeter pox;

uint32_t tsLastReport = 0;

#define FIREBASE_HOST "iotweather-f233f-default-rtdb.firebaseio.com/" 
#define FIREBASE_AUTH "mBkvm7ZeFvWePM0mSYBUcdgj3k80TUK9tVs2uozL"
#define WIFI_SSID "MJ"
#define WIFI_PASSWORD "good54321"

#define DHTPIN D4
#define DHTTYPE    DHT11
DHT dht(DHTPIN, DHTTYPE);

FirebaseData firebaseData;
FirebaseData ledData;
FirebaseJson json;

float limtmax, limtmin, limhmax, limhmin, limgmax, limgmin ;
int btnstate;


void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{

  Serial.begin(9600);

  pinMode(D8, OUTPUT);
  pinMode(D6, INPUT);

  dht.begin();
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if (!pox.begin()) {
    Serial.println("MAX30100 was not found. Please check the wiring/power.");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }

  pox.setOnBeatDetectedCallback(onBeatDetected);

}

void sensorUpdate(){

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float g = analogRead(A0);
  float spo2, heart; 



   if (Firebase.getInt(firebaseData, "/tempmax")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      Serial.println(firebaseData.to<int>());
      limtmax = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/tempmin")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      Serial.println(firebaseData.to<int>());
      limtmin = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/hummax")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      Serial.println(firebaseData.to<int>());
      limhmax = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/hummin")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      Serial.println(firebaseData.to<int>());
      limhmin = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/gasmax")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      Serial.println(firebaseData.to<int>());
      limgmax = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/gasmin")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      Serial.println(firebaseData.to<int>());
      limgmin = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }


  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C  ,"));
  Serial.print(f);
  Serial.println(F("°F  ")); 
  Serial.print("Gas Content: ");
  Serial.print(g); 

  pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

      heart = pox.getHeartRate();
      spo2 = pox.getSpO2();

      if (Firebase.setFloat(firebaseData, "/spo2", spo2)){
        Serial.println("PASSED");
        Serial.println("PATH: " + firebaseData.dataPath());
        Serial.println("TYPE: " + firebaseData.dataType());
        Serial.println("ETag: " + firebaseData.ETag());
        Serial.println("------------------------------------");
        Serial.println();
      }
      else{
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }

      
      Serial.print("Heart rate:");
      Serial.print(heart);
      Serial.print("bpm / SpO2:");
      Serial.print(spo2);
      Serial.println("%");

      tsLastReport = millis();
  }


  if(h>limhmax || h<limhmin || t>limtmax || t<limtmin || g>limgmax || g<limgmin){

    digitalWrite(D8, HIGH); 
    delay(1000);  
    digitalWrite(D8, LOW);  

  }else{
    digitalWrite(D8, LOW);  

  }


  if (Firebase.setFloat(firebaseData, "/temperature", t))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (Firebase.setFloat(firebaseData, "/humidity", h))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  if (Firebase.setFloat(firebaseData, "/gas", g))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  
}

void btnUpdate(){

 int btn = digitalRead(D6);
 Serial.println(btn);

 if (Firebase.getInt(firebaseData, "/buttonalarm")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      btnstate = firebaseData.to<int>();
    
    }

  }

    if (btn == LOW){
    //     Serial.println("btn off");
      
    }else{

      if(btnstate == LOW){
        
        Firebase.setInt(firebaseData, "/buttonalarm", btn);
          
      }
    }

}


void loop() {
    btnUpdate();
    sensorUpdate();
   // delay(500);
}

