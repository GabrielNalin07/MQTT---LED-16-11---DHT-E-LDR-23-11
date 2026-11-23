// #include <Arduino.h>
// #include <ArduinoJson.h>
// #include <PubSubClient.h>
// #include <WiFi.h>
// #include "Internet.h"
// #include <ezTime.h>
// #include <LiquidCrystal_I2C.h>
// #define pinLed 2

// const char *mqtt_server = "test.mosquitto.org";
// const int mqtt_port = 1883;
// const char *mqtt_client_id = "senai134_esp_gabriel";
// const char *mqtt_topic_sub = "bapnalin/netnet/pub";
// const char *mqtt_topic_pub = "bapnalin/netnet/sub";

// void connectMqtt();
// void retornoMqtt(char *topic, byte *payload, unsigned int lenght);

// WiFiClient espClient;
// PubSubClient client(espClient);

// String textoDigitado;
// Timezone sp;
// String dataAtual;
// String horaAtual;
// bool estadoLed = false; // Controla se o LED fica ligado (true) ou desligado (false)

// void setup() {
//   Serial.begin(115200);
//   Serial.setTimeout(10000);
//   pinMode(pinLed, OUTPUT);

//   conexaoWifi();

//   waitForSync();

//   client.setServer(mqtt_server, mqtt_port);
//   client.setCallback(retornoMqtt);
// }

// void loop() {
//   reConexaoWifi();
//   events();

//   // Obtenção da Data (xx/xx/xxxx) e Horario completo (HH:MM:SS)
//   dataAtual = sp.dateTime("d/m/Y");
//   horaAtual = sp.dateTime("H:i:s");

//   client.loop();
//   if (!client.connected())
//   {
//     connectMqtt();
//   }

//   // Atualiza o LED conforme o comando recebido
//   digitalWrite(pinLed, estadoLed);

//   // ler da serial 
//   if (Serial.available() > 0)
//   {
//     textoDigitado = Serial.readStringUntil('\n');
//     textoDigitado.trim();
//     if (textoDigitado.length() == 0)
//     {
//       Serial.println("msg vazia");
//       return;
//     }
  
//     // Montar Json
//     JsonDocument doc; 
//     doc["disp"] = "esp-nalin";
//     doc["msg"] = textoDigitado;
//     doc["data"] = dataAtual;
//     doc["hora"] = horaAtual;
  
//     String stringJson;
//     serializeJson(doc, stringJson);
//     client.publish(mqtt_topic_pub, stringJson.c_str());
  
//     Serial.print("enviado: ");
//     Serial.println(stringJson);
//     Serial.println("Data: " + dataAtual);
//     Serial.println("Hora: " + horaAtual);
//   }
// }

// //--------------------------------------------

// void connectMqtt() {
//   while (!client.connected())
//   {
//     Serial.println("Conectando ao MQTT");
//     if (client.connect(mqtt_client_id))
//     {
//       Serial.println("Conectado ");
//       client.subscribe(mqtt_topic_sub);
//     }
//     else {
//       Serial.println("Falha ao se conectar ");
//       Serial.println(client.state());
//       Serial.println("Tentando novamente, em 5 seg");
//       delay(5000);
//     }
//   }
// }

// void retornoMqtt(char *topic, byte *payload, unsigned int length) {
//   Serial.print("Mensagem recebida em ");
//   Serial.print(topic);
//   Serial.print(": ");

//   String msgRecebida = "";
//   for (int i = 0; i < length; i++)
//   {
//     msgRecebida += (char)payload[i];
//   }
//   Serial.println(msgRecebida);

//   JsonDocument doc;
//   DeserializationError erro = deserializeJson(doc, msgRecebida);
//   if (erro)
//   {
//     Serial.print("Erro ao decodificar o JSON: ");
//     Serial.println(erro.c_str());
//     return;
//   }

//   // Acessar os campos do Json
//   const char* dispositivo = doc["disp"];
//   const char* msg = doc["msg"];
//   Serial.println("Dados Decodificados");
//   Serial.print("Esp: ");
//   Serial.println(dispositivo);
//   Serial.print("Mensagem ");
//   Serial.println(msg);
//   Serial.println("---------------------------");

//   // Verifica o comando recebido no campo "msg"
//   if (msg != nullptr) {
//     if (strcasecmp(msg, "ligar") == 0) {
//       estadoLed = true;
//     } 
//     else if (strcasecmp(msg, "desligar") == 0) {
//       estadoLed = false;
//     }
//   }
// }

//-------------------- Atividade 23/11 -------------------------------------------------------------------------------
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "Internet.h"
#include <LiquidCrystal_I2C.h>

const char *mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char *mqtt_client_id = "senai134_esp_gabriel";
const char *mqtt_topic_sub = "bapnalin/netnet/pub";

void connectMqtt();
void retornoMqtt(char *topic, byte *payload, unsigned int length);

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Conectando...");

  conexaoWifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(retornoMqtt);
}

void loop() {
  reConexaoWifi();

  if (!client.connected()) {
    connectMqtt();
  }
  client.loop();
}


void connectMqtt() {
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT");
    if (client.connect(mqtt_client_id)) {
      Serial.println("Conectado ");
      client.subscribe(mqtt_topic_sub);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT Conectado");
    } else {
      Serial.println("Falha ao se conectar ");
      Serial.println(client.state());
      Serial.println("Tentando novamente, em 5 seg");
      delay(5000);
    }
  }
}

void retornoMqtt(char *topic, byte *payload, unsigned int length) {
  String msgRecebida = "";
  for (int i = 0; i < length; i++) {
    msgRecebida += (char)payload[i];
  }
  Serial.println(msgRecebida);

  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, msgRecebida);
  if (erro) {
    Serial.print("Erro ao decodificar o JSON: ");
    Serial.println(erro.c_str());
    return;
  }

  const char* dispositivo = doc["disp"];
  float temperatura = doc["temperatura"];
  float umidade = doc["umidade"];
  int luminosidade = doc["luminosidade"];

 
  lcd.clear();
 
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperatura, 1);
  lcd.print("C U:");
  lcd.print(umidade, 1);
  lcd.print("%");


  lcd.setCursor(0, 1);
  lcd.print("L:");
  lcd.print(luminosidade);
  lcd.print(" ");
  lcd.print(dispositivo);
}