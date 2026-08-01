# STM32 OTA Flasher via ESP32

Bu proje, ESP32 kullanarak STM32 serisi mikrodenetleyicilere Wi-Fi üzerinden (uzaktan) kod yüklemenizi sağlar. STM32'nin AN3155 UART Bootloader protokolünü temel alır. 

Özellikle STM32'ye kablo çekmenin zor olduğu, kapalı kutu içinde çalışan veya uzaktan güncellenmesi gereken donanım projeleri için geliştirilmiştir.

## Neler Yapabiliyor?
* **Otomatik Bootloader Kontrolü:** ESP32, STM32'nin BOOT0 ve RESET pinlerini kendisi yönetir. Kodu yüklerken fiziksel olarak hiçbir butona basmanıza gerek kalmaz.
* **Akıllı Kurulum (Captive Portal):** Cihazı yeni bir ortama götürdüğünüzde kod değiştirmenize gerek yok. Kayıtlı bir ağ bulamazsa kendi Wi-Fi ağını açar ve telefondan yeni şifreyi girmenizi sağlar.
* **Kolay Erişim:** mDNS desteği sayesinde IP adresi ezberlemek yerine doğrudan `http://stm32ota.local` yazarak arayüze ulaşabilirsiniz.
* **Gömülü Web Arayüzü:** HTML/CSS kodları doğrudan C++ içine gömüldüğü için LittleFS veya SPIFFS gibi ekstra dosya sistemleriyle uğraşmanıza gerek kalmaz. Sadece ana kodu yükleyip çalıştırabilirsiniz.

## Bağlantı Şeması

| ESP32 Pini | STM32 Pini | Görev |
| :--- | :--- | :--- |
| GPIO 17 (TX2) | PA10 (RX) | ESP32'den STM32'ye veri gönderimi |
| GPIO 16 (RX2) | PA9 (TX)  | STM32'den dönen onay (ACK) paketlerini okuma |
| GPIO 4 | BOOT0 | STM32'yi yazılım yükleme moduna alma |
| GPIO 5 | NRST | STM32'yi yeniden başlatma |
| GND | GND | Ortak toprak (Sistemin kararlı çalışması için şarttır) |

## Bağımlılıklar
Kodu derlemeden önce Arduino IDE üzerinden `WiFiManager` (tzapu) kütüphanesini kurmanız gerekmektedir.