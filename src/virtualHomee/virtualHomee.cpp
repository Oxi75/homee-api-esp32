#include "virtualHomee.hpp"

void virtualHomee::getSettings(JsonObject jsonDoc)
{
    jsonDoc["settings"]["address"] = "";
    jsonDoc["settings"]["city"] = "";
    jsonDoc["settings"]["zip"] = 11111;
    jsonDoc["settings"]["state"] = F("BW");
    jsonDoc["settings"]["latitude"] = "";
    jsonDoc["settings"]["longitude"] = "";
    jsonDoc["settings"]["country"] = F("Germany");
    jsonDoc["settings"]["language"] = F("de");
    jsonDoc["settings"]["wlan_dhcp"] = 1;
    jsonDoc["settings"]["remote_access"] = 1;
    jsonDoc["settings"]["beta"] = 0;
    jsonDoc["settings"]["webhooks_key"] = F("WEBHOOKKEY");
    jsonDoc["settings"]["automatic_location_detection"] = 0;
    jsonDoc["settings"]["polling_interval"] = 60;
    jsonDoc["settings"]["timezone"] = F("Europe%2FBerlin");
    jsonDoc["settings"]["enable_analytics"] = 0;
    jsonDoc["settings"]["wlan_enabled"] = 1;
    jsonDoc["settings"]["wlan_ip_address"] = F("192.168.178.222");
    jsonDoc["settings"]["wlan_ssid"] = F("homeeWifi");
    jsonDoc["settings"]["wlan_mode"] = 2;
    jsonDoc["settings"]["online"] = 0;
    jsonDoc["settings"]["lan_enabled"] = 1;
    
    // V7 Syntax: Array erstellen und hinzufügen
    jsonDoc["settings"]["available_ssids"].to<JsonArray>().add("homeeWifi");
    
    jsonDoc["settings"]["time"] = 1562707105;
    jsonDoc["settings"]["civil_time"] = F("2019-07-09 23:18:25");
    jsonDoc["settings"]["version"] = this->version;
    jsonDoc["settings"]["uid"] = this->homeeId;
    jsonDoc["settings"]["gateway_id"] = 1313337;
    jsonDoc["settings"]["local_ssl_enabled"] = false;
    jsonDoc["settings"]["b2b_partner"] = F("homee");
    jsonDoc["settings"]["homee_name"] = this->homeeId;
    
    // Leeres Array erstellen
    jsonDoc["settings"]["cubes"].to<JsonArray>();
}

void virtualHomee::addNode(node *n)
{
    nds.AddNode(n);
}

node* virtualHomee::getNodeById(int32_t node_id)
{
    return nds.GetNodeById(node_id);
}

nodeAttributes *virtualHomee::getAttributeWithId(uint32_t id)
{
    for (uint8_t i = 0; i < nds.GetNumberOfNodes(); i++)
    {
        for (uint8_t j = 0; j < nds.GetNode(i)->GetNumberOfAttributes(); j++)
        {
            if (nds.GetNode(i)->GetAttribute(j)->getId() == id)
            {
                return nds.GetNode(i)->GetAttribute(j);
            }
        }
    }
    return nullptr;
}

void virtualHomee::updateAttribute(nodeAttributes *_nodeAttribute)
{
    ws.cleanupClients();
    
    // Neu für ArduinoJson 7:
    JsonDocument doc;
    JsonObject attribute = doc["attribute"].to<JsonObject>();
    _nodeAttribute->GetJSONObject(attribute);
    
    String output;
    serializeJson(doc, output);
    ws.textAll(output);
}

void virtualHomee::updateNode(node* _node)
{
    ws.cleanupClients();
    
    // Neu für ArduinoJson 7:
    JsonDocument doc;
    JsonObject node = doc["node"].to<JsonObject>();
    _node->AddJSONObject(node);
    
    String output;
    serializeJson(doc, output);
    ws.textAll(output);
}

String virtualHomee::getUrlParameterValue(const String& url, const String& parameterName)
{
    int index = url.indexOf(parameterName + "=");
    String substr = url.substring(index + parameterName.length() + 1);
    index = substr.indexOf("&");
    if (index < 0)
    {
        return substr;
    }
    else
    {
        return substr.substring(0, index);
    }
}

nodeAttributes* virtualHomee::getAttributeById(uint32_t _id)
{
    for(int i = 0; i < this->nds.GetNumberOfNodes(); i++)
    {
        for(int j = 0; j < this->nds.GetNode(i)->GetNumberOfAttributes(); j++)
        {
            if(this->nds.GetNode(i)->GetAttribute(j)->getId() == _id)
            {
                return this->nds.GetNode(i)->GetAttribute(j);
            }
        }
    }
    return nullptr;
}

void virtualHomee::handleHttpOptionsAccessToken(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response = request->beginResponse(204);
    response->addHeader("access-control-allow-methods", "POST, DELETE");
    response->addHeader("access-control-allow-origin", "*");
    request->send(response);
}

void virtualHomee::handleHttpPostRequest(virtualHomee* context, AsyncWebServerRequest *request)
{ 
    char _buff[128];

    sprintf_P(_buff, PSTR("access_token=%s&user_id=1&device_id=1&expires=31536000"), context->access_token);
    AsyncWebServerResponse *response = request->beginResponse(200, "application/x-www-form-urlencoded", _buff);

    sprintf_P(_buff, PSTR("access_token=%s;Max-Age=2592000;"), context->access_token);
    response->addHeader("set-cookie", _buff);

    request->send(response);
}

void virtualHomee::initializeWebServer()
{
    server.on("/access_token", HTTP_OPTIONS, handleHttpOptionsAccessToken);
    server.on("/access_token", HTTP_DELETE, [](AsyncWebServerRequest *request) {});
    server.on("/access_token", HTTP_POST, [this](AsyncWebServerRequest *request){handleHttpPostRequest(this, request);});
}

void virtualHomee::initializeWebsocketServer()
{
    ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        if (type == WS_EVT_CONNECT)
        {
            this->clientConnected();
#ifdef DEBUG_VIRTUAL_HOMEE
            Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
            Serial.println(client->remoteIP());
#endif
        }
        else if (type == WS_EVT_DISCONNECT)
        {
            this->clientDisconnected();
#ifdef DEBUG_VIRTUAL_HOMEE
            Serial.printf("ws[%s] disconnect: %u\n", server->url(), client->id());
#endif
        }
        else if (type == WS_EVT_PONG)
        {
#ifdef DEBUG_VIRTUAL_HOMEE
            Serial.println("PONG");
#endif
        }
        else if (type == WS_EVT_DATA)
        {
            AwsFrameInfo *info = (AwsFrameInfo *)arg;
            if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
            {
                char* msg = new char[len + 1];
                memcpy(msg, data, len);
                msg[len] = '\0';
                String message = msg;
                delete[] msg;

#ifdef DEBUG_VIRTUAL_HOMEE
                Serial.print("DEBUG: Received Message: ");
                Serial.println(message);
#endif
                if (message.equalsIgnoreCase("GET:Settings"))
                {                 
                    JsonDocument doc;
                    JsonObject root = doc.to<JsonObject>();
                    this->getSettings(root);
                    this->sendWSMessage(doc, client);
                }
                else if (message.equalsIgnoreCase("GET:nodes"))
                {
                    JsonDocument doc;
                    JsonArray arr = doc["nodes"].to<JsonArray>();
                    nds.GetJSONArray(arr);
                    this->sendWSMessage(doc, client);
                }
                else if (message.substring(0, 9).equalsIgnoreCase("PUT:nodes")) 
                {
                    int32_t attributeId = this->getUrlParameterValue(message, "IDs").toInt();
                    double_t targetValue = this->getUrlParameterValue(message, "target_value").toDouble();

                    nodeAttributes *changedNode = this->getAttributeWithId(attributeId);
                    if (changedNode != nullptr)
                    {
                        changedNode->setTargetValue(targetValue);
                        changedNode->executeCallback();
                    }
                }
                else if (message.substring(0, 10).equalsIgnoreCase("POST:nodes")) 
                {
                    if (message.indexOf("compatibility_check=1") >= 0)
                    {
                        JsonDocument jsonDoc;
                        JsonObject root = jsonDoc["compatibility_check"].to<JsonObject>();
                        root["compatible"] = true;
                        root["account"] = true;
                        root["external_homee_status"] = F("none");
                        root["your_version"] = true;
                        root["my_version"] = this->version;
                        root["my_homeeID"] = this->homeeId;

                        this->sendWSMessage(jsonDoc, client);
                    }
                    else if (message.indexOf("start_pairing=1") >= 0)
                    {
                        JsonDocument jsonDoc;
                        JsonObject root = jsonDoc["pairing"].to<JsonObject>();
                        root["access_token"] = this->access_token;
                        root["expires"] = 315360000;
                        root["userID"] = 1;
                        root["deviceID"] = 1;

                        this->sendWSMessage(jsonDoc, client);
                    }
                }
                else if (message == "DELETE:users/1/devices/1")
                {
                    JsonDocument jsonDoc;
                    JsonObject root = jsonDoc["warning"].to<JsonObject>();
                    root["code"] = 600;
                    root["description"] = F("Your device got removed.");
                    root["message"] = F("You have been logged out.");
                    root["data"] = "{}";
                    this->sendWSMessage(jsonDoc, client);
                    client->close(4444, "DEVICE_DISCONNECT");
                }
            }
        }
        ws.cleanupClients();
    });
}

// Angepasste Sende-Funktion für ArduinoJson 7
void virtualHomee::sendWSMessage(JsonDocument& doc, AsyncWebSocketClient *client)
{ 
    String output;
    serializeJson(doc, output);

#ifdef DEBUG_VIRTUAL_HOMEE
    Serial.print("DEBUG: Send Message Size: ");
    Serial.println(output.length());
#endif
    
    // Senden des Strings direkt an den Client
    client->text(output);
}

void virtualHomee::start()
{
    server.begin();
    if(this->firstStart)
    {
        this->firstStart = false;
        this->startDiscoveryService();
    }
}

void virtualHomee::stop()
{
    //Disconnect all Clients
    ws.closeAll(4444, "DEVICE_DISCONNECT");
    ws.cleanupClients();
    //Stop Services
    server.end();
    
    // Handle UDP properly if it's listening
    if (udp.connected()) {
        udp.close();
    }    
}

void virtualHomee::startDiscoveryService()
{
    if (udp.listen(15263))
    {
#ifdef DEBUG_VIRTUAL_HOMEE
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
#endif
        udp.onPacket([this](AsyncUDPPacket packet)
        {
            String message = packet.readString();
#ifdef DEBUG_VIRTUAL_HOMEE
            Serial.print("UDP Message reveived: ");
            Serial.println(message);
#endif

            if (message.equalsIgnoreCase(this->gethomeeId()))
            {
                packet.printf("initialized:%s:%s:homee", this->gethomeeId().c_str(), this->gethomeeId().c_str());
            }
        });
    }
}

void virtualHomee::updateAttributeValue(nodeAttributes *_attribute, double _value)
{
    _attribute->setTargetValue(_value);
    this->updateAttribute(_attribute);
    _attribute->setCurrentValue(_value);
    this->updateAttribute(_attribute);
}

void virtualHomee::updateAttributeData(nodeAttributes* _attribute, const String& _data)
{
    _attribute->setData(_data);
    this->updateAttribute(_attribute);
}

String virtualHomee::gethomeeId()
{
    return this->homeeId;
}

void virtualHomee::clientConnected()
{
    this->numberOfWSClients++;
}

void virtualHomee::clientDisconnected()
{
    this->numberOfWSClients--;
}

size_t virtualHomee::getNumberOfWSClients()
{
    return this->numberOfWSClients;
}

String virtualHomee::getHomeeId()
{
    return this->homeeId;
}
void virtualHomee::setHomeeId(const String& _homeeId)
{
    this->homeeId = _homeeId;
}

virtualHomee::virtualHomee(const String& _homeeId)
    : virtualHomee()
{
    this->homeeId = _homeeId;
}

virtualHomee::virtualHomee()
    : server(7681),
      ws("/connection")
{
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    this->homeeId = mac;
    this->version = "2.25.0 (ed9c50)";
    this->nds.AddNode(new node(-1, 1, "homee"));

    initializeWebServer();
    initializeWebsocketServer();
    server.addHandler(&ws);
}

virtualHomee::~virtualHomee()
{
    ws.closeAll();
}

void virtualHomee::removeNodeById(uint32_t node_id)
{
    nds.RemoveNodeById(node_id);
}