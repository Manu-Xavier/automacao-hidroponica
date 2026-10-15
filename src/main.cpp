/*
  Autor: Emanuel Xavier
  Data: 21/02/2025

  git:15/10/2026
*/


#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial condutivimetro(10, 11);

//Valor de referencia em dS/cm 1.30

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x20, lcdColumns, lcdRows);

byte teste[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b};

byte condutHex[2] = {0x00, 0x00};

word condutTotal = 0;

const int botao = 13;
const int enableRxTx = 9;

unsigned long recMillis = 0;
unsigned int tempo = 100;

unsigned int retornaCondutividade();

void setup(){
  Serial.begin(9600);
  condutivimetro.begin(9600);

  pinMode(botao, INPUT_PULLUP);
  pinMode(enableRxTx, OUTPUT);

  digitalWrite(enableRxTx, 0);
  
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando.");
  for(int i = 0; i < 10; i++){
    lcd.print(".");
    delay(100);
  }

  lcd.clear();
}

void loop(){
  if(digitalRead(botao) != 1){
    delay(500);
    digitalWrite(enableRxTx, 1);
    condutivimetro.write(teste, sizeof(teste));
    condutivimetro.flush();
    digitalWrite(enableRxTx, 0);
  }

  if(millis() - recMillis >= tempo){
    unsigned int mediaCond = 0;
    for(int i = 0; i < 10; i++){
      mediaCond += retornaCondutividade();
      delay(100);
    }
    mediaCond /= 10;
    unsigned int cond = mediaCond;
    Serial.print(cond); Serial.println(" mS/cm");
  
    char dadosLcd[20] = "\0";
    char texto[7] = " uS/cm";

    itoa(cond, dadosLcd, 10);
    strcat(dadosLcd, texto);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(dadosLcd);

    recMillis = millis();
  } 
}

unsigned int retornaCondutividade(){
  unsigned int condutividade = 0;

  if(condutivimetro.available()){
    byte leitura = 0x00;
    int i = 0;
    int j = 0;

    if(condutivimetro.available() == 9){
      while(condutivimetro.available()){
        leitura = condutivimetro.read();
        if(i == 5 || i == 6){
          condutHex[j] = leitura;
          j++;
        }
        i++;
      }
    }

    else{
      while(condutivimetro.available()){
        leitura = condutivimetro.read();
        delay(20);
      }
    }
  }

  condutTotal = word(condutHex[0], condutHex[1]);

  condutividade = condutTotal;

  //condutividade = random(20000); 

  return condutividade;
}