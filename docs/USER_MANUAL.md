# Kurulum ve Kullanım Kılavuzu

Projeyi ayağa kaldırmak ve STM32'ye uzaktan kod yüklemek için aşağıdaki adımları sırasıyla izleyebilirsiniz.

## 1. STM32 İçin .bin Dosyası Hazırlama
ESP32'nin bootloader ile haberleşebilmesi için STM32 kodunuzun `.hex` veya `.elf` formatında değil, `.bin` (Binary) formatında olması gerekiyor.

**STM32CubeIDE Kullanıyorsanız:**
1. Projenizin `Properties` (Özellikler) menüsünü açın.
2. `C/C++ Build -> Settings` yolunu izleyin.
3. `MCU Post-build outputs` sekmesine tıklayın.
4. **"Convert to binary file (-O binary)"** seçeneğini işaretleyip kaydedin.
5. Kodu derlediğinizde proje klasörünüzde yüklenebilir `.bin` dosyanız oluşacaktır.

## 2. İlk Wi-Fi Kurulumu
Cihazı ilk kez çalıştırdığınızda hafızasında bir ağ kayıtlı olmadığı için kendi Wi-Fi ağını açacaktır.

1. Telefonunuzdan veya bilgisayarınızdan **`STM32_OTA_Kurulum`** ağına bağlanın (Şifre: 12345678).
2. Kurulum sayfası otomatik olarak açılır (Açılmazsa tarayıcıdan 192.168.4.1 adresine gidin).
3. Ekranda çıkan listeden kendi Wi-Fi ağınızı seçin, şifresini girip kaydedin.
4. ESP32 kendini yeniden başlatacak ve girdiğiniz ağa bağlanacaktır.

## 3. Web Arayüzü ile Güncelleme
1. Bilgisayarınızı, ESP32'yi bağladığınız ağa bağlayın.
2. Tarayıcıya `http://stm32ota.local` yazarak arayüze girin. (Eğer açılmazsa Arduino IDE Seri Port ekranından ESP32'nin aldığı yerel IP adresini öğrenip onu da yazabilirsiniz).
3. Sayfadaki "Dosya Seç" kısmından az önce hazırladığınız `.bin` dosyasını seçin.
4. Yüklemeyi başlatın. Sistem STM32'nin hafızasını temizleyip yeni kodu yazacak ve işlem bittiğinde cihazı otomatik olarak yeniden başlatacaktır.

## Sık Karşılaşılan Sorunlar

**Soru:** *IP adresini tarayıcıya yazıyorum ama arayüz açılmıyor, "Bağlantı Reddedildi" hatası alıyorum. Neden?*
**Cevap:** GSB-WiFi gibi yurt ağlarında veya üniversite/şirket internetlerinde genellikle "İstemci İzolasyonu" (Client Isolation) açıktır. Bu güvenlik önlemi, aynı ağa bağlı cihazların birbirini görmesini engeller. 

* **Çözüm:** Arayüzdeki veya koddaki reset butonuyla cihazı kurulum moduna alın. Kendi telefonunuzun "İnternet Paylaşımını (Hotspot)" açın. ESP32'yi ve bilgisayarınızı kendi telefonunuzun ağına bağlayın. Cihazlar doğrudan telefonunuz üzerinden haberleşeceği için bağlantı engeli ortadan kalkacaktır.