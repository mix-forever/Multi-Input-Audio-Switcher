// audio_ctrl.cpp
#include "globals.h"
#include "config.h"

// AUDIO ADJUSTMENT
// ============================================================
void adjustAudio(int delta) {
    // Reguluje parametr wskazywany przez audioFocus
    if(audioFocus == 0) {
        // Volume w skali użytkowej: 0=cicho/mute, 56=maks.
        // Krok 3 dla szybszej regulacji z pilota (bez repeat)
        int step = (delta > 0) ? 3 : -3;
        int newVol = (int)audioVolume + step;
        if(newVol < 0) newVol = 0;
        if(newVol > 56) newVol = 56;  // Max 56!
        audioVolume = (uint8_t)newVol;
        audio.setVolume(audioVolume);
        logMsg("Volume: %d (raw) = %d%%", audioVolume, audioVolume*100/56);
    }
    else if(audioFocus == 1) {
        // Bass/Treble pracują w krokach 2 dB obsługiwanych przez PT2314.
        int newBass = audioBass + (delta * 2);
        audioBass = normalizeToneDb(newBass);
        audio.setBass(audioBass);
        logMsg("Bass: %+d dB", audioBass);
    }
    else if(audioFocus == 2) {
        // Bass/Treble pracują w krokach 2 dB obsługiwanych przez PT2314.
        int newTreble = audioTreble + (delta * 2);
        audioTreble = normalizeToneDb(newTreble);
        audio.setTreble(audioTreble);
        logMsg("Treble: %+d dB", audioTreble);
    }
    
    notifyAudioChanged();
    
    // Zapis do NVS jest opóźniony, żeby IR auto-repeat nie zużywał flash.
    markAudioSettingsDirty();
    
    // Publikuj do MQTT
    mqttPublishAudio();
    
    audioOverlayActive = true;
    audioOverlayStart = millis();
    showAudioOverlay();
}

void changeFocus(int delta) {
    audioFocus = (audioFocus + delta + 3) % 3;  // 0-2 z wraparound
    resetAudioOverlay();  // Zmiana fokusu - wymaga pełnego redraw
    audioOverlayActive = true;
    audioOverlayStart = millis();
    showAudioOverlay();
    Serial.printf("Focus: %d (Vol/Bass/Treb)\n", audioFocus);
}

// ============================================================
// AUDIO
// ============================================================
void initAudio(uint8_t input) {
    logMsg("PT2314E: Initializing...");
    logMsg("PT2314E: audioBass=%d audioTreble=%d audioVolume=%d", audioBass, audioTreble, audioVolume);
    
    // Soft-start: najpierw mute w skali użytkowej.
    audio.setVolume(0);
    delay(100);
    
    // Ustaw parametry
    audio.setBass(audioBass);
    audio.setTreble(audioTreble);
    audio.setAttL(0);  // Lewy kanał: 0dB
    audio.setAttR(0);  // Prawy kanał: 0dB
    
    // Próba z gain +3.75dB (usil=1) - może poprawić SNR
    audio.setSwitch(input, 0, 1);  // Input, Loudness OFF, Gain +3.75dB
    delay(50);
    
    // Teraz ustaw docelową głośność
    audio.setVolume(audioVolume);
    
    logMsg("PT2314E: Input=%d, Vol=%d (raw), Bass=%+d, Treble=%+d, Gain=+3.75dB",
            input, audioVolume, audioBass, audioTreble);
}

// ============================================================
// MQTT - publish
// ============================================================
void mqttPublishInput(uint8_t input) {
    if(!mqttConnected) return;
    char topic[64];
    char payload[4];
    snprintf(topic, sizeof(topic), "%s/state", mqttTopic.c_str());
    snprintf(payload, sizeof(payload), "%d", input + 1);
    mqttClient.publish(topic, 0, true, payload);
    Serial.printf("MQTT pub: %s = %s\n", topic, payload);
}

void mqttPublishStatus(const char* status) {
    if(!mqttConnected) return;
    char topic[64];
    snprintf(topic, sizeof(topic), "%s/status", mqttTopic.c_str());
    mqttClient.publish(topic, 0, true, status);
}

void mqttPublishAudio() {
    if(!mqttConnected) return;
    
    char topic[64];
    char payload[96];
    int volPercent = audioVolume * 100 / 56;
    snprintf(topic, sizeof(topic), "%s/audio", mqttTopic.c_str());
    snprintf(payload, sizeof(payload),
             "{\"volume\":%d,\"volume_raw\":%d,\"bass\":%d,\"treble\":%d}",
             volPercent, audioVolume, audioBass, audioTreble);
    
    mqttClient.publish(topic, 0, false, payload);
    Serial.printf("MQTT pub: %s = %s\n", topic, payload);
}

// ============================================================
// INPUT SWITCH
// ============================================================
void switchInput(uint8_t newInput) {
    if(newInput > 3) return;
    uint8_t old = currentInput;
    currentInput = newInput;
    audio.setSwitch(currentInput, 0, 1);  // Gain +3.75dB (test SNR)
    updateInput(old, currentInput);
    saveLastInput(currentInput);
    applyRelays(currentInput);
    mqttPublishInput(currentInput);
    Serial.printf("Input: %d [%s]\n", currentInput + 1, INPUT_NAMES[currentInput]);
}

// ============================================================
