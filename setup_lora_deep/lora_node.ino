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

  
  delay(500);
  Serial.begin(115200);
  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  // Configuração inicial do LoRa
  setupLoRa();
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
  // Escreve os dados do sensor no pacote
  LoRa.write((uint8_t*)&data, sizeof(data));
  //Finaliza e envia o pacote
  LoRa.endPacket();
  // Habilitando o modo deep sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  delay(100);
  LoRa.end();
  LoRa.sleep();
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  Serial.println("Going to sleep now");
  delay(100);
  esp_deep_sleep_start(); 
  

}

void loop(){
  

  // Espera 5s para o envio de mais um pacote
  delay(5000);

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
#endif
