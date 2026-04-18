// network.cpp
#include <Arduino.h>
#include <Update.h>
#include <esp_ota_ops.h>
#include <AsyncTCP.h>
#include "globals.h"
#include "config.h"
#include "web.h"

// WEBSOCKET - status JSON
// ============================================================
String buildStatusJSON() {
    String json = "{\"type\":\"status\"";
    json.reserve(768);
    json += ",\"input\":"        + String(currentInput);
    json += ",\"ip\":\""         + jsonEscape(staConnected ? localIP : String(AP_IP)) + "\"";
    json += ",\"wifi_ssid\":\""  + jsonEscape(wifiSSID) + "\"";
    json += ",\"brightness\":"   + String(brightness);
    json += ",\"disp_invert\":"  + String(dispInvert ? "true" : "false");
    json += ",\"disp_rotate\":"  + String(dispRotate ? "true" : "false");
    json += ",\"mqtt_host\":\""  + jsonEscape(mqttHost) + "\"";
    json += ",\"mqtt_port\":"    + String(mqttPort);
    json += ",\"mqtt_user\":\""  + jsonEscape(mqttUser) + "\"";
    json += ",\"mqtt_pass_set\":" + String(mqttPass.length() > 0 ? "true" : "false");
    json += ",\"mqtt_topic\":\"" + jsonEscape(mqttTopic) + "\"";
    json += ",\"mqtt_enabled\":" + String(mqttEnabled ? 1 : 0);
    json += ",\"mqtt_connected\":" + String(mqttConnected ? 1 : 0);
    json += ",\"admin_pass_set\":" + String(adminPass.length() > 0 ? "true" : "false");
    // AP pass removed — always uses default
    json += ",\"relay_masks\":[";
    for(int i = 0; i < 4; i++) {
        if(i > 0) json += ",";
        json += relayEnabled[i] ? "1" : "0";
    }
    json += "]";
    
    // Audio - wysyłaj procenty (0-100%), nie raw (0-56)
    int volPercent = audioVolume * 100 / 56;
    json += ",\"audio_vol\":" + String(volPercent);
    json += ",\"audio_bass\":" + String((int)audioBass);
    json += ",\"audio_treble\":" + String((int)audioTreble);
    
    json += ",\"ir\":{";
    json += "\"source\":"  + String(irCodes[0]);
    json += ",\"input1\":" + String(irCodes[1]);
    json += ",\"input2\":" + String(irCodes[2]);
    json += ",\"input3\":" + String(irCodes[3]);
    json += ",\"input4\":" + String(irCodes[4]);
    json += ",\"volp\":"   + String(irCodes[5]);
    json += ",\"volm\":"   + String(irCodes[6]);
    json += ",\"up\":"     + String(irCodes[7]);
    json += ",\"down\":"   + String(irCodes[8]);
    json += "}}";
    return json;
}

void notifyAll(const String& msg) {
    ws.textAll(msg);
}

void notifyAudioChanged() {
    int volPercent = audioVolume * 100 / 56;
    String msg;
    msg.reserve(72);
    msg = "{\"type\":\"audio_changed\",";
    msg += "\"vol\":" + String(volPercent) + ",";
    msg += "\"bass\":" + String((int)audioBass) + ",";
    msg += "\"treble\":" + String((int)audioTreble) + "}";
    notifyAll(msg);
}

void sendWSError(AsyncWebSocketClient* client, const char* code) {
    if(!client) return;
    client->text("{\"type\":\"error\",\"message\":\"" + String(code) + "\"}");
}

// ============================================================
// WEBSOCKET - obsługa wiadomości
// ============================================================
// NOTE: handleWSMessage() jest wywoływane z callbacku AsyncWebSocket (inny kontekst niż loop).
// Dla pojedynczego użytkownika ryzyko konfliktu jest niskie. Przy wielu klientach
// lub przy OTA warto rozważyć kolejkowanie zdarzeń do loop().
void handleWSMessage(AsyncWebSocketClient* client, const String& data) {
    Serial.println("WS: " + data);

    String cmd;
    if(!extractWSCommand(data, cmd)) {
        sendWSError(client, "missing_cmd");
        return;
    }

    if(cmd == "auth") {
        String pass;
        if(extractJsonString(data, "pass", pass)) {
            if(adminPass.length() == 0 || pass == adminPass) {
                wsAuthorized.insert(client->id());
                client->text("{\"type\":\"auth_ok\"}");
                Serial.printf("WS: Client %u authenticated\n", client->id());
            } else {
                wsAuthorized.erase(client->id());
                client->text("{\"type\":\"auth_fail\"}");
                Serial.printf("WS: Client %u auth failed\n", client->id());
            }
        } else {
            sendWSError(client, "missing_pass");
        }
        return;
    }

    bool authorized = (adminPass.length() == 0) || wsAuthorized.count(client->id());
    if(cmd != "get_status" && !authorized) {
        sendWSError(client, "unauthorized");
        return;
    }

    if(cmd == "get_status") {
        client->text(buildStatusJSON());
    }
    else if(cmd == "set_mqtt_enabled") {
        bool newEnabled = false;
        if(extractJsonBoolish(data, "enabled", newEnabled)) {
            if(newEnabled != mqttEnabled) {
                mqttEnabled = newEnabled;
                saveMqttSettings();
                logMsg("MQTT: set_mqtt_enabled = %d", mqttEnabled);
                
                if(!mqttEnabled && mqttClient.connected()) {
                    mqttClient.disconnect();
                    logMsg("MQTT: Disconnecting...");
                }
                if(mqttEnabled && mqttHost.length() > 0) {
                    setupMqtt();
                    mqttLastReconnect = 0;  // natychmiastowa próba połączenia w loop()
                }
            }
            notifyAll(buildStatusJSON());
        } else {
            sendWSError(client, "missing_enabled");
        }
    }
    else if(cmd == "set_input") {
        int input = 0;
        if(extractJsonInt(data, "input", input)) {
            switchInput(input);
            notifyAll("{\"type\":\"input_changed\",\"input\":" + String(currentInput) + "}");
        } else {
            sendWSError(client, "missing_input");
        }
    }
    else if(cmd == "learn_ir") {
        String key;
        if(extractJsonString(data, "key", key)) {
            learningKey = key;
            learningIR  = true;
            learnStart  = millis();
            showIROverlay("IR: Press button for " + learningKey + "...");
        } else {
            sendWSError(client, "missing_key");
        }
    }
    else if(cmd == "clear_ir") {
        String key;
        if(extractJsonString(data, "key", key)) {
            // Znajdź indeks i wyzeruj kod
            if(key == "source")       irCodes[0] = 0;
            else if(key == "input1")  irCodes[1] = 0;
            else if(key == "input2")  irCodes[2] = 0;
            else if(key == "input3")  irCodes[3] = 0;
            else if(key == "input4")  irCodes[4] = 0;
            else if(key == "volp")    irCodes[5] = 0;
            else if(key == "volm")    irCodes[6] = 0;
            else if(key == "up")      irCodes[7] = 0;
            else if(key == "down")    irCodes[8] = 0;
            
            saveIRCodes();
            Serial.println("IR cleared: " + key);
            notifyAll("{\"type\":\"ir_cleared\",\"key\":\"" + jsonEscape(key) + "\"}");
        } else {
            sendWSError(client, "missing_key");
        }
    }
    else if(cmd == "save_ir") {
        const char* keys[] = {"source","input1","input2","input3","input4","volp","volm","up","down"};
        for(int i = 0; i < 9; i++) {
            uint32_t value = 0;
            if(extractJsonUInt(data, keys[i], value)) irCodes[i] = value;
        }
        saveIRCodes();
        notifyAll("{\"type\":\"ir_saved\"}");
    }
    else if(cmd == "set_brightness") {
        int value = 0;
        if(extractJsonInt(data, "value", value)) {
            uint8_t val = normalizeBrightness(value);
            setBrightness(val);
            saveBrightness(val);
        } else {
            sendWSError(client, "missing_value");
        }
    }
    else if(cmd == "set_invert") {
        bool value = false;
        if(extractJsonBoolish(data, "value", value)) {
            dispInvert = value;
            tft.invertDisplay(dispInvert);
            saveDisplaySettings();
            notifyAll("{\"type\":\"status_updated\",\"disp_invert\":" + String(dispInvert ? "true" : "false") + "}");
        } else {
            sendWSError(client, "missing_value");
        }
    }
    else if(cmd == "set_rotate") {
        bool value = false;
        if(extractJsonBoolish(data, "value", value)) {
            dispRotate = value;
            applyDisplaySettings();
            saveDisplaySettings();
            notifyAll("{\"type\":\"status_updated\",\"disp_rotate\":" + String(dispRotate ? "true" : "false") + "}");
        } else {
            sendWSError(client, "missing_value");
        }
    }
    else if(cmd == "save_relays") {
        Serial.printf("DEBUG: Parsing relays from: %s\n", data.c_str());
        for(int i = 0; i < 4; i++) {
            int val = 0;
            if(extractJsonInt(data, ("mask" + String(i)).c_str(), val)) {
                relayEnabled[i] = (val != 0);
                Serial.printf("  mask%d: val=%d, enabled=%d\n", i, val, relayEnabled[i]);
            }
        }
        saveRelayEnabled();
        applyRelays(currentInput);
        // Wyślij pełny status ze Updated relay_masks
        String statusJson = "{\"type\":\"relays_updated\",\"relay_masks\":[";
        for(int i = 0; i < 4; i++) {
            if(i > 0) statusJson += ",";
            statusJson += relayEnabled[i] ? "1" : "0";
        }
        statusJson += "]}";
        notifyAll(statusJson);
        Serial.println("Relay config updated");
    }
    else if(cmd == "set_volume") {
        int percent = 0;
        if(extractJsonInt(data, "value", percent)) {
            if(percent < 0) percent = 0;
            if(percent > 100) percent = 100;
            
            // Konwersja procent → PT2314 (0-56, biblioteka robi 56-val)
            audioVolume = normalizeVolume(percent * 56 / 100);
            
            audio.setVolume(audioVolume);
            markAudioSettingsDirty();
            notifyAudioChanged();
            mqttPublishAudio();
            Serial.printf("WebUI: Volume set to %d%% (raw: %d)\n", percent, audioVolume);
        } else {
            sendWSError(client, "missing_value");
        }
    }
    else if(cmd == "set_bass") {
        int value = 0;
        if(extractJsonInt(data, "value", value)) {
            audioBass = normalizeToneDb(value);
            audio.setBass(audioBass);
            markAudioSettingsDirty();
            notifyAudioChanged();
            mqttPublishAudio();
        } else {
            sendWSError(client, "missing_value");
        }
    }
    else if(cmd == "set_treble") {
        int value = 0;
        if(extractJsonInt(data, "value", value)) {
            audioTreble = normalizeToneDb(value);
            audio.setTreble(audioTreble);
            markAudioSettingsDirty();
            notifyAudioChanged();
            mqttPublishAudio();
        } else {
            sendWSError(client, "missing_value");
        }
    }
    else if(cmd == "save_wifi") {
        String ssid, pass;
        if(extractJsonString(data, "ssid", ssid) && extractJsonString(data, "pass", pass)) {
            saveWifiCreds(ssid, pass);
            notifyAll("{\"type\":\"wifi_saved\"}");
            flushAudioSettingsIfDirty(true);
            restartRequested = true;
            restartAtMs = millis() + 1500;
            Serial.println("WiFi: Restart scheduled");
        } else {
            sendWSError(client, "missing_wifi_fields");
        }
    }
    else if(cmd == "test_wifi") {
        String ssid, pass;
        if(extractJsonString(data, "ssid", ssid) && extractJsonString(data, "pass", pass)) {
            if(wifiTestInProgress) {
                sendWSError(client, "wifi_test_busy");
                return;
            }
            wifiTestInProgress = true;
            wifiTestSSID = ssid;
            wifiTestPass = pass;
            wifiTestClientId = client->id();
            wifiTestTries = 0;
            wifiTestNextMs = millis() + 500;
            WiFi.begin(ssid.c_str(), pass.c_str());
            client->text("{\"type\":\"wifi_test_started\"}");
            Serial.printf("WiFi: Test started for %s (client %u)\n", ssid.c_str(), client->id());
        } else {
            sendWSError(client, "missing_wifi_fields");
        }
    }
    else if(cmd == "save_mqtt") {
        String value;
        int port = mqttPort;
        bool enabled = mqttEnabled;

        bool hostOk = extractJsonString(data, "host", value);
        if(hostOk) mqttHost = value;
        bool userOk = extractJsonString(data, "user", value);
        if(userOk) mqttUser = value;
        bool passOk = extractJsonString(data, "pass", value);
        if(passOk && value.length() > 0) mqttPass = value;
        // Puste hasło = nie zmieniaj (użytkownik nie wpisał nowego)
        bool topicOk = extractJsonString(data, "topic", value);
        if(topicOk) mqttTopic = normalizeMqttTopic(value);
        if(extractJsonInt(data, "port", port)) mqttPort = normalizeMqttPort(port);
        if(extractJsonBoolish(data, "enabled", enabled)) mqttEnabled = enabled;

        logMsg("MQTT: save parse - hostOk=%d userOk=%d passOk=%d passLen=%d topicOk=%d enabled=%d",
               hostOk, userOk, passOk, passOk ? value.length() : -1, topicOk, mqttEnabled);

        saveMqttSettings();
        logMsg("MQTT: Config saved - enabled=%d, host=%s, port=%d", 
               mqttEnabled, mqttHost.c_str(), mqttPort);
        notifyAll("{\"type\":\"mqtt_saved\"}");
        
        // Disconnect i rekonfiguruj z nowymi parametrami
        if(mqttClient.connected()) {
            mqttClient.disconnect();
            logMsg("MQTT: Disconnecting to apply new config...");
        }
        
        if(mqttEnabled && staConnected) {
            logMsg("MQTT: Reconfiguring with new parameters...");
            setupMqtt();
            mqttLastReconnect = 0;  // natychmiastowa próba połączenia w loop()
            logMsg("MQTT: Config applied, will connect immediately");
        } else {
            logMsg("MQTT: Not connecting - enabled=%d staConnected=%d", mqttEnabled, staConnected);
        }
    }
    else if(cmd == "save_admin") {
        String pass;
        if(extractJsonString(data, "pass", pass)) {
            saveAdminPass(pass);
            notifyAll(buildStatusJSON());
            notifyAll("{\"type\":\"admin_saved\"}");
        } else {
            sendWSError(client, "missing_pass");
        }
    }
    else {
        sendWSError(client, "unknown_cmd");
    }
}

// ============================================================
// WEBSOCKET - event handler
// ============================================================
void onWSEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if(type == WS_EVT_CONNECT) {
        Serial.printf("WS: client %u connected\n", client->id());
        client->text(buildStatusJSON());
    }
    else if(type == WS_EVT_DISCONNECT) {
        wsAuthorized.erase(client->id());
        Serial.printf("WS: client %u disconnected\n", client->id());
    }
    else if(type == WS_EVT_DATA) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if(!info || info->opcode != WS_TEXT) {
            client->text("{\"type\":\"error\",\"message\":\"ws_text_only\"}");
            return;
        }

        // UI wysyła małe pojedyncze ramki; nie parsujemy połowy wiadomości JSON.
        if(!info->final || info->index != 0 || info->len != len) {
            client->text("{\"type\":\"error\",\"message\":\"fragmented_ws_unsupported\"}");
            return;
        }

        String msg;
        msg.reserve(len + 1);
        for(size_t i = 0; i < len; i++) msg += (char)data[i];
        handleWSMessage(client, msg);
    }
}

// ============================================================
// MQTT - callbacks
// ============================================================
void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            localIP = WiFi.localIP().toString();
            staConnected = true;
            logMsg("WiFi: Got IP %s - MQTT can now connect", localIP.c_str());
            if(mqttEnabled && mqttHost.length() > 0) {
                setupMqtt();
                mqttLastReconnect = 0;  // natychmiastowa próba połączenia
            }
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            logMsg("WiFi: Disconnected - MQTT will disconnect");
            staConnected = false;
            mqttConnected = false;
            if(mqttClient.connected()) {
                mqttClient.disconnect();
            }
            break;
        default:
            break;
    }
}

void onMqttConnect(bool sessionPresent) {
    mqttConnected = true;
    logMsg("MQTT: Connected");

    // Subscribe na temat /set (input)
    String subTopic = mqttTopic + "/set";
    mqttClient.subscribe(subTopic.c_str(), 0);
    logMsg("MQTT: Subscribed to %s", subTopic.c_str());
    
    // Subscribe na tematy audio
    mqttClient.subscribe((mqttTopic + "/volume/set").c_str(), 0);
    mqttClient.subscribe((mqttTopic + "/bass/set").c_str(), 0);
    mqttClient.subscribe((mqttTopic + "/treble/set").c_str(), 0);
    logMsg("MQTT: Subscribed to audio topics");

    // Publish status online + aktualny stan
    mqttPublishStatus("online");
    mqttPublishInput(currentInput);
    mqttPublishAudio();  // Publikuj początkowy stan audio

    // Powiadom web interface
    notifyAll("{\"type\":\"mqtt_connected\"}");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    mqttConnected = false;
    
    // Wyświetl kod błędu
    logMsg("MQTT: Disconnected - reason: %d", (int)reason);
    switch(reason) {
        case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
            logMsg("MQTT: reason TCP_DISCONNECTED"); break;
        case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
            logMsg("MQTT: reason UNACCEPTABLE_PROTOCOL_VERSION"); break;
        case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
            logMsg("MQTT: reason IDENTIFIER_REJECTED"); break;
        case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
            logMsg("MQTT: reason SERVER_UNAVAILABLE"); break;
        case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
            logMsg("MQTT: reason MALFORMED_CREDENTIALS"); break;
        case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
            logMsg("MQTT: reason NOT_AUTHORIZED - check username/password!"); break;
        default:
            logMsg("MQTT: reason UNKNOWN (%d)", (int)reason); break;
    }
    
    notifyAll("{\"type\":\"mqtt_disconnected\"}");
}

// NOTE: onMqttMessage() jest wywoływane z callbacku AsyncMqttClient (inny kontekst niż loop).
// Dla pojedynczego użytkownika ryzyko konfliktu z loop() jest niskie.
void onMqttMessage(char* topic, char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {
    // Buforowanie fragmentów — jeśli total > len, payload jest podzielony
    if(index == 0) {
        mqttPayloadBuf = "";
        mqttPayloadBuf.reserve(total + 1);
        strncpy(mqttFragTopic, topic, sizeof(mqttFragTopic) - 1);
        mqttFragTopic[sizeof(mqttFragTopic) - 1] = '\0';
    }
    for(size_t i = 0; i < len; i++) mqttPayloadBuf += (char)payload[i];
    if(index + len < total) return; // czekaj na resztę

    String msg = mqttPayloadBuf;
    String topicStr = String(mqttFragTopic);
    mqttPayloadBuf = "";

    logMsg("MQTT recv: %s = %s", topicStr.c_str(), msg.c_str());

    // {topic}/set - przełącz wejście
    if(topicStr == mqttTopic + "/set") {
        int input = msg.toInt() - 1;  // 1-4 → 0-3
        if(input >= 0 && input <= 3) {
            switchInput(input);
            notifyAll("{\"type\":\"input_changed\",\"input\":" + String(currentInput) + "}");
        }
    }
    // {topic}/volume/set - ustaw głośność (0-100%)
    else if(topicStr == mqttTopic + "/volume/set") {
        int percent = msg.toInt();
        if(percent >= 0 && percent <= 100) {
            audioVolume = normalizeVolume(percent * 56 / 100);
            audio.setVolume(audioVolume);
            markAudioSettingsDirty();
            
            notifyAudioChanged();
            mqttPublishAudio();
            logMsg("MQTT: Volume set to %d%%", percent);
        }
    }
    // {topic}/bass/set - ustaw bass (-14 do +14)
    else if(topicStr == mqttTopic + "/bass/set") {
        int val = msg.toInt();
        if(val >= -14 && val <= 14) {
            audioBass = normalizeToneDb(val);
            audio.setBass(audioBass);
            markAudioSettingsDirty();
            
            notifyAudioChanged();
            mqttPublishAudio();
            logMsg("MQTT: Bass set to %+d dB", audioBass);
        }
    }
    // {topic}/treble/set - ustaw treble (-14 do +14)
    else if(topicStr == mqttTopic + "/treble/set") {
        int val = msg.toInt();
        if(val >= -14 && val <= 14) {
            audioTreble = normalizeToneDb(val);
            audio.setTreble(audioTreble);
            markAudioSettingsDirty();
            
            notifyAudioChanged();
            mqttPublishAudio();
            logMsg("MQTT: Treble set to %+d dB", audioTreble);
        }
    }
}

// ============================================================
// MQTT - setup i reconnect
// ============================================================
void setupMqtt() {
    if(mqttHost.length() == 0 || !mqttEnabled) {
        logMsg("MQTT: setup skipped - host='%s' enabled=%d", mqttHost.c_str(), mqttEnabled);
        return;
    }

    logMsg("MQTT: setupMqtt() start - host=%s port=%d", mqttHost.c_str(), mqttPort);
    
    // Użyj IPAddress zamiast const char* żeby ominąć AsyncDNS
    // (podanie IP jako string do DNS może dawać natychmiastowe błędy)
    IPAddress mqttIp;
    if(mqttIp.fromString(mqttHost)) {
        mqttClient.setServer(mqttIp, mqttPort);
        logMsg("MQTT: Using IPAddress %s (bypass DNS)", mqttIp.toString().c_str());
    } else {
        mqttClient.setServer(mqttHost.c_str(), mqttPort);
        logMsg("MQTT: Using hostname %s (DNS lookup)", mqttHost.c_str());
    }

    if(mqttUser.length() > 0) {
        mqttClient.setCredentials(mqttUser.c_str(), mqttPass.c_str());
        logMsg("MQTT: Using credentials - user='%s', pass='%s'", 
               mqttUser.c_str(), mqttPass.length() > 0 ? "***" : "(empty)");
    } else {
        mqttClient.setCredentials(nullptr, nullptr);
        logMsg("MQTT: No credentials set (anonymous)");
    }

    // Last Will Testament
    String lwtStr = mqttTopic + "/status";
    strncpy(mqttLwtTopicBuf, lwtStr.c_str(), sizeof(mqttLwtTopicBuf) - 1);
    mqttLwtTopicBuf[sizeof(mqttLwtTopicBuf) - 1] = '\0';
    mqttClient.setWill(mqttLwtTopicBuf, 0, true, "offline");
    logMsg("MQTT: LWT topic=%s", mqttLwtTopicBuf);

    // Unikalny ClientId na podstawie MAC (ostatnie 3 bajty)
    uint8_t mac[6];
    WiFi.macAddress(mac);
    snprintf(mqttClientIdBuf, sizeof(mqttClientIdBuf), "MultiAMP-%02X%02X%02X", mac[3], mac[4], mac[5]);
    mqttClient.setClientId(mqttClientIdBuf);
    
        logMsg("MQTT: Configured for %s:%d (clientId=%s)", mqttHost.c_str(), mqttPort, mqttClientIdBuf);
        logMsg("MQTT: staConnected=%d mqttEnabled=%d mqttConnected=%d", staConnected, mqttEnabled, mqttConnected);
}

void mqttReconnectIfNeeded() {
    if(!mqttEnabled || mqttHost.length() == 0) return;
    if(mqttConnected) return;
    if(!staConnected) return;

    unsigned long now = millis();
    if(now - mqttLastReconnect < MQTT_RECONNECT_DELAY) return;
    mqttLastReconnect = now;

    bool internalConnected = mqttClient.connected();
        logMsg("MQTT: Attempting connect to %s:%d (internalConnected=%d mqttConnected=%d)...", 
               mqttHost.c_str(), mqttPort, internalConnected, mqttConnected);
    mqttClient.connect();
    logMsg("MQTT: connect() called");
}

// ============================================================
// OTA - setup
// ============================================================
// NOTE: Callbacki OTA działają w kontekście AsyncWebServer (inny task niż loop).
// Operacje na sprzęcie (I2C, TFT) są teraz kolejkowane do loop() przez flagi
// (otaMutePending, otaDrawPercentPending), aby uniknąć konfliktów.
void setupOTA() {
    // Endpoint odbioru pliku .bin
    server.on("/update", HTTP_POST,
        // Odpowiedź po zakończeniu
        [](AsyncWebServerRequest* req) {
            if(adminPass.length() > 0 && !req->authenticate("admin", adminPass.c_str())) {
                req->send(401, "application/json", "{\"ok\":false,\"error\":\"Unauthorized\"}");
                return;
            }
            bool success = !Update.hasError();
            
            if(success) {
                logMsg("OTA: Update SUCCESS!");
                logMsg("OTA: New firmware will boot on next restart");
            } else {
                logMsg("OTA: Update FAILED!");
                Update.printError(Serial);
            }
            
            String msg = success ? "{\"type\":\"ota_ok\"}" : "{\"type\":\"ota_fail\"}";
            notifyAll(msg);
            req->send(200, "application/json", success ? "{\"ok\":true}" : "{\"ok\":false}");
            
            if(success) {
                logMsg("OTA: Restart scheduled in 2 seconds...");
                flushAudioSettingsIfDirty(true);
                otaRestartPending = true;
                otaRestartAt = millis() + 2000;
            }
        },
        // Odbiór danych
        [](AsyncWebServerRequest* req, String filename,
           size_t index, uint8_t* data, size_t len, bool final) {
            if(adminPass.length() > 0 && !req->authenticate("admin", adminPass.c_str())) {
                if(index == 0) {
                    req->send(401, "application/json", "{\"ok\":false,\"error\":\"Unauthorized\"}");
                }
                return;
            }

            if(index == 0) {
                logMsg("OTA: Start - %s", filename.c_str());
                otaInProgress = true;
                
                // MUTE audio podczas OTA - zmniejsza interference
                // Nie wywołujemy I2C bezpośrednio z callbacku async — flaga obsługiwana w loop().
                otaMutePending = true;
                logMsg("OTA: Mute scheduled");
                
                size_t updateSize = req->contentLength();
                if(updateSize == 0) {
                    logMsg("OTA: Rejected - zero content length!");
                    otaInProgress = false;
                    return;
                }
                logMsg("OTA: Size = %u bytes", updateSize);
                
                const esp_partition_t* running = esp_ota_get_running_partition();
                const esp_partition_t* target = esp_ota_get_next_update_partition(NULL);
                if(running) {
                    logMsg("OTA: Running from: %s (0x%x)", running->label, running->address);
                }
                if(target) {
                    logMsg("OTA: Will update: %s (0x%x)", target->label, target->address);
                }
                
                if(!Update.begin(updateSize)) {
                    Update.printError(Serial);
                    logMsg("OTA: Update.begin() failed!");
                    otaInProgress = false;
                    return;
                }
                
                logMsg("OTA: Update.begin() OK - receiving data...");
            }

            if(Update.write(data, len) != len) {
                Update.printError(Serial);
            }

            // Progress - ograniczaj powiadomienia do co 5%
            if(Update.size() > 0) {
                int percent = (index + len) * 100 / req->contentLength();
                // Nie rysujemy TFT bezpośrednio z callbacku async — flaga obsługiwana w loop().
                otaDrawPercentPending = true;
                otaDrawPercentValue = percent;
                if(index == 0) otaOverlayReset = true;  // pierwszy chunk — wyczyść ekran
                static int lastPercent = -10; // -10 by zawsze wysłać pierwsze
                if (percent / 5 != lastPercent / 5) { // tylko co 5%
                    notifyAll("{\"type\":\"ota_progress\",\"percent\":" + String(percent) + "}");
                    lastPercent = percent;
                }
            }

            if(final) {
                if(Update.end(true)) {
                    logMsg("OTA: Done - %u bytes written", index + len);
                    logMsg("OTA: MD5: %s", Update.md5String().c_str());
                    
                    if(Update.hasError()) {
                        logMsg("OTA: Verification FAILED!");
                        Update.printError(Serial);
                    } else {
                        logMsg("OTA: Verification OK! Boot partition set by Update.end()");
                    }
                } else {
                    logMsg("OTA: Update.end() FAILED!");
                    Update.printError(Serial);
                }
                otaInProgress = false;
            }
        }
    );
}

// ============================================================
// WIFI SETUP
// ============================================================
void setupWifi() {
    WiFi.onEvent(WiFiEvent);
    
    // Obniż moc WiFi
    WiFi.setTxPower(WIFI_POWER_11dBm);

    if(wifiSSID.length() > 0) {
        // === TRYB STA: próba połączenia z zapisaną siecią ===
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.persistent(false);  // Nie zapisuj do flash przy każdym reconnect
        logMsg("WiFi: Connecting to %s", wifiSSID.c_str());
        showWifiOverlay("WiFi: " + wifiSSID, "", COLOR_GRAY);

        WiFi.begin(wifiSSID.c_str(), wifiPass.c_str());
        int tries = 0;
        while(WiFi.status() != WL_CONNECTED && tries < 20) { delay(500); tries++; }

        if(WiFi.status() == WL_CONNECTED) {
            localIP      = WiFi.localIP().toString();
            staConnected = true;
            logMsg("WiFi: Connected! IP: %s", localIP.c_str());
            showWifiOverlay("WiFi: " + localIP, "", COLOR_GREEN);
            delay(2000);
            clearOverlay();
            drawUI();
            return;  // Sukces — tylko STA, bez AP
        }
        // Nieudane — przejdź do trybu AP
        logMsg("WiFi: Failed, switching to AP mode");
        WiFi.disconnect();
        delay(100);
    }

    // === TRYB AP: brak zapisanej sieci lub połączenie nieudane ===
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    logMsg("WiFi: AP mode — %s", AP_SSID);
    
    // Ekran konfiguracji AP — pokaż nazwę AP i hasło
    showWifiOverlay("AP: " + String(AP_SSID), "Pass: " + String(AP_PASS), COLOR_GREEN);
    // W tym trybie nie wywołujemy clearOverlay/drawUI — 
    // użytkownik musi połączyć się z AP i skonfigurować WiFi
}

// ============================================================
// SERVER SETUP
// ============================================================
void setupServer() {
    ws.onEvent(onWSEvent);
    server.addHandler(&ws);

    // Log buffer API
    server.on("/api/logs", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "text/plain", logToText());
    });
    server.on("/api/logs/clear", HTTP_POST, [](AsyncWebServerRequest* req) {
        logClear();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send_P(200, "text/html", INDEX_HTML);
    });
    setupOTA();
    server.begin();
    logMsg("HTTP: Server started");
}

// ============================================================
// IR
// ============================================================
void handleIR(uint32_t code) {
    if(learningIR) {
        Serial.printf("IR learned: 0x%08X for %s\n", code, learningKey.c_str());
        if(learningKey == "source")       irCodes[0] = code;
        else if(learningKey == "input1")  irCodes[1] = code;
        else if(learningKey == "input2")  irCodes[2] = code;
        else if(learningKey == "input3")  irCodes[3] = code;
        else if(learningKey == "input4")  irCodes[4] = code;
        else if(learningKey == "volp")    irCodes[5] = code;
        else if(learningKey == "volm")    irCodes[6] = code;
        else if(learningKey == "up")      irCodes[7] = code;
        else if(learningKey == "down")    irCodes[8] = code;
        learningIR = false;
        clearOverlay();
        String msg = "{\"type\":\"ir_received\",\"key\":\"" + jsonEscape(learningKey) + "\",\"code\":" + String(code) + "}";
        notifyAll(msg);
        return;
    }

    // Sprawdź czy kod jest znany (zapisany w irCodes)
    bool knownCode = false;
    uint8_t codeIndex = 255;
    for(int i = 0; i < 9; i++) {
        if(code == irCodes[i] && irCodes[i] != 0) {
            knownCode = true;
            codeIndex = i;
            break;
        }
    }
    
    // Debounce: ten sam przycisk w ciągu 150ms = ignoruj (double-trigger z pilota RC6)
    static uint8_t lastCodeIndex = 255;
    static unsigned long lastHandleTime = 0;
    if(knownCode && codeIndex == lastCodeIndex && (millis() - lastHandleTime < 150)) {
        return;  // ignoruj double-trigger
    }
    
    // Zapisz kod i czas TYLKO dla znanych kodów (dla auto-repeat)
    if(knownCode) {
        lastIRCode = code;
        lastIRTime = millis();
        lastCodeIndex = codeIndex;
        lastHandleTime = millis();
    }

    if(code == irCodes[0] && irCodes[0] != 0) {
        switchInput((currentInput + 1) % 4);
        notifyAll("{\"type\":\"input_changed\",\"input\":" + String(currentInput) + "}");
    } 
    // Vol+ / Vol- - regulacja aktywnego parametru
    else if(code == irCodes[5] && irCodes[5] != 0) {
        adjustAudio(+1);  // Vol+ zwiększa aktualnie wybrany parametr
    }
    else if(code == irCodes[6] && irCodes[6] != 0) {
        adjustAudio(-1);  // Vol- zmniejsza aktualnie wybrany parametr
    }
    // ↑ / ↓ - zmiana fokusu
    else if(code == irCodes[7] && irCodes[7] != 0) {
        changeFocus(-1);  // ↑ = do góry listy
    }
    else if(code == irCodes[8] && irCodes[8] != 0) {
        changeFocus(+1);  // ↓ = w dół listy
    }
    // Direct input select
    else if(!knownCode) {
        // Nieznany kod - ignoruj (może być tail/repeat z pilota)
    }
    else {
        for(int i = 0; i < 4; i++) {
            if(code == irCodes[i+1] && irCodes[i+1] != 0) {
                switchInput(i);
                notifyAll("{\"type\":\"input_changed\",\"input\":" + String(currentInput) + "}");
                break;
            }
        }
    }
}

// ============================================================
