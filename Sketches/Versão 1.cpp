/*
  Autor: Emanuel Xavier
  Data: 21/02/2025

  Nessa versão, a comunicação RS485 é estabelecida com o condutivímetro.
  Os dados são lidos na UART do microcontrolador (ATMEGA328P), e logo após,
  são tratados.
  Atenção especial para o "ajuste" sugerido pelos professores em que o valor coletado
  do condutivímetro é dividio por 10 (linha 165), para que seja retornado o valor
  correto. Esse "ajuste" foi pensado para um comportamento linear do condutivímetro.
*/


#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Criação do objeto para a biblioteca software serial
SoftwareSerial mySerial(10, 11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Consulte o datasheet para mais informações a respeito da comunicação
//Vetores de bytes para comunicação rs485
byte teste[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b}; //Sequencia de bytes obtidos através do datasheet do condutivimetro

byte condutHex[2] = {0x00, 0x00}; //Vetor com duas posições para armazenar os dois bytes referentes a condutividade lidos do sensor

word condutTotal = 0; //Variável para combinar o valor dos bytes contidos no vetor condutHex para converter em um valor inteiro

//Entradas e saídas
const int botao = 13;
const int enRxTx = 9;

void setup() {
  //Velocidade de comunicação
  Serial.begin(9600);
  mySerial.begin(9600);

  lcd.init();
  lcd.clear();

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
    byte leitura = 0x00;
    int i = 0;
    int j = 0;

    if(mySerial.available() == 9){
      while(mySerial.available()){
        leitura = mySerial.read();
        if(i == 5 || i == 6){
          condutHex[j] = leitura;
          j++;
        }
        i++;
      }
    }
    
    else {
      while(mySerial.available()){
        leitura = mySerial.read();
        //Serial.print(mySerial.read(), HEX);
        //Serial.print("-");
      }
    }

    condutTotal = word(condutHex[0], condutHex[1]);

    /*lcd.clear();
    lcd.setCursor(0, 0);
    char dadosLcd[16] = "\0";
    char texto[7] = " uS/cm";

    //Converte um número int em um array de char
    itoa(condutTotal, dadosLcd, 10); //itoa(número int, destino, base numérica)
    strcat(dadosLcd, texto); //concatena o array dadosLcd com o array texto

    lcd.print(dadosLcd);*/

    Serial.print(condutTotal/10);
    Serial.print(" uS/cm");
    Serial.println();
  }
}