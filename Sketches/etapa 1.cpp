/*
  Autor: Emanuel Xavier
  Data: 21/02/2025
*/


#include <Arduino.h>
#include <SoftwareSerial.h>

//Criação do objeto para a biblioteca software serial
SoftwareSerial mySerial(10, 11);

//Consulte o datasheet para mais informações a respeito da comunicação
//Vetores de bytes para comunicação rs485
byte teste[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b}; //Sequencia de bytes obtidos através do datasheet do condutivimetro

//Entradas e saídas
const int botao = 13;
const int enRxTx = 9;

void setup() {
  //Velocidade de comunicação
  Serial.begin(9600);
  mySerial.begin(9600);

  //Confiuguração das entradas e saídas
  pinMode(botao, INPUT_PULLUP);
  pinMode(enRxTx, OUTPUT);

  //Inicialização das saídas
  digitalWrite(enRxTx, 0); //0 - habilita o RX
                           //1 - habilita o TX
  //Por default deixaremos em 0 para que o microcontrolador sempre "escute" o dispositivo rs485.
  //Quando quisermos enviar alguma coisa ao condutivímetro, colocamos esse pino em HIGH,
  //enviamos o comando e logo em seguida voltamos o pino para 0 novamente.
  //Isso garante que o controlador receba as informações solicitadas ao condutivímetro
}

void loop() {
  //Caso o botão seja pressionado, entra na rotina abaixo
  if(digitalRead(botao) != 1){
    delay(500);
    digitalWrite(enRxTx, 1);
    mySerial.write(teste, sizeof(teste));
    mySerial.flush();
    digitalWrite(enRxTx, 0);
  }

  delay(100); //Espera um tempo para o dispositivo enviar a resposta da solicitação acima

  //Caso algum dado tenha chegado na Serial virtual, entra na rotina abaixo
  if(mySerial.available()){
    while(mySerial.available()){
      Serial.print(mySerial.read(), HEX);
      Serial.print("-");
    }
    Serial.println();
  }
}