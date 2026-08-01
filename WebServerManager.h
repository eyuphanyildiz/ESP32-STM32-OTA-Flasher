#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h> // tzapu/WiFiManager kütüphanesi kullanılmalı
#include "STM32Flasher.h"

class WebServerManager {
public:
    WebServerManager(STM32Flasher* flasher);
    void begin();
    void handleClient();
    void resetWiFiSettings(); // Fabrika ayarlarına dönüş için

private:
    WebServer server;
    STM32Flasher* stmFlasher;
    bool isFlashing;
    uint32_t currentAddress;

    void setupRoutes();
    void handleRoot();
    void handleUpload();
    void handleUploadEnd();
};

#endif // WEBSERVER_MANAGER_H