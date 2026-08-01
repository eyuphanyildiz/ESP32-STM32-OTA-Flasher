#include "WebServerManager.h"
#include "Config.h"
#include <ESPmDNS.h>
// LittleFS kütüphanesini sildik, arayüzü doğrudan koda gömdük!

WebServerManager::WebServerManager(STM32Flasher* flasher) 
    : server(WEBSERVER_PORT), stmFlasher(flasher), isFlashing(false), currentAddress(FLASH_START_ADDR) {}

void WebServerManager::begin() {
    Serial.println("[WIFI] Kayıtlı Wi-Fi ağına bağlanılıyor...");

    WiFi.mode(WIFI_STA);
    WiFi.begin();
    
    unsigned long startAttemptTime = millis();
    bool connected = false;

    while (millis() - startAttemptTime < 10000) {
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            break;
        }
        delay(500);
        Serial.print(".");
    }

    if (!connected) {
        Serial.println("\n[WIFI] Kayıtlı ağa ulaşılamadı! Akıllı Kurulum Portalı açılıyor...");
        WiFiManager wm;
        WiFiManagerParameter custom_html("<p style='color:red;'>Ağa bağlanılamadı. Lütfen yeni Wi-Fi seçin:</p>");
        wm.addParameter(&custom_html);

        if (!wm.startConfigPortal(AP_PORTAL_SSID, AP_PORTAL_PASS)) {
            Serial.println("[HATA] Zaman aşımı! Sistem yeniden başlatılıyor...");
            delay(3000);
            ESP.restart();
        }
    }

    Serial.print("\n[WIFI] Bağlantı Başarılı! ESP32 IP Adresi: ");
    Serial.println(WiFi.localIP());

    // mDNS Başlat (http://stm32ota.local)
    if (MDNS.begin("stm32ota")) {
        Serial.println("[MDNS] Başlatıldı. Tarayıcıdan http://stm32ota.local yazarak girebilirsiniz.");
    }

    // HTTP Sunucu Route'ları
    setupRoutes();
    server.begin();
    Serial.println("[WEB] HTTP Sunucusu yayında.");
}

void WebServerManager::setupRoutes() {
    // DOĞRUDAN KODA GÖMÜLÜ ŞIK HTML TASARIMI
    server.on("/", HTTP_GET, [this]() { 
        String html = R"rawliteral(
        <!DOCTYPE html><html lang="tr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>STM32 OTA Programlayıcı</title>
        <style>
            body { font-family: Arial, sans-serif; background: #f4f7f6; display: flex; justify-content: center; padding: 20px; }
            .card { background: white; padding: 25px; border-radius: 10px; box-shadow: 0 4px 15px rgba(0,0,0,0.05); max-width: 500px; width: 100%; margin-bottom: 20px; }
            .btn { padding: 12px 15px; border: none; border-radius: 6px; cursor: pointer; color: white; font-weight: bold; width: 100%; margin-top: 10px; font-size: 15px;}
            .btn-blue { background: #3498db; } .btn-blue:hover { background: #2980b9; }
            .btn-red { background: #e74c3c; } .btn-red:hover { background: #c0392b; }
            .file-box { border: 2px dashed #bdc3c7; padding: 30px; text-align: center; margin: 15px 0; border-radius: 8px; background: #fafbfc;}
        </style></head><body><div>
        
        <div class="card">
            <h2 style="color:#2c3e50; text-align:center;">STM32 Uzaktan Yükleyici</h2>
            <p style="color:#7f8c8d; text-align:center;">AN3155 UART Bootloader Portalı</p>
            <form method="POST" action="/upload" enctype="multipart/form-data">
                <div class="file-box">
                    <input type="file" name="update" accept=".bin" required>
                </div>
                <button type="submit" class="btn btn-blue">STM32'yi Güncelle</button>
            </form>
        </div>

        <div class="card" style="border-left: 5px solid #e74c3c;">
            <h3 style="color:#34495e;">Ağ Ayarları</h3>
            <p style="color:#666; font-size: 14px;">Farklı bir ağa geçmek istiyorsanız cihazı sıfırlayın:</p>
            <form method="POST" action="/reset-wifi" onsubmit="return confirm('Wi-Fi sıfırlanacak ve Kurulum Moduna geçilecek. Emin misiniz?');">
                <button type="submit" class="btn btn-red">Wi-Fi Ayarlarını Sıfırla</button>
            </form>
        </div>
        
        </div></body></html>
        )rawliteral";
        server.send(200, "text/html", html);
    });

    // Wi-Fi Ayarlarını Sıfırlama
    server.on("/reset-wifi", HTTP_POST, [this]() { 
        server.send(200, "text/html", "<h2>Wi-Fi Ayarları Sifirlandi!</h2><p>Cihaz yeniden baslatiliyor...</p>");
        delay(2000);
        resetWiFiSettings();
        ESP.restart();
    });

    // Firmware Upload Endpoint'i
    server.on("/upload", HTTP_POST, 
        [this]() { handleUploadEnd(); },
        [this]() { handleUpload(); }
    );
}

void WebServerManager::handleUpload() {
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("[OTA] Yükleme Basladi: %s\n", upload.filename.c_str());
        currentAddress = FLASH_START_ADDR;
        
        if (stmFlasher->enterBootloader() && stmFlasher->eraseAll()) {
            isFlashing = true;
            Serial.println("[OTA] STM32 Silindi, veri yazilmaya hazir...");
        } else {
            isFlashing = false;
            Serial.println("[HATA] STM32 Bootloader moduna gecemedi veya silinemedi!");
        }
    } 
    else if (upload.status == UPLOAD_FILE_WRITE) {
        if (isFlashing) {
            uint8_t* data = upload.buf;
            size_t len = upload.currentSize;
            size_t offset = 0;

            while (offset < len) {
                size_t chunkSize = min((size_t)256, len - offset);
                if (!stmFlasher->writeMemory(currentAddress, data + offset, chunkSize)) {
                    Serial.println("[HATA] STM32 yazma hatasi!");
                    isFlashing = false;
                    break;
                }
                currentAddress += chunkSize;
                offset += chunkSize;
            }
        }
    } 
    else if (upload.status == UPLOAD_FILE_END) {
        if (isFlashing) {
            Serial.printf("[OTA] Yükleme Tamamlandi! Toplam Bayt: %u\n", upload.totalSize);
            stmFlasher->enterNormalMode();
        }
    }
}

void WebServerManager::handleUploadEnd() {
    if (isFlashing) {
        server.send(200, "text/html", "<h2>Yükleme Basarili! STM32 Yeniden Baslatildi.</h2><br><a href='/'>Geri Don</a>");
    } else {
        server.send(500, "text/html", "<h2>Yükleme Basarisiz! Baglantilari Kontrol Edin.</h2><br><a href='/'>Geri Don</a>");
    }
}

void WebServerManager::handleClient() {
    server.handleClient();
}

void WebServerManager::resetWiFiSettings() {
    WiFiManager wm;
    wm.resetSettings();
    Serial.println("[WIFI] Kayitli Wi-Fi ayarlari silindi.");
}