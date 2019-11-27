/* 
 * Compila apenas se GATEWAY estiver definido na aba de setup.
 * O Gateway terá a função de receber as mensagens via LoRa do node.
 * Além disso, os dados deve ser enviado via MQTT para a Cloud da IBM.
 * Desta maneira será possível o monitoramento em tempo real.
 */
   
#ifdef GATEWAY

// Import de Bibliotecas MQTT e Wi-Fi
#include <PubSubClient.h>
#include <WiFi.h>

// SSID rede Wi-Fi. Configurar com nome rede Wi-Fi.
#define SSID "SSID_NAME"

// Senha wi-fi, configurar previamente.
#define PASSWORD "PASSWORD"

// Server MQTT disponibilizado pela IBM
#define MQTT_SERVER "quickstart.messaging.internetofthings.ibmcloud.com"

// Nome do tópico que irá receber os dados do sensor.
#define TOPIC_NAME "iot-2/evt/status/fmt/json"

//Altura máxima lixeira.
#define ALTURA_MAXIMA 45

// Id único que será identificado pelo server da IBM.
const String QUICK_START = "d:quickstart:arduino:";
const String DEVICE_ID = "DEVICE_ID";//inserir mac_address dispositivo.
const String CLIENT_ID =  QUICK_START + DEVICE_ID;

// Cliente WiFi que o MQTT irá utilizar para se conectar
WiFiClient wifiClient;

//Cliente MQTT,é necessário a URL do Server e Porta, alem do cliente Wi-Fi.
PubSubClient client(MQTT_SERVER, 1883, wifiClient);

// Valor que será lido do sensor ultrassônico.
unsigned int data = 0;

// Variavel responsável por medir distãncia sensor em porcentagem.
int porcentagem = 0;


// Variável que representa a última mensagem enviada ao server MQTT.
String last_msg = "{\"d\": {\"distancia\":0}}";

// Variável que representa uma nova mensagem recebida.
String new_msg = "";
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
  Serial.println("Connecting to ");
  Serial.println(SSID);

  // O gateway se conectará à rede Wi-Fi informada.
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
  Serial.begin(115200);
  //Chama a configuração inicial do LoRa
  setupLoRa();
  // Se conectando à rede Wi-Fi e ao server MQTT
  setupWiFi();
  connectMQTTServer();
  
}


/** Loop principal **/

void loop(){

  
  // Verificando se há pacotes para serem processados.
  new_msg = receive();
  
  // Verificando se há conexão à rede Wi-fi foi perdida
  if (WiFi.status()!= WL_CONNECTED){
    // Se sim reconecta à rede Wi-Fi
    setupWiFi();
    // Reconecta ao server MQTT
    connectMQTTServer();
  // Caso esteja conectado ao WiFi
  }else if (client.connected()){
    
     // Caso exista uma nova mensagem recebida, está será publicada no Server MQTT.
    if (new_msg != ""){
      // Publicando no tópico do server a nova mensagem recebida.
      client.publish(TOPIC_NAME, new_msg.c_str());
      Serial.println("Publish message: ");
      Serial.println(new_msg);
      // Atualiza a variável last_message
      last_msg = new_msg;
    } else {
      // Publicando no tópico do server a última mensagem, mantendo o dado sem atualização.
      client.publish(TOPIC_NAME, last_msg.c_str());
      Serial.println("Publish message: ");
      Serial.println(last_msg);
    }
    // Mantém a conexão ao server MQTT.
    client.loop();
  }
  delay(500);
}

/** Funções auxiliares **/

// Verificando se o LoRa node enviou algum pacote com a info do sensor.
// Se encontrou algum dado, já é publicado ao server MQTT.
String receive(){
  // Mensagem que foi recebida
  String msg ="";
  // Tentativa de leitura do pacote
  int packetSize = LoRa.parsePacket();
  // Deve ter o tamanho minimo maior que o SET.
  if (packetSize > SETDATA.length()){
    String received = "";
    // Armazena os dados do pacote em uma String.
    for(int i=0; i<SETDATA.length(); i++){
      received += (char) LoRa.read();
    }

    // Se o cabeçalho contém SET, isso indica que é um node que o Gateway estava esperando.
    if(received.equals(SETDATA)){
      // Fazendo a leitura dos dados.
      LoRa.readBytes((uint8_t*)&data, sizeof(data));
      porcentagem = transforma_porcentagem(data);
      // Criando o json que será enviado ao server MQTT posteriormente.
      msg = createJsonString(porcentagem);
      return msg;
    } else {
      // Se o dado recebido não está no padrão enviado pelo node. Retorna uma string vazia
      return msg;
    }
  } else{
    // Caso não tenha recebido nenhum dado envia uma string vazia.
    return msg;
  }
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

#endif
