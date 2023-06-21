#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Lucas Eduardo";
const char* password = "";
const char* mqttServer = "";
const int mqttPort = 1883;
const char* mqttUser = "Lucas";
const char* mqttPassword = "";
const char* mqttTopicTemperature = "esp32/temperature";
const char* mqttTopicPressure = "esp32/pressure";
const char* mqttTopicLevel = "esp32/level";
const char* mqttTopicLedTemperature = "Led/IndicaTemperatura";
const char* mqttTopicLedPressure = "Led/IndicaPressao";
const char* mqttTopicLedLevel = "Led/IndicaNivel";

const int sensorPinTemperature = 32;
const int sensorPinPressure = 33;
const int sensorPinLevel = 34;

const int ledPinTemperature = 18;
const int ledPinPressure = 19;
const int ledPinLevel = 21;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado ao WiFi com endereço IP: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPinTemperature, OUTPUT);
  pinMode(ledPinPressure, OUTPUT);
  pinMode(ledPinLevel, OUTPUT);

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Conectando ao servidor MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado ao servidor MQTT!");
      client.subscribe(mqttTopicLedTemperature);
      client.subscribe(mqttTopicLedPressure);
      client.subscribe(mqttTopicLedLevel);
    } else {
      Serial.print("Falha na conexão - Estado: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  // Verifica se a mensagem recebida é para o LED de temperatura
  if (strcmp(topic, mqttTopicLedTemperature) == 0) {
    Serial.print("Valor recebido para o LED de temperatura: ");
    Serial.write(payload, length);
    Serial.println();

    // Converte o payload para uma string
    String value = "";
    for (unsigned int i = 0; i < length; i++) {
      value += (char)payload[i];
    }

    value.toLowerCase(); // Converte para minúsculas

    // Liga ou desliga o LED de acordo com o valor recebido
    if (value.equals("true")) { // Comparação com letras minúsculas
      digitalWrite(ledPinTemperature, HIGH);
    } else {
      digitalWrite(ledPinTemperature, LOW);
    }
  }

  // Verifica se a mensagem recebida é para o LED de pressão
  if (strcmp(topic, mqttTopicLedPressure) == 0) {
    Serial.print("Valor recebido para o LED de pressão: ");
    Serial.write(payload, length);
    Serial.println();

    // Converte o payload para uma string
    String value = "";
    for (unsigned int i = 0; i < length; i++) {
      value += (char)payload[i];
    }

    value.toLowerCase(); // Converte para minúsculas

    // Liga ou desliga o LED de acordo com o valor recebido
    if (value.equals("true")) { // Comparação com letras minúsculas
      digitalWrite(ledPinPressure, HIGH);
    } else {
      digitalWrite(ledPinPressure, LOW);
    }
  }

  // Verifica se a mensagem recebida é para o LED de nível
  if (strcmp(topic, mqttTopicLedLevel) == 0) {
    Serial.print("Valor recebido para o LED de nível: ");
    Serial.write(payload, length);
    Serial.println();

    // Converte o payload para uma string
    String value = "";
    for (unsigned int i = 0; i < length; i++) {
      value += (char)payload[i];
    }

    value.toLowerCase(); // Converte para minúsculas

    // Liga ou desliga o LED de acordo com o valor recebido
    if (value.equals("true")) { // Comparação com letras minúsculas
      digitalWrite(ledPinLevel, HIGH);
    } else {
      digitalWrite(ledPinLevel, LOW);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Tentando reconectar ao servidor MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado ao servidor MQTT!");
      client.subscribe(mqttTopicLedTemperature);
      client.subscribe(mqttTopicLedPressure);
      client.subscribe(mqttTopicLedLevel);
    } else {
      Serial.print("Falha na conexão - Estado: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int sensorValueTemperature = analogRead(sensorPinTemperature);
  int sensorValuePressure = analogRead(sensorPinPressure);
  int sensorValueLevel = analogRead(sensorPinLevel);

  float temperature = map(sensorValueTemperature, 0, 4095, 0, 1000);
  float pressure = map(sensorValuePressure, 0, 4095, 0, 2000);
  float level = map(sensorValueLevel, 0, 4095, 0, 3000);

  String temperaturePayload = String(temperature);
  String pressurePayload = String(pressure);
  String levelPayload = String(level);

  Serial.print("Enviando dados para o Node-RED - Temperatura: ");
  Serial.println(temperature);
  client.publish(mqttTopicTemperature, temperaturePayload.c_str());

  Serial.print("Enviando dados para o Node-RED - Pressão: ");
  Serial.println(pressure);
  client.publish(mqttTopicPressure, pressurePayload.c_str());

  Serial.print("Enviando dados para o Node-RED - Nível: ");
  Serial.println(level);
  client.publish(mqttTopicLevel, levelPayload.c_str());

  delay(1000);
}
