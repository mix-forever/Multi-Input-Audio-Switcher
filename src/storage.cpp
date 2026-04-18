// ============================================================
// STORAGE - NVS / Preferences
// ============================================================
#include "globals.h"
#include "config.h"

void loadPrefs() {
    prefs.begin("switcher", false);

    wifiSSID     = prefs.getString("wifi_ssid",  "");
    wifiPass     = prefs.getString("wifi_pass",  "");
    currentInput = normalizeInputIndex(prefs.getUChar("last_input",  0));
    brightness   = normalizeBrightness(prefs.getUChar("brightness",  BL_DEFAULT));
    dispInvert   = prefs.getBool("disp_invert",  false);
    dispRotate   = prefs.getBool("disp_rotate",  false);

    irCodes[0]   = prefs.getUInt("ir_source",  0);
    irCodes[1]   = prefs.getUInt("ir_input1",  0);
    irCodes[2]   = prefs.getUInt("ir_input2",  0);
    irCodes[3]   = prefs.getUInt("ir_input3",  0);
    irCodes[4]   = prefs.getUInt("ir_input4",  0);
    irCodes[5]   = prefs.getUInt("ir_volp",  0);
    irCodes[6]   = prefs.getUInt("ir_volm",  0);
    irCodes[7]   = prefs.getUInt("ir_up",    0);
    irCodes[8]   = prefs.getUInt("ir_down",  0);

    mqttHost     = prefs.getString("mqtt_host",  "");
    mqttPort     = normalizeMqttPort(prefs.getUShort("mqtt_port",  1883));
    mqttUser     = prefs.getString("mqtt_user",  "");
    mqttPass     = prefs.getString("mqtt_pass",  "");
    mqttTopic    = normalizeMqttTopic(prefs.getString("mqtt_topic", MQTT_TOPIC_DEFAULT));
    mqttEnabled  = prefs.getBool("mqtt_en",      false);

    adminPass    = prefs.getString("admin_pass", "");

    relayEnabled[0] = prefs.getBool("relay_en0", false);
    relayEnabled[1] = prefs.getBool("relay_en1", false);
    relayEnabled[2] = prefs.getBool("relay_en2", false);
    relayEnabled[3] = prefs.getBool("relay_en3", false);

    int8_t rawBass = prefs.getChar("audio_bass", 0);
    int8_t rawTreb = prefs.getChar("audio_treb", 0);
    audioVolume = normalizeVolume(prefs.getUChar("audio_vol",  0));
    audioBass   = normalizeToneDb(rawBass);
    audioTreble = normalizeToneDb(rawTreb);
    
    // Sanity check: jeśli wartości są skrajne, zresetuj do 0
    // (możliwe że NVS ma śmieci ze starej wersji firmware)
    if(audioBass < -10 || audioBass > 10) {
        logMsg("NVS: bass=%d out of range, resetting to 0", audioBass);
        audioBass = 0;
        prefs.putChar("audio_bass", 0);
    }
    if(audioTreble < -10 || audioTreble > 10) {
        logMsg("NVS: treble=%d out of range, resetting to 0", audioTreble);
        audioTreble = 0;
        prefs.putChar("audio_treb", 0);
    }
    
    logMsg("NVS: rawBass=%d rawTreb=%d -> bass=%d treb=%d", rawBass, rawTreb, audioBass, audioTreble);

    prefs.end();
    Serial.println("NVS: Prefs loaded");
}

void saveLastInput(uint8_t input) {
    prefs.begin("switcher", false);
    prefs.putUChar("last_input", input);
    prefs.end();
}

void saveBrightness(uint8_t val) {
    prefs.begin("switcher", false);
    prefs.putUChar("brightness", val);
    prefs.end();
}

void saveDisplaySettings() {
    prefs.begin("switcher", false);
    prefs.putBool("disp_invert", dispInvert);
    prefs.putBool("disp_rotate", dispRotate);
    prefs.end();
}

void saveIRCodes() {
    prefs.begin("switcher", false);
    prefs.putUInt("ir_source", irCodes[0]);
    prefs.putUInt("ir_input1", irCodes[1]);
    prefs.putUInt("ir_input2", irCodes[2]);
    prefs.putUInt("ir_input3", irCodes[3]);
    prefs.putUInt("ir_input4", irCodes[4]);
    prefs.putUInt("ir_volp",  irCodes[5]);
    prefs.putUInt("ir_volm",  irCodes[6]);
    prefs.putUInt("ir_up",    irCodes[7]);
    prefs.putUInt("ir_down",  irCodes[8]);
    prefs.end();
    Serial.println("NVS: IR codes saved");
}

void saveWifiCreds(const String& ssid, const String& pass) {
    prefs.begin("switcher", false);
    prefs.putString("wifi_ssid", ssid);
    prefs.putString("wifi_pass", pass);
    prefs.end();
    Serial.println("NVS: WiFi saved");
}

void saveMqttSettings() {
    prefs.begin("switcher", false);
    prefs.putString("mqtt_host",  mqttHost);
    prefs.putUShort("mqtt_port",  mqttPort);
    prefs.putString("mqtt_user",  mqttUser);
    prefs.putString("mqtt_pass",  mqttPass);
    prefs.putString("mqtt_topic", mqttTopic);
    prefs.putBool("mqtt_en",      mqttEnabled);
    prefs.end();
    Serial.println("NVS: MQTT saved");
}

void saveRelayEnabled() {
    prefs.begin("switcher", false);
    prefs.putBool("relay_en0", relayEnabled[0]);
    prefs.putBool("relay_en1", relayEnabled[1]);
    prefs.putBool("relay_en2", relayEnabled[2]);
    prefs.putBool("relay_en3", relayEnabled[3]);
    prefs.end();
    Serial.println("NVS: Relay config saved");
}

void saveAdminPass(const String& pass) {
    prefs.begin("switcher", false);
    prefs.putString("admin_pass", pass);
    prefs.end();
    adminPass = pass;
    Serial.println("NVS: Admin pass saved");
}

// saveApPass removed — AP password is always AP_PASS

void saveAudioSettings() {
    prefs.begin("switcher", false);
    prefs.putUChar("audio_vol",  audioVolume);
    prefs.putChar("audio_bass",  audioBass);
    prefs.putChar("audio_treb",  audioTreble);
    prefs.end();
    Serial.println("NVS: Audio settings saved");
}

void markAudioSettingsDirty() {
    audioSettingsDirty = true;
    audioSettingsDirtyAt = millis();
}

void flushAudioSettingsIfDirty(bool force) {
    if(!audioSettingsDirty) return;
    if(!force && (millis() - audioSettingsDirtyAt < AUDIO_SAVE_DELAY)) return;
    saveAudioSettings();
    audioSettingsDirty = false;
}
