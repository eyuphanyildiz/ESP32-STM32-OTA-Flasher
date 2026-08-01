#ifndef STM32_FLASHER_H
#define STM32_FLASHER_H

#include <Arduino.h>

class STM32Flasher {
public:
    STM32Flasher();
    void begin();
    bool enterBootloader();
    void enterNormalMode();
    bool sync();
    bool eraseAll();
    bool writeMemory(uint32_t address, uint8_t *data, uint16_t length);

private:
    bool waitForACK(unsigned long timeoutMs = 1000);
};

#endif // STM32_FLASHER_H