/*
 * Nome: José Carlos Pereira dos Santos Júnior
 * RA: 155972
 * Trabalho de Graduação Final
 * Smart Campus - Lixeira Inteligente de Pilhas
 * 
 * Arquitetura - End-device Wi-Fi com o uso de Deep Sleep.
 * 
 * Os dados do sensor ultrassonico medem a distância na lixeira,
 * são enviados via MQTT para a Cloud da IBM, sendo possível a visualização
 * em tempo real dos dados
 */
 
// Import de Bibliotecas para utilização do módulo LoRa.
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>


// Import de Bibliotecas MQTT e Wi-Fi
#include <PubSubClient.h>
#include <WiFi.h>
// Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>

// SSID rede Wi-Fi. Configurar previamente
#define SSID "SSID_NAME"

// Senha wi-fi. Configurar previamente.
#define PASSWORD "12345678#"

// Server MQTT disponibilizado pela IBM
#define MQTT_SERVER "quickstart.messaging.internetofthings.ibmcloud.com"

// Nome do tópico que irá receber os dados do sensor.
#define TOPIC_NAME "iot-2/evt/status/fmt/json"

// Conexão dos pinos para o sensor ultrasonico
#define PIN_TRIGGER   12
#define PIN_ECHO      13


#define RST 14  // GPIO14 RESET
#define DI00 26 // GPIO26 IRQ(Interrupt Request)

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5    

//Altura máxima lixeira.
#define ALTURA_MAXIMA 45

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(PIN_TRIGGER, PIN_ECHO);

// Cliente WiFi que o MQTT irá utilizar para se conectar
WiFiClient wifiClient;

//Cliente MQTT,é necessário a URL do Server e Porta, alem do cliente Wi-Fi.
PubSubClient client(MQTT_SERVER, 1883, wifiClient);

// Id único que será identificado pelo server da IBM.
const String QUICK_START = "d:quickstart:arduino:";
const String DEVICE_ID = "DEVICE_ID"; // Inserir mac_address dispositivo.
const String CLIENT_ID =  QUICK_START + DEVICE_ID;

// Variável responsável por medir distância sensor ultrassônico.
unsigned int data = 0;

// Variavel responsável por medir distãncia sensor em porcentagem.
int porcentagem = 0;

// String que será enviada ao topico MQTT
String msg = "";


/** Funções de setup **/

// Função que fará a conexão ao server da IBM Cloud.
void connectMQTTServer() {
  Serial.println("Connecting to MQTT Server...");
  // Se conecta ao ID dentro do server que foi definido.
  while(!client.connect(CLIENT_ID.c_str())) {
    // Ocorreu algum erro na conexão
    Serial.println("error = ");
    Serial.println(client.state());
    
  }
  // Conexão estabelecida ao server MQTT.
  Serial.println("connected");
  
}

// Função que fará a conexão ao Wi-Fi definido.
void setupWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(SSID);
  
  WiFi.begin(SSID, PASSWORD);
  // Espera até que a conexão seja bem sucedida
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  // Caso tenha chegado à esse ponto à conexão Wi-Fi foi estabelecida.
  Serial.println("");
  Serial.println("WiFi connected");
}


void setup(){

  delay(500);
  Serial.begin(115200);
  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  
  // Se conectando à rede Wi-Fi e ao server MQTT
  setupWiFi();
  connectMQTTServer();
  
   // Fazendo a leitura do sensor ultrassonico
  long microsec = ultrasonic.timing();
  data = ultrasonic.convert(microsec, Ultrasonic::CM);
  
  porcentagem = transforma_porcentagem(data);
  Serial.println("Distancia ultrassônico: ");
  Serial.println(porcentagem);
  msg = createJsonString(porcentagem);
  
  if (client.connected()){
    client.publish(TOPIC_NAME, msg.c_str());
  }
  
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  delay(100);
  client.disconnect();
  WiFi.disconnect();
  delay(100);
  LoRa.end();
  LoRa.sleep();
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  Serial.println("Going to sleep now");
  delay(100);
  esp_deep_sleep_start(); 
}


/** Loop principal **/

void loop(){
  // Não alcançará o Loop devido o modo deep sleep.
}


//Função responsável por criar
//um Json com os dados lidos
String createJsonString(int porcentagem_lixeira) {
  String json = "{";
    json+= "\"d\": {";
      json+="\"distancia\":";
      json+=String(porcentagem_lixeira);
    json+="}";
  json+="}";
  return json;
}

// Função responsável por fazer a euivalencia de cm para porcentagem.
int transforma_porcentagem(unsigned int distancia_cm){
  int porcentagem = 0;
  porcentagem = (int)(100 - (distancia_cm*100)/ALTURA_MAXIMA);
  if (porcentagem < 0){
    porcentagem = 0; 
  } 
  return porcentagem;
}


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  :
    {
      Serial.println("Wakeup caused by external signal using RTC_IO");
      delay(2);
    } break;
    case 2  :
    {
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      delay(2);
    } break;
    case 3  :
    {
      Serial.println("Wakeup caused by timer");
      delay(2);
    } break;
    case 4  :
    {
      Serial.println("Wakeup caused by touchpad");
      delay(2);
    } break;
    case 5  :
    {
      Serial.println("Wakeup caused by ULP program");
      delay(2);
    } break;
    default :
    {
      Serial.println("Wakeup was not caused by deep sleep");
      delay(2);
    } break;
  }
}
