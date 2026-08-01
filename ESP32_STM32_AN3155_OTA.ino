#include <Arduino.h>
#include "Config.h"
#include "STM32Flasher.h"
#include "WebServerManager.h"

// Nesnelerimizi oluşturuyoruz
STM32Flasher stmFlasher;
WebServerManager webManager(&stmFlasher);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- [ESP32 - STM32 AN3155 OTA PROGRAMLAYICI] ---");

    // 1. STM32 UART ve kontrol pinlerini başlat
    stmFlasher.begin();

    // 2. Wi-Fi (Captive Portal / AutoConnect) ve Web Sunucuyu Başlat
    webManager.begin();
    
    Serial.println("[SISTEM] Hazır. Tarayıcıdan IP adresine giderek flashlama yapabilirsiniz.");
}

void loop() {
    // Gelen web arayüzü ve dosya yükleme isteklerini dinle
    webManager.handleClient();
}