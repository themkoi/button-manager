#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// ButtonPins
#define BUTTON_UP_PIN GPIO_NUM_12
#define BUTTON_DOWN_PIN GPIO_NUM_13
#define BUTTON_CONFIRM_PIN GPIO_NUM_25
#define BUTTON_EXIT_PIN GPIO_NUM_26

#define TOUCH_1_SEGMENT_PIN GPIO_NUM_33
#define TOUCH_1_SEGMENT_THRESHOLD 23
#define TOUCH_2_SEGMENT_PIN GPIO_NUM_4
#define TOUCH_2_SEGMENT_THRESHOLD 25
#define TOUCH_3_SEGMENT_PIN GPIO_NUM_32
#define TOUCH_3_SEGMENT_THRESHOLD 25
#define TOUCH_4_SEGMENT_PIN GPIO_NUM_27
#define TOUCH_4_SEGMENT_THRESHOLD 27
#define TOUCH_5_SEGMENT_PIN GPIO_NUM_2
#define TOUCH_5_SEGMENT_THRESHOLD 33

#define TOUCH_1_SEGMENT_THRESHOLD_BAT 30
#define TOUCH_2_SEGMENT_THRESHOLD_BAT 30
#define TOUCH_3_SEGMENT_THRESHOLD_BAT 30
#define TOUCH_4_SEGMENT_THRESHOLD_BAT 35
#define TOUCH_5_SEGMENT_THRESHOLD_BAT 40

#define TOUCH_1_SEGMENT_THRESHOLD_SLEEP 27
#define TOUCH_2_SEGMENT_THRESHOLD_SLEEP 27
#define TOUCH_3_SEGMENT_THRESHOLD_SLEEP 27
#define TOUCH_4_SEGMENT_THRESHOLD_SLEEP 30
#define TOUCH_5_SEGMENT_THRESHOLD_SLEEP 32

// Enum for actions
typedef enum
{
    BUTTON_CONFIRM,
    BUTTON_EXIT,
    BUTTON_UP,
    BUTTON_DOWN,
    TOUCH_1,
    TOUCH_2,
    TOUCH_3,
    TOUCH_4,
    TOUCH_5,
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
    bool checkFirstSegment();
    bool checkSecondSegment();
    bool checkThirdSegment();
    bool checkFourthSegment();
    bool checkFifthSegment();
    void createTask();
    bool checkButtonInput();
    bool checkInput();
    void setTouchInterrupt(int segmentPin, int threshold);

private:
};

extern buttonManager buttons;

#endif // OLEDMANAGER_H
