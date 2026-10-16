/*
  Autor: Emanuel Xavier
  Data: 21/02/2025

  git:15/10/2026

  v1.1.1
  Necessário revisar a aquisição dos dados do sensor na função retornaCondutividade()!!!
*/


#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>

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
unsigned int cond = 0;

const int analogReadB1 = A0;
const int analogReadB2 = A1;

const int bomba1 = A2;
const int bomba2 = A3;
const int habilitaBombeamento = 12;
const int habilitaIrrigacao = 7;
const int ligaRele = 6;

unsigned long recMillis = 0;
const unsigned int tempo = 1000;

uint16_t setPointB1 = 0;
uint16_t setPointB2 = 0;

unsigned int retornaCondutividade();
void iniciaLcd();
void configIo();

void setup(){
  Serial.begin(9600);
  condutivimetro.begin(9600);

  configIo();

  iniciaLcd();
  
  wdt_enable(WDTO_8S);
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
    cond = retornaCondutividade();
    Serial.print(cond); Serial.println(" uS/cm");

    char dadosLcd[20] = "\0";
    char texto[7] = "uS/cm";

    itoa(cond, dadosLcd, 10);
    strcat(dadosLcd, texto);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor: ");
    lcd.print(dadosLcd);

    setPointB1 = map(analogRead(analogReadB1), 0, 1023, 0, 40000);
    setPointB2 = map(analogRead(analogReadB2), 0, 1023, 0, 40000);

    lcd.setCursor(0, 1);
    lcd.print("Lim.inf: ");
    lcd.print(setPointB1);
    lcd.print(" uS/cm");
    lcd.setCursor(0, 2);
    lcd.print("Lim.sup: ");
    lcd.print(setPointB2);
    lcd.print(" uS/cm");

    Serial.println();
    Serial.print("SetPoint B1: "); Serial.print(setPointB1); Serial.println(" uS/cm");
    Serial.print("SetPoint B2: "); Serial.print(setPointB2); Serial.println(" uS/cm");
    Serial.println();

    recMillis = millis();
  } 


if(digitalRead(habilitaBombeamento) == false){
    if(cond < setPointB1){
      digitalWrite(bomba1, 1);
      digitalWrite(bomba2, 0);
    }

    else if(cond > setPointB1){
      digitalWrite(bomba1, 0);
      digitalWrite(bomba2, 1);
    }

    else{
      digitalWrite(bomba1, 0);
      digitalWrite(bomba2, 0);
    }
  }

  else{
    digitalWrite(bomba1, 0);
    digitalWrite(bomba2, 0);
  }

  if(digitalRead(habilitaIrrigacao) == false){
    digitalWrite(ligaRele, 1);
  }

  else digitalWrite(ligaRele, 0);

  wdt_reset();
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

void iniciaLcd(){
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

void configIo(){
  pinMode(botao, INPUT_PULLUP);
  pinMode(enableRxTx, OUTPUT);
  pinMode(habilitaBombeamento, INPUT_PULLUP);
  pinMode(habilitaIrrigacao, INPUT_PULLUP);
  pinMode(ligaRele, OUTPUT);

  digitalWrite(ligaRele, 0);

  pinMode(bomba1, OUTPUT);
  pinMode(bomba2, OUTPUT);

  digitalWrite(bomba1, 0);
  digitalWrite(bomba2, 0);

  digitalWrite(enableRxTx, 0);
}