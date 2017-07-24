

/*
 *  CONECTADO UMA ARDUINO UNO A REDE WI FI COM UM ESP 8266 ENVIANDO DADOS COM LINGUAGEM MQTT.

  Node red recebe tópico hello world  da placa ARDUINO e ESP8266 usado apenas para conectar o Arduino na rede WI FI.
  Este código tem a intenção de ajudar as pessoas a entender melhor como utilizar a linguagem MQTT                         
                                                                                                                          
  Neste exemplo, usei uma placa Arduino UNO original (Atmega 328) e um ESP8266-12-E para conectar o Arduino                 
  em uma rede WI FI.                                                                                                       
  Neste caso, estou utilizando a bibloteca Softwareserial que permite usar ports do Arduino para comunicação               
  deixando livre os ports TX RX para usar sua porta serial para debug.                                                     
  As portas usadas neste exemplo são a 2 e a 3, (RX, TX) para isso elas são declaradas como abaixo no codigo.              
  O ESP8266 deve estar com o firmware AT instalado para poder funcionar neste modo.                                        
  É importante ajustar a velocidade do ESP8266 para 9600 usando este comando AT: AT+UART_DEF=9600, 8, 1, 0, 3             
  Com este comando você ajusta a velocidade e e os demais parâmetros, controle de fluxo, paridade etc.                     
                                                                                                                        
  No meu caso usei um Broker (Servidor MQTT) próprio instalado no meu computador, mas     você pode usar diversos.
  Públicos como mosquito, mosca, etc, mas para fins didáticos eu aconselho baixar e instalar seu próprio Broker.          
  eu utilizei o HIVEMQ que pode ser baixado gratuitamente do site desta empresa http://www.hivemq.com.
 
  IMPORTANTE os ports do ESP funcionam com tensão de 3V3, já os do Arduino com 5V,                                                                                                                                                            
  Portanto é necesario adaptar esta tensão, eu utilizei um conversor de nível, mas você pode usar por exemplo                                                                                                                        
  um divisores resistivo se não souber calcular existem diversas calculadoras online.                                                                                                                     
  Também usei o node-red para observar em uma interface gráfica o resultado de envio e recebimento.                        
                                                                                                                           
  contato@carloskwiek.com.br                                                                                               
                                                                                                                           
  Eng.Carlos kwiek - www.carloskwiek.com.br * 
 * 
 */


/*
 *  CONECTADO UMA ARDUINO UNO A REDE WI FI COM UM ESP 8266 ENVIANDO DADOS COM LINGUAGEM MQTT.

  Node red recebe tópico hello world  da placa ARDUINO e ESP8266 usado apenas para conectar o Arduino na rede WI FI.
  Este código tem a intenção de ajudar as pessoas a entender melhor como utilizar a linguagem MQTT                         
                                                                                                                          
  Neste exemplo, usei uma placa Arduino UNO original (Atmega 328) e um ESP8266-12-E para conectar o Arduino                 
  em uma rede WI FI.                                                                                                       
  Neste caso, estou utilizando a bibloteca Softwareserial que permite usar ports do Arduino para comunicação               
  deixando livre os ports TX RX para usar sua porta serial para debug.                                                     
  As portas usadas neste exemplo são a 2 e a 3, (RX, TX) para isso elas são declaradas como abaixo no codigo.              
  O ESP8266 deve estar com o firmware AT instalado para poder funcionar neste modo.                                        
  É importante ajustar a velocidade do ESP8266 para 9600 usando este comando AT: AT+UART_DEF=9600, 8, 1, 0, 3             
  Com este comando você ajusta a velocidade e e os demais parâmetros, controle de fluxo, paridade etc.                     
                                                                                                                        
  No meu caso usei um Broker (Servidor MQTT) próprio instalado no meu computador, mas     você pode usar diversos.
  Públicos como mosquito, mosca, etc, mas para fins didáticos eu aconselho baixar e instalar seu próprio Broker.          
  eu utilizei o HIVEMQ que pode ser baixado gratuitamente do site desta empresa http://www.hivemq.com.
 
  IMPORTANTE os ports do ESP funcionam com tensão de 3V3, já os do Arduino com 5V,                                                                                                                                                            
  Portanto é necesario adaptar esta tensão, eu utilizei um conversor de nível, mas você pode usar por exemplo                                                                                                                        
  um divisores resistivo se não souber calcular existem diversas calculadoras online.                                                                                                                     
  Também usei o node-red para observar em uma interface gráfica o resultado de envio e recebimento.                        
                                                                                                                           
  contato@carloskwiek.com.br                                                                                               
                                                                                                                           
  Eng.Carlos kwiek - www.carloskwiek.com.br * 
 * 
 */
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>

IPAddress server(192,168,0,6);
char ssid[] = "REDE";           // your network SSID (name)
char pass[] = "SENHA";           // your network password
int status = WL_IDLE_STATUS;   // the Wifi radio's status
long lastMsg = 0;
char msg[50];
int value = 0;
int LED = 13;

// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial soft(2,3); // RX, TX
void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

   // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);

  }
 

 Serial.println("You're connected to the network");

  //connect to MQTT server
  client.setServer(server, 1883);
  client.setCallback(callback);
}

//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
  Serial.print((char)payload[i]);
  }
  Serial.println();
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}
