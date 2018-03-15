/**
 * Simple example to demo the esp-link MQTT client
 */

#include <ELClient.h>
#include <ELClientCmd.h>
#include <ELClientMqtt.h>

   

// Initialize a connection to esp-link using the normal hardware serial port both for
// SLIP and for debug messages.
ELClient esp(&Serial, &Serial);

// Initialize CMD client (for GetTime)
ELClientCmd cmd(&esp);

// Initialize the MQTT client
ELClientMqtt mqtt(&esp);

// Callback made from esp-link to notify of wifi status changes
// Here we just print something out for grins
void wifiCb(void* response) {
  ELClientResponse *res = (ELClientResponse*)response;
  if (res->argc() == 1) {
    uint8_t status;
    res->popArg(&status, 1);

    if(status == STATION_GOT_IP) {
      Serial.println("WIFI CONNECTED");
    } else {
      Serial.print("WIFI NOT READY: ");
      Serial.println(status);
    }
  }
}

bool connected;

// Callback when MQTT is connected
void mqttConnected(void* response) {
  Serial.println("MQTT connected!");
//  mqtt.subscribe("/esp-link/1");
  mqtt.subscribe("/hello/world/test");
  mqtt.subscribe("/igor/home/hall/hallsensor1");
   mqtt.subscribe("/igor/home/kitchen/kitchensensor1");
   mqtt.subscribe("/igor/home/door/doorsensor1")'
  //mqtt.subscribe("/esp-link/2", 1);
  //mqtt.publish("/esp-link/0", "test1");
  connected = true;
}

// Callback when MQTT is disconnected
void mqttDisconnected(void* response) {
  Serial.println("MQTT disconnected");
  connected = false;
}

// Callback when an MQTT message arrives for one of our subscriptions
void mqttData(void* response) {
  ELClientResponse *res = (ELClientResponse *)response;

  Serial.print("Received: topic=");
  String topic = res->popString();
  Serial.println(topic);

  Serial.print("data=");
  String data = res->popString();
  Serial.println(data);
  
     
     //pinMode(13, OUTPUT);// устанавливаем ножку 13(светодиод) как выход 
     //boolean LampRED=0;//устанавлиаваем лампочку в негорящую позицию 
     //data=int(data);
     if(data == "1")
     {
     digitalWrite(13,HIGH);
        mqtt.publish("/hello/world/led", "Led is on!");
        Serial.println("LED is off now!");
     }
     else
     {
     digitalWrite(13,LOW);
        mqtt.publish("/hello/world/led", "Led is off!");
        Serial.println("LED is on now!");
     }

  mqtt.publish("/hello/world/reaction", "GOT SIGNAL!!!");
}

void mqttPublished(void* response) {
  Serial.println("MQTT published");
}

void setup() {
  

     
  pinMode(13, OUTPUT);
  pinMode(15, INPUT);
  Serial.begin(57600);
  Serial.println("EL-Client starting!");

  // Sync-up with esp-link, this is required at the start of any sketch and initializes the
  // callbacks to the wifi status change callback. The callback gets called with the initial
  // status right after Sync() below completes.
  esp.wifiCb.attach(wifiCb); // wifi status change callback, optional (delete if not desired)
  bool ok;
  do {
    ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
    if (!ok) Serial.println("EL-Client sync failed!");
  } while(!ok);
  Serial.println("EL-Client synced!");
  
 
  // Set-up callbacks for events and initialize with es-link.
  mqtt.connectedCb.attach(mqttConnected);
  mqtt.disconnectedCb.attach(mqttDisconnected);
  mqtt.publishedCb.attach(mqttPublished);
  mqtt.dataCb.attach(mqttData);
  mqtt.setup();

  //Serial.println("ARDUINO: setup mqtt lwt");
  //mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

  Serial.println("EL-MQTT ready");
}

static int count;
static uint32_t last;

 
  

void loop() {
  esp.Process();

  if (connected && (millis()-last) > 4000) {
    Serial.println("publishing");
    char buf[12];

    itoa(count++, buf, 10);
    mqtt.publish("/esp-link/1", buf);

    itoa(count+99, buf, 10);
    mqtt.publish("/hello/world/arduino", buf);
    
                
    uint32_t t = cmd.GetTime();
    Serial.print("Time: "); Serial.println(t);
     
     // Опрашиваем датчик температуры на 15 пине.
     val = analogRead(15);     
     Serial.println(val);
     mqtt.publish("/home/watertemp", val);

    last = millis();
  }
}
