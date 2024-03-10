#include <ArduinoJson.h>

// Pinos dos LEDs e Sensores
int bombaPin = 2;
int ledsCaixa[] = {3, 4, 5};
int sensores[] = {6, 7, 8};
int nLedsCaixa = sizeof(ledsCaixa) / sizeof(ledsCaixa[0]);

// Estado anterior dos sensores
//bool estadoAnteriorSensores[] = {HIGH, HIGH, HIGH};

// Estado da caixa d'água
enum NivelAgua { MINIMO, NIVEL_50, NIVEL_100 };
NivelAgua nivelAgua = MINIMO;

// Temporizador para envio de dados
unsigned long lastDataSendTime = 0;
const unsigned long dataSendInterval = 3000;  // Intervalo de envio em milissegundos

void setup() {
  Serial.begin(9600);

  // Inicialização dos LEDs e Sensores
  pinMode(bombaPin, OUTPUT);
  for (int i = 0; i < nLedsCaixa; i++) {
    pinMode(ledsCaixa[i], OUTPUT);
    pinMode(sensores[i], INPUT);
  }
  digitalWrite(bombaPin, HIGH);
}

void loop() 
{
  bool sensor0 = digitalRead(sensores[0]);
  bool sensor1 = digitalRead(sensores[1]);
  bool sensor2 = digitalRead(sensores[2]);
  bool bombaLigada = digitalRead(bombaPin) == HIGH;

  for (int i = 0; i < nLedsCaixa; i++) 
  {
    int estadoAtual = digitalRead(sensores[i]);

      // Houve uma mudança no estado do sensor
      if (estadoAtual == HIGH) {
        acenderLedCaixa(i);
      } else if (estadoAtual == LOW) {
        apagarLedCaixa(i);
      }
  }
  if (bombaLigada) {
    if (sensor0 && sensor1 && sensor2) {
      digitalWrite(bombaPin, LOW);
      delay(150);
    } else {
    delay(1);
    }
  } else {
    if (sensor0 || sensor1 || sensor2) {
      digitalWrite(bombaPin, LOW);
      delay(150);
    } else {
      digitalWrite(bombaPin, HIGH);
      delay(1);
    }
  }
  // Verifica se é hora de enviar dados ao Flask
  unsigned long currentMillis = millis();
  if (currentMillis - lastDataSendTime >= dataSendInterval) {
    enviarDadosParaFlask();
    lastDataSendTime = currentMillis;  // Atualiza o tempo do último envio
  }
}



void acenderLedCaixa(int index) {
  digitalWrite(ledsCaixa[index], HIGH);
  nivelAgua = static_cast<NivelAgua>(index);
}

void apagarLedCaixa(int index) {
  digitalWrite(ledsCaixa[index], LOW);
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
