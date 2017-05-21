
#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"
int motionPine = 5;
int moveu = 0;
int estadoMovimento = LOW;
int valorPinoMotion = 0;
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
#define DHTPIN 2     // qual pino digital esta conectado
#define DHTTYPE DHT11   // DHT 11 eh o tipo do sensor que estou usando
//#define DHTTYPE DHT22  
EthernetServer server(12345);
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  pinMode(motionPine,INPUT);
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  // comeca a conexao com a ethernet e o servidor
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  dht.begin();
}


void loop() {
  EthernetClient client = server.available();
  valorPinoMotion = digitalRead(motionPine);
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float f = dht.readTemperature(true);
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Nao conseguiu ler o sensor dht!");
        return;
      }
     if(valorPinoMotion == HIGH){
        if(estadoMovimento == LOW){
          Serial.println("Movimento detectado!!");
          moveu = 1;
          estadoMovimento = HIGH;
          }
        }
        else{
          if(estadoMovimento == HIGH){
            Serial.println("Movimento terminou");
            moveu = 0;
            estadoMovimento = LOW;
            }
          } 
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Connection: close");  // a conexao fecha apos receber a resposta
          client.println("Refresh: 2"); // atualiza a pagina a cada cinco segundos
          client.println();
          float hif = dht.computeHeatIndex(f, h);
          float hic = dht.computeHeatIndex(t, h, false);
          client.println("[");
          client.print("{\"Umidade\": ");
          client.print(h);
          client.print("},");  
          client.print("{\"Temperatura\": ");
          client.print(t);
          client.print("},");
          client.print("{\"Movimento\": ");
          client.print(moveu);
          client.print("}");
          client.println("]");
          delay(200);
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    // dando tempo para que o navegador recdba os dados
    delay(200);
    // fecha a conexao
    client.stop();
    Serial.println("client disconnected");
  }
}
