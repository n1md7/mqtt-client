#include <Ethernet.h>

#include "MQTT.h"

#define ON "ON"
#define OFF "OFF"


MQTT::MQTT(Component &connectingLight, Timer &timer) {
  this->connectingLight = connectingLight;
  this->timer = timer;

  this->clientId = nullptr;
  this->username = new char[32];
  this->password = new char[32];
  this->publishTopic = new char[32];
  this->subscribeTopic = new char[32];
  this->willTopic = new char[32];
  this->willQoS = new char[32];
  this->willRetain = new char[32];
  this->willPayload = new char[32];

  this->host = new char[32];
  this->port = new char[32];

  this->cleanSession = false;
  this->reportInterval = 5000;
  this->maxPingTimeout = 30000;

  this->retain = true;
  this->QoS = 1;

  this->lastReconnectAttempt = 0;
  this->pingInterval = 0;

  this->mqttClient = PubSubClient();

  this->emitPayload = new char[128];

  this->deviceCode = new char[32];
  this->deviceName = new char[32];
  this->deviceType = new char[32];
  this->deviceVersion = new char[2];
}

void MQTT::begin() {
  pingInterval = reportInterval;
  lastReconnectAttempt = 0;

  mqttClient.setServer(host, port);
  mqttClient.setCallback([](char *topic, byte *payload, unsigned int length) {
    this->onMessage(topic, payload, length);
  });
}

void MQTT::keepConnected() {
  if (!mqttClient.connected()) reconnect();
}

void MQTT::reconnect() {
  unsigned long currentMillis = millis();
  unsigned long deltaMillis = currentMillis - lastReconnectAttempt;

  if (deltaMillis < pingInterval) return;

  Serial.println("Attempting MQTT connection...");
  createPayload(willPayload);
  if (mqttClient.connect(
        clientId,         // Unique Client ID
        username,         // Username
        password,         // Password
        willTopic,        // Will topic
        willQoS,          // Will QoS
        willRetain,       // Will retain
        willPayload,      // Will payload - OFF status
        cleanSession)) {  // Clean session
    Serial.println("Connected to MQTT broker!");
    Serial.print("Subscribing to topic: ");
    Serial.println(subscribeTopic);

    mqttClient.subscribe(subscribeTopic, QoS);  // QoS 1, at least once delivery
    lastReconnectAttempt = 0;                // Reset the reconnect attempt counter
    pingInterval = reportInterval;           // Reset the retry delay
    connectingLight.off();                   // Turn off when connected
  } else {
    Serial.print("Failed to connect, rc=");
    Serial.print(mqttClient.state());
    Serial.println(" Retrying in " + String(pingInterval / 1000) + " seconds");

    // Use exponential backoff for retry delay
    lastReconnectAttempt = currentMillis;
    pingInterval = min(2 * pingInterval, maxPingTimeout | 30);  // Maximum retry delay of 30 seconds

    connectingLight.on();  // Turn on when trying to reconnect
  }
}

void MQTT::onMessage(const char[] topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [ ");
  Serial.print(topic);
  Serial.println(" ] ");

  // More info about the size @at https://arduinojson.org/v6/assistant/
  // If payload changes, this value should be updated
  StaticJsonDocument<48> doc;

  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // NestJS MQTT message emitter adds "data" property
  const char *status = doc["data"]["status"] | OFF;

  int minutes = doc["data"]["time"]["min"] | 0;
  int seconds = doc["data"]["time"]["sec"] | 0;


  Serial.print("Status: ");
  Serial.print(status);
  Serial.print("; Time: ");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.println();

  const bool turnOn = strcmp(status, ON) == 0;

  if (!turnOn) timer.reset();
  else {
    timer.set(minutes, seconds);
    timer.start();
  }

  emitCurrentState();
}

void MQTT::emitCurrentState() {
  createPayload(emitPayload, timer.getStatus(), timer.getValue());
  mqttClient.publish(
    publishTopic,
    emitPayload);
}

void MQTT::handle() {
  mqttClient.loop();
}

StaticJsonDocument MQTT::createJSONPayload() {
  StaticJsonDocument<128> payload;

  payload["status"] = OFF;
  payload["code"] = DEVICE_CODE;
  payload["name"] = DEVICE_NAME;
  payload["type"] = DEVICE_TYPE;
  payload["version"] = DEVICE_VERSION;
  payload["time"] = 0;

  return payload;
}

void MQTT::createPayload(char *buffer) {
  StaticJsonDocument<128> payload;

  payload["status"] = OFF;
  payload["time"] = 0;

  serializeJson(payload, buffer);
}

void MQTT::createPayload(char *buffer, char *status, unsigned long time) {
  StaticJsonDocument<128> payload;

  payload["status"] = status;
  payload["time"] = time;

  serializeJson(payload, buffer);
}

void MQTT::setClientId(char *clientId) {
  this->clientId = clientId;
}

void MQTT::setUsername(char *username) {
  this->username = username;
}

void MQTT::setPassword(char *password) {
  this->password = password;
}

void MQTT::setPublishTopic(char *publishTopic) {
  this->publishTopic = publishTopic;
}

void MQTT::setSubscribeTopic(char *subscribeTopic) {
  this->subscribeTopic = subscribeTopic;
}

void MQTT::setWillTopic(char *willTopic) {
  this->willTopic = willTopic;
}

void MQTT::setWillQoS(char *willQoS) {
  this->willQoS = willQoS;
}

void MQTT::setWillRetain(char *willRetain) {
  this->willRetain = willRetain;
}

void MQTT::setWillPayload(char *willPayload) {
  this->willPayload = willPayload;
}

void MQTT::setCleanSession(bool cleanSession) {
  this->cleanSession = cleanSession;
}

void MQTT::setReportInterval(int reportInterval) {
  this->reportInterval = reportInterval;
}

void MQTT::setMaxPingTimeout(int maxPingTimeout) {
  this->maxPingTimeout = maxPingTimeout;
}

void MQTT::setHost(char *host) {
  this->host = host;
}

void MQTT::setPort(char *port) {
  this->port = port;
}

void MQTT::setRetain(bool retain) {
  this->retain = retain;
}

void MQTT::setQoS(int *QoS) {
  this->QoS = QoS;
}
