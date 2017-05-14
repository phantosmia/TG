
#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22  
EthernetServer server(80);
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  dht.begin();
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        
        //delay(2000);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float f = dht.readTemperature(true);
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          float hif = dht.computeHeatIndex(f, h);
          float hic = dht.computeHeatIndex(t, h, false);
          client.println("[");
          client.print("{\"Umidade\": ");
          client.print(h);
          client.print("},");  
          client.print("{\"Temperatura\": ");
          client.print(t);
          client.print("}");
          client.println("]");
         /* for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
               int sensorReading = analogRead(analogChannel);
               client.print("{\"key\": ");
               client.print(analogChannel);                           // The key for Analog pin 0 (A0) is equal to 0   eg.  "key":0
               client.print(", \"value\": ");
               client.print(sensorReading);               // The value is equal to the Analog reading from that pin.  eg. "value":300
               if(analogChannel==5){
                client.print("}");                       // The last value will end with a bracket (without a comma)
              }else {
                client.print("},");                      // All other values will have a comma after the bracket.
              }
          }
          client.println("]"); */
          
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

