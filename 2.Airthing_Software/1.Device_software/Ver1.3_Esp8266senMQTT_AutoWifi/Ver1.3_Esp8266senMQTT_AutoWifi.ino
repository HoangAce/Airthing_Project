#include <PubSubClient.h>
#include <WiFiManager.h>

// Information of MQTT Broker
#define mqtt_server "27.72.63.36"
#define mqtt_topic_pub "demo"
#define mqtt_topic_sub "demo"
#define mqtt_user "duynn"
#define mqtt_pwd "Duy@2292"

const uint16_t mqtt_port = 1883; //Port of CloudMQTT

// Address ID of device
#define device_id 1

long lastMsg = 0; //Variable that stores message start time for MQTT
char msg[100]; //Array that stores the value of the message sent to MQTT
int rc = 0;    //Variable that counts the number of device reconnections to MQTT

//The variables stores the device's sensor data, with the initial value being a good indicator of the air
int temp = 30, humidity = 45, VOC = 126, CO2= 201, PM10 = 75, PM25 = 13;                           

//Declare device type with library
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    // put your setup code here, to run once:
  Serial.begin(115200);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //sets timeout until configuration portal gets turned off useful to make it all retry or go to sleep in 180 seconds
  wifiManager.setConfigPortalTimeout(180);
  
  //fetches ssid and pass and tries to connect 
  //if it does not connect it starts an access point with the specified name here  "AirMonitoring", "password" and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("AirMonitoring", "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  } 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
}

//The function callback to receive data
void callback();
//The function reconnect reconnects when the connection is lost with MQTT Broker
void reconnect();
//The  function random performs random air data results under standard conditions
void randomData();

void loop() {
  //Every 60 seconds since sending a message
  long now = millis();
  if (now - lastMsg > 60000) {
     //Checking connection
    if (!client.connected()) {
      //Make connection and check until finished
      reconnect();
    }
    //Perform to get and send data to MQTT when the connection is successful
    client.loop();
    lastMsg = now;
    randomData();
    snprintf (msg, 100, "{\"id\":%d, \"temp\":%d, \"humidity\":%d, \"VOC\":%d, \"CO2\":%d, \"PM10\":%d, \"PM25\":%d}", device_id, temp, humidity, VOC, CO2, PM10, PM25);
    client.publish(mqtt_topic_pub, msg); 
    Serial.print("Publish message: ");
    Serial.println(msg);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  Serial.println("Attempting MQTT connection:");
  //Wait for connection
  while (!client.connected()) {
    //Make connection with mqtt user and pass
    if (!client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) {
      Serial.print("failed, rc=");
      rc++;
      Serial.print(rc);
      Serial.println(" try again in 5 seconds");
      // Wait 5s
      delay(5000);
      if(rc > 9)
      {
        ESP.restart();
        rc = 0;
      }
    }
  }
}
void randomData(){
 //Random Temp
  if(temp < 20){
    temp = temp + random(-1, 6);
    if(temp < 0)
      temp = 1;
  }
  else if(20 <= temp and temp < 28){
    temp = temp + random(-2, 5);
  }
  else if(28 <= temp and temp < 37){
    temp = temp + random(-3, 4);
  }
  else if(37 <= temp and temp < 42){
    temp = temp + random(-4, 3);
  }
  else if(42 <= temp){
    temp = temp + random(-5, 2);
    if(60 < temp)
      temp = 59;
  }

 //Random Humidity
  if(humidity < 25){
    humidity = humidity + random(-1, 6);
    if(humidity < 0)
      humidity = 1;
  }
  else if(25 <= humidity and humidity < 30){
    humidity = humidity + random(-2, 5);
  }
  else if(30 <= humidity and humidity < 60){
    humidity = humidity + random(-3, 4);
  }
  else if(60 <= humidity and humidity < 70){
    humidity = humidity + random(-4, 3);
  }
  else if(70 <= humidity){
    humidity = humidity + random(-5, 2);
    if(99 < humidity)
      humidity = 98;
  }

 //Random VOC
  if(VOC < 250){
    VOC = VOC + random(-1, 4) * 25;
    if(VOC < 0)
      VOC = 26;
  }
  else if(250 <= VOC and VOC < 2000){
    VOC = VOC + random(-2, 3) * 25;
  }
  else if(2000 <= VOC){
    VOC = VOC + random(-3, 2) * 25;
    if(2500 < VOC)
      VOC = 2474;
  }

 //Random CO2
  if(CO2 < 400){
    CO2 = CO2 + random(-1, 5) * 50;
    if(CO2 < 0)
      CO2 = 51;
  }
  else if(400 <= CO2 and CO2 < 1000){
    CO2 = CO2 + random(-2, 4) * 50;
  }
  else if(1000 <= CO2 and CO2 < 2000){
    CO2 = CO2 + random(-3, 3) * 50;
  }
  else if(2000 <= CO2){
    CO2 = CO2 + random(-4, 2) * 50;
    if(5000 < CO2)
      CO2 = 4949;
  }

  //Random PM10
  if(PM10 < 150){
    PM10 = PM10 + random(-1, 7) * 7;
    if(PM10 < 0)
      PM10 = 8;
  }
  else if(150 <= PM10 and PM10 < 250){
    PM10 = PM10 + random(-2, 6) * 7;
  }
  else if(250 <= PM10 and PM10 < 350){
    PM10 = PM10 + random(-3, 5) * 7;
  }
  else if(350 <= PM10 and PM10 < 420){
    PM10 = PM10 + random(-4, 4) * 7;
  }
  else if(420 <= PM10 and PM10 < 600){
    PM10 = PM10 + random(-5, 3) * 7;
  }
  else if(600 <= PM10){
    PM10 = PM10 + random(-6, 2) * 7;
    if(700 < PM10)
      PM10 = 692;
  }

   //Random PM25
  if(PM25 < 25){
    PM25 = PM25 + random(-1, 7) * 3;
    if(PM25 < 0)
      PM25 = 4;
  }
  else if(25 <= PM25 and PM25 < 50){
    PM25 = PM25 + random(-2, 6) * 3;
  }
  else if(50 <= PM25 and PM25 < 80){
    PM25 = PM25 + random(-3, 5) * 3;
  }
  else if(80 <= PM25 and PM25 < 150){
    PM25 = PM25 + random(-4, 4) * 3;
  }
  else if(150 <= PM25 and PM25 < 250){
    PM25 = PM25 + random(-5, 3) * 3;
  }
  else if(250 <= PM25){
    PM25 = PM25 + random(-6, 2) * 3;
    if(300 < PM25)
      PM25 = 296;
  }
}
