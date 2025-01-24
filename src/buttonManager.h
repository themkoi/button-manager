#ifndef OLEDMANAGER_H
#define OLEDMANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// ButtonPins
#define BUTTON_UP_PIN      GPIO_NUM_12
#define BUTTON_DOWN_PIN    GPIO_NUM_13
#define BUTTON_CONFIRM_PIN GPIO_NUM_25
#define BUTTON_EXIT_PIN    GPIO_NUM_26

// Enum for actions
typedef enum
{
    BUTTON_CONFIRM,
    BUTTON_EXIT,
    BUTTON_UP,
    BUTTON_DOWN,
} Action;

// Struct for action data
typedef struct
{
    Action button;
} buttonState;

// OledManager class definition
class buttonManager
{
public:
    void oledDisplay();
    void oledFadeOut();
    void oledFadeIn();
    void oledDisable();
    void oledEnable();
    void startScrollingLeft(uint8_t startPage, uint8_t endPage, uint8_t speed);
    void startScrollingRight(uint8_t startPage, uint8_t endPage, uint8_t speed);
    void stopScrolling();
    void sendCustomCommand(uint8_t command);
    void createTask();

private:
    void handleAction(const ActionData &actionData);
};

extern buttonManager buttons;

#endif // OLEDMANAGER_H
