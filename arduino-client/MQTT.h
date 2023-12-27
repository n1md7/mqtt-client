#ifndef MQTT_h
#define MQTT_h

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Component.h"
#include "Timer.h"

class MQTT {
public:
  MQTT(Component &connectingLight, Timer &timer);

  void begin();
  void keepConnected();
  void reconnect();
  void onMessage(char *topic, byte *payload, unsigned int length);
  void emitCurrentState();
  void handle();

  // Setters for configuration parameters
  void setDeviceCode(const char *deviceCode);
  void setDeviceName(const char *deviceName);
  void setDeviceType(const char *deviceType);
  void setDeviceVersion(const char *deviceVersion);

  void setClientId(const char *clientId);
  void setHost(const char *host);
  void setPort(int port);
  void setRetain(bool retain);
  void setQoS(int QoS);
  void setUsername(const char *username);
  void setPassword(const char *password);

  void setPublishTopic(const char *publishTopic);
  void setSubscribeTopic(const char *subscribeTopic);

  void setWillTopic(const char *willTopic);
  void setWillQoS(int willQoS);
  void setWillRetain(bool willRetain);

  void setCleanSession(bool cleanSession);

  void setReportInterval(int reportInterval);
  void setMaxPingTimeout(int maxPingTimeout);

private:
  static unsigned long lastReconnectAttempt;
  unsigned long pingInterval;

  PubSubClient mqttClient;
  Component &connectingLight;
  Timer &timer;

  char emitPayload[128];

  char deviceCode[32];
  char deviceName[32];
  char deviceType[32];
  char deviceVersion[32];

  char clientId[32];
  char host[64];
  int port;
  bool retain;
  int QoS;

  char username[32];
  char password[32];

  char publishTopic[64];
  char subscribeTopic[64];

  char willTopic[64];
  int willQoS;
  bool willRetain;

  bool cleanSession;

  unsigned int reportInterval;
  unsigned int maxPingTimeout;

  void handleUpdateRequest();
  void handleStatusUpdate();

  StaticJsonDocument<128> createJSONPayload();

  void createPayload(char *buffer);
  void createPayload(char *buffer, char *status, unsigned long time);
};

#endif
