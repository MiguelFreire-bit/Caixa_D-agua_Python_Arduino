#include <ArduinoJson.h>

// Pinos dos LEDs
int bombaPin = 2;
int ledsCaixa[] = {3, 4, 5};
int nLedsCaixa = sizeof(ledsCaixa) / sizeof(ledsCaixa[0]);

// Estado da caixa d'água
enum NivelAgua { MINIMO, NIVEL_50, NIVEL_100 };
NivelAgua nivelAgua = MINIMO;

void setup() {
  Serial.begin(9600);

  // Inicialização dos LEDs
  pinMode(bombaPin, OUTPUT);
  for (int i = 0; i < nLedsCaixa; i++) {
    pinMode(ledsCaixa[i], OUTPUT);
  }

  // Inicialmente, liga a bomba
  digitalWrite(bombaPin, HIGH);
}

void loop() {
  // Acende os LEDs da caixa um por um
  for (int i = 0; i < nLedsCaixa; i++) {

  if (nivelAgua == NIVEL_100) 
  {
    return;
  }

    acenderLedCaixa(i);
    enviarDadosParaFlask();
    
    if (i == 2) {
      delay(1000); // Aguarda 1 segundo entre cada LED
    } else {
      delay(9000); // Aguarda 9 segundos entre cada LED
    }
  }

  // Apaga o primeiro LED (bomba) e envia dados para Flask
  apagarLedBomba();
  enviarDadosParaFlask();
  delay(2000); // Aguarda 2 segundos
}

void acenderLedCaixa(int index) {
  digitalWrite(ledsCaixa[index], HIGH);
  nivelAgua = static_cast<NivelAgua>(index);
}

void apagarLedBomba() {
  digitalWrite(bombaPin, LOW);
}

void enviarDadosParaFlask() {
  // Crie um objeto JSON
  StaticJsonDocument<100> jsonDoc;
  jsonDoc["nivel_minimo"] = nivelAgua == MINIMO;
  jsonDoc["nivel_50"] = nivelAgua == NIVEL_50;
  jsonDoc["nivel_100"] = nivelAgua == NIVEL_100;
  jsonDoc["bomba_estado"] = digitalRead(bombaPin) == HIGH;

  // Converta o JSON para uma string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Envie a string para Flask usando a conexão serial
  Serial.println(jsonString);
}