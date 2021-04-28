#ifndef virtualHomee_hpp
#define virtualHomee_hpp

#if defined(ESP32)
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "nodes.hpp"

class virtualHomee
{
private:
    String homeeId;
    String version;
    String settings;
    String access_token;

    AsyncWebServer server;
    AsyncWebSocket ws;

    nodes nds;

    void setSettings();

    nodeAttributes* getAttributeWithId(uint32_t id);
    String getUrlParameterValue(String url, String parameterName);

public:
    void start();
    void addNode(node* n);
    void updateAttribute(nodeAttributes* _nodeAttribute);

    virtualHomee();
    ~virtualHomee();
};

#endif