/* 
 * Compila apenas se GATEWAY estiver definido na aba de setup.
 * O LoRa node terá como função fazer a leitura do nível da lixeira,
 * através do sensor HC-SR04, e enviar via LoRa na frequência definida.
 * Será montado um pacote de dados que será identificado pelo gateway.
 */
 
#ifndef GATEWAY

// Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>

// Conexão dos pinos para o sensor ultrasonico
#define PIN_TRIGGER   12
#define PIN_ECHO      13

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(PIN_TRIGGER, PIN_ECHO);

// Variável responsável por medir distância sensor ultrassônico.
unsigned int data = 0;

void setup(){
  
  Serial.begin(115200);
  // Configuração inicial do LoRa
  setupLoRa();

}

void loop(){
  
  // Fazendo a leitura do sensor ultrassonico
  long microsec = ultrasonic.timing();
  data = ultrasonic.convert(microsec, Ultrasonic::CM);
  
  Serial.print("Distancia ultrassônico: ");
  Serial.println(data);
  Serial.println("Criando pacote para envio");
  // Cria o pacote para envio
  LoRa.beginPacket();
  // Inserindo o cabeçalho esperado
  LoRa.print(SETDATA);
  // Escreve os dados do senosor no pacote
  LoRa.write((uint8_t*)&data, sizeof(data));
  //Finaliza e envia o pacote
  LoRa.endPacket();
  // Espera 5s para o envio de mais um pacote
  delay(5000);

}

#endif
