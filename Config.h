#ifndef CONFIG_H
#define CONFIG_H

// --- ESP32 Kurulum (Access Point) Ayarları ---
#define AP_PORTAL_SSID  "STM32_OTA_Kurulum"  // İlk kurulumda yayılacak Wi-Fi adı
#define AP_PORTAL_PASS  "12345678"           // Kurulum ağı şifresi (boş bırakılabilir: "")

// --- STM32 UART ve Pin Tanımları ---
#define STM32_TX_PIN    17 // ESP32 TX2 -> STM32 RX (PA10 vb.)
#define STM32_RX_PIN    16 // ESP32 RX2 -> STM32 TX (PA9 vb.)
#define STM32_BOOT0_PIN 4  // ESP32 GPIO4 -> STM32 BOOT0
#define STM32_RESET_PIN 5  // ESP32 GPIO5 -> STM32 NRST

// --- Haberleşme Ayarları ---
#define STM32_UART      Serial2
#define UART_BAUD_RATE  115200

// --- Sistem Ayarları ---
#define WEBSERVER_PORT  80                   // Web arayüzü portu
#define FLASH_START_ADDR 0x08000000          // STM32 Varsayılan Flash Başlangıç Adresi

#endif // CONFIG_H