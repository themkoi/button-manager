#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// ButtonPins
#define BUTTON_UP_PIN GPIO_NUM_12
#define BUTTON_DOWN_PIN GPIO_NUM_13
#define BUTTON_CONFIRM_PIN GPIO_NUM_25
#define BUTTON_EXIT_PIN GPIO_NUM_26

#define TOUCH_BUTTON_PIN 4
#define TOUCH_BUTTON_THRESHOLD 8

// Enum for actions
typedef enum
{
    BUTTON_CONFIRM,
    BUTTON_EXIT,
    BUTTON_UP,
    BUTTON_DOWN,
    TOUCH_BUTTON,
    NO_BUTTON
} buttonType;

// Struct for action data
typedef struct
{
    buttonType button;
} buttonState;

typedef struct
{
    bool buttonConfirm = false;
    bool buttonExit = false;
    bool buttonUp = false;
    bool buttonDown = false;
} buttonStates;

class buttonManager
{
public:
    bool checkConfirm();
    bool checkExit();
    bool checkUp();
    bool checkDown();
    bool checkTouch();
    void createTask();
    bool checkButtonInput();
bool checkInput(); private:
};

extern buttonManager buttons;

#endif // OLEDMANAGER_H
