#include <Wire.h>
#include <DHT.h>
#include "FirebaseESP8266.h"	
#include <ESP8266WiFi.h>
//#include "MAX30100_PulseOximeter.h"

//#define REPORTING_PERIOD_MS    1000

//PulseOximeter pox;

//uint32_t tsLastReport = 0;

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
int btnstate, SpO2, BPM; 


// void onBeatDetected()
// {
//     Serial.println("Beat!");
// }

void setup()
{

  Serial.begin(115200);

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

  // if(!pox.begin()) {
  //     Serial.println("FAILED");
  //     for(;;);
  // } else {
  //     Serial.println("SUCCESS");
  // }
  // pox.setOnBeatDetectedCallback(onBeatDetected);

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}

void sensorUpdate(){

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float g = analogRead(A0);

 
   if (Firebase.getInt(firebaseData, "/tempmax")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      limtmax = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/tempmin")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      limtmin = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/hummax")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      limhmax = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/hummin")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      limhmin = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/gasmax")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      limgmax = firebaseData.to<int>();
    
    }

  } else {
    Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getInt(firebaseData, "/gasmin")) {

      if (firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
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


  if(h>limhmax || h<limhmin || t>limtmax || t<limtmin || g>limgmax || g<limgmin){

    digitalWrite(D8, HIGH); 
    delay(1000);  
    digitalWrite(D8, LOW);  

  }else{
    digitalWrite(D8, LOW);  

  }

  if (Firebase.setFloat(firebaseData, "/temperature", t))
  {}
  else
  {
    Serial.println("FAILED");
  
  }

  if (Firebase.setFloat(firebaseData, "/humidity", h))
  {}
  else
  {
    Serial.println("FAILED");

  }
  if (Firebase.setFloat(firebaseData, "/gas", g))
  {}
  else
  {
    Serial.println("FAILED");
  
  }
    Serial.println("");
  
}

void btnUpdate(){

 int btn = digitalRead(D6);
 //Serial.println(btn);

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

// void spo2Update(){

//   pox.update();
 
//     BPM = pox.getHeartRate();
//     SpO2 = pox.getSpO2();

//     if (millis() - tsLastReport > REPORTING_PERIOD_MS)
//     {
//         Serial.print("Heart rate:");
//         Serial.print(BPM);
//         Serial.print("   ");
//         Serial.print("SpO2:");
//         Serial.print(SpO2);
//         Serial.println("%");
        
//         tsLastReport = millis();

//          if (BPM != 0 && SpO2 !=0) {
//             Firebase.setInt(firebaseData, "/spo2", SpO2);
//             Firebase.setInt(firebaseData, "/bpm", BPM);
           
//         }
//     }

// }

void loop() {

  //  spo2Update();
    btnUpdate();
    sensorUpdate();
  
}

