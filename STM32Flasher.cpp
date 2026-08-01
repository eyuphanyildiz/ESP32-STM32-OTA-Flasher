#include "STM32Flasher.h"
#include "Config.h"

STM32Flasher::STM32Flasher() {}

void STM32Flasher::begin() {
    pinMode(STM32_BOOT0_PIN, OUTPUT);
    pinMode(STM32_RESET_PIN, OUTPUT);
    digitalWrite(STM32_BOOT0_PIN, LOW);
    digitalWrite(STM32_RESET_PIN, HIGH);
    
    // AN3155 standart olarak Even Parity ile en stabil çalışır (8E1)
    STM32_UART.begin(UART_BAUD_RATE, SERIAL_8E1, STM32_RX_PIN, STM32_TX_PIN);
}

bool STM32Flasher::enterBootloader() {
    Serial.println("[STM32] Bootloader moduna geciliyor...");
    digitalWrite(STM32_BOOT0_PIN, HIGH);
    delay(50);
    digitalWrite(STM32_RESET_PIN, LOW);
    delay(50);
    digitalWrite(STM32_RESET_PIN, HIGH);
    delay(100);
    return sync();
}

void STM32Flasher::enterNormalMode() {
    Serial.println("[STM32] Normal calisma moduna donuluyor...");
    digitalWrite(STM32_BOOT0_PIN, LOW);
    delay(50);
    digitalWrite(STM32_RESET_PIN, LOW);
    delay(50);
    digitalWrite(STM32_RESET_PIN, HIGH);
    delay(100);
}

bool STM32Flasher::sync() {
    while (STM32_UART.available()) STM32_UART.read(); // Buffer'i temizle
    STM32_UART.write(0x7F);
    return waitForACK(1000);
}

bool STM32Flasher::eraseAll() {
    Serial.println("[STM32] Hafiza siliniyor (Erase Command)...");
    uint8_t cmd[] = {0x43, 0xBC}; // 0x43 XOR 0xFF = 0xBC
    STM32_UART.write(cmd, 2);
    if (!waitForACK(2000)) return false;

    // Ozel komut: Tum flasi sil (0xFF ve checksum)
    uint8_t eraseCmd[] = {0xFF, 0x00};
    STM32_UART.write(eraseCmd, 2);
    return waitForACK(15000); // Silme islemi uzun surebilir
}

bool STM32Flasher::writeMemory(uint32_t address, uint8_t *data, uint16_t length) {
    if (length > 256) return false;

    // 1. Write Command (0x31) + Checksum (0xCE)
    uint8_t cmd[] = {0x31, 0xCE};
    STM32_UART.write(cmd, 2);
    if (!waitForACK(1000)) return false;

    // 2. Adresi gonder (4 bayt + XOR Checksum)
    uint8_t addrBuf[5];
    addrBuf[0] = (address >> 24) & 0xFF;
    addrBuf[1] = (address >> 16) & 0xFF;
    addrBuf[2] = (address >> 8) & 0xFF;
    addrBuf[3] = address & 0xFF;
    addrBuf[4] = addrBuf[0] ^ addrBuf[1] ^ addrBuf[2] ^ addrBuf[3];
    STM32_UART.write(addrBuf, 5);
    if (!waitForACK(1000)) return false;

    // 3. Veriyi gonder (Uzunluk - 1 + Veri baytlari + XOR Checksum)
    uint8_t packetLen = length - 1;
    STM32_UART.write(packetLen);
    
    uint8_t checksum = packetLen;
    for (uint16_t i = 0; i < length; i++) {
        STM32_UART.write(data[i]);
        checksum ^= data[i];
    }
    STM32_UART.write(checksum);

    return waitForACK(2000);
}

bool STM32Flasher::waitForACK(unsigned long timeoutMs) {
    unsigned long start = millis();
    while (millis() - start < timeoutMs) {
        if (STM32_UART.available()) {
            uint8_t response = STM32_UART.read();
            if (response == 0x79) return true;
            if (response == 0x1F) {
                Serial.println("[STM32] NACK alindi!");
                return false;
            }
        }
    }
    Serial.println("[STM32] Zaman asimi (Timeout)!");
    return false;
}