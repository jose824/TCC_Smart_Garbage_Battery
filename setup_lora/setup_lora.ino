/*
 * Nome: José Carlos Pereira dos Santos Júnior
 * RA: 155972
 * Trabalho de Graduação Final
 * Smart Campus - Lixeira Inteligente de Pilhas.
 * 
 * Arquitetura - End-Device LoRa + Gateway
 * 
 * Os dados são enviados de um ESP32 configurado como o device, para o outro configurado
 * como o gateway. Este enviará os dados para a IBM Cloud, utilizando Wi-Fi e MQTT.
 */
 
// Import de Bibliotecas para utilização do módulo LoRa.
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>


// Esta linha deve ser comentada ou descomentada quando for compilar o Gateway ou o  node LoRa
#define GATEWAY

#define RST 14  // GPIO14 RESET
#define DI00 26 // GPIO26 IRQ(Interrupt Request)

#define BAND 915E6 //Frequência do radio - exemplo : 433E6, 868E6, 915E6

//Constante que o node LoRa enviará junto ao pacote do sensor.
//Isso faz com que o Gateway identifique o dado e suba ao servidor em Cloud.
const String SETDATA = "set";

// Configuração do módulo LoRa. Será aplicada as mesmas configurações ao Node e ao Gateway.
void setupLoRa(){ 
  //Inicializa a comunicação
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI00);

  // Aguarda até que o LoRa seja inicializado.
  if (!LoRa.begin(BAND)){
    while (1);
  }

  //Ativa o CRC
  LoRa.enableCrc();
  //Ativa o recebimento de pacotes
  LoRa.receive();
}
