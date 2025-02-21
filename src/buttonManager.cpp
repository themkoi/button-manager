#include "buttonManager.h"
#include <Arduino.h>

buttonManager buttons;

TaskHandle_t buttonTask = NULL;
QueueHandle_t buttonQueue;
SemaphoreHandle_t buttonSemaphore;

typedef enum {
    BUTTON_EVENT_PRESS
} buttonEventType;

typedef struct {
    buttonType button;
    buttonEventType event;
} buttonEvent;

typedef struct {
    bool confirmFlag;
    bool exitFlag;
    bool upFlag;
    bool downFlag;
    bool touch1Flag;
    bool touch2Flag;
    bool touch3Flag;
    bool touch4Flag;
    bool touch5Flag;
    bool inputFlag;
    bool buttoninputFlag;
    bool touchInputFlag;
} buttonFlags;

buttonFlags buttonStateFlags;

#define DEBOUNCE_TIME 100

volatile uint32_t lastPressTimeConfirm = 0;
volatile uint32_t lastPressTimeExit = 0;
volatile uint32_t lastPressTimeUp = 0;
volatile uint32_t lastPressTimeDown = 0;
volatile uint32_t lastPressTimeTouch = 0;

bool buttonManager::checkConfirm() {
    delay(1);
    if (buttonStateFlags.confirmFlag) {
        buttonStateFlags.confirmFlag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkExit() {
    delay(1);
    if (buttonStateFlags.exitFlag) {
        buttonStateFlags.exitFlag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkUp() {
    delay(1);
    if (buttonStateFlags.upFlag) {
        buttonStateFlags.upFlag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkDown() {
    delay(1);
    if (buttonStateFlags.downFlag) {
        buttonStateFlags.downFlag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkTouch() {
    delay(1);
    if (buttonStateFlags.touchInputFlag) {
        buttonStateFlags.touchInputFlag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkFirstSegment() {
    delay(1);
    if (buttonStateFlags.touch1Flag) {
        buttonStateFlags.touch1Flag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkSecondSegment() {
    delay(1);
    if (buttonStateFlags.touch2Flag) {
        buttonStateFlags.touch2Flag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkThirdSegment() {
    delay(1);
    if (buttonStateFlags.touch3Flag) {
        buttonStateFlags.touch3Flag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkFourthSegment() {
    delay(1);
    if (buttonStateFlags.touch4Flag) {
        buttonStateFlags.touch4Flag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkFifthSegment() {
    delay(1);
    if (buttonStateFlags.touch5Flag) {
        buttonStateFlags.touch5Flag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkButtonInput() {
    delay(1);
    if (buttonStateFlags.buttoninputFlag) {
        buttonStateFlags.buttoninputFlag = false;
        return true;
    }
    return false;
}

bool buttonManager::checkInput() {
    delay(1);
    if (buttonStateFlags.inputFlag) {
        buttonStateFlags.inputFlag = false;
        return true;
    }
    return false;
}


// ISR to handle button presses and queue the event
void IRAM_ATTR buttonISR(buttonType btnType, volatile uint32_t &lastPressTime) {
    uint32_t currentTime = millis();
    if (currentTime - lastPressTime > DEBOUNCE_TIME) {
        lastPressTime = currentTime;
        buttonEvent event = {btnType, BUTTON_EVENT_PRESS};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
        xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR buttonConfirmPress() { buttonISR(BUTTON_CONFIRM, lastPressTimeConfirm); }
void IRAM_ATTR buttonExitPress() { buttonISR(BUTTON_EXIT, lastPressTimeExit); }
void IRAM_ATTR buttonUpPress() { buttonISR(BUTTON_UP, lastPressTimeUp); }
void IRAM_ATTR buttonDownPress() { buttonISR(BUTTON_DOWN, lastPressTimeDown); }

// Touch Handler
void IRAM_ATTR touchButtonHandler(buttonType touchBtn) {
    uint32_t currentTime = millis();
    if (currentTime - lastPressTimeTouch > DEBOUNCE_TIME) {
        lastPressTimeTouch = currentTime;
        buttonEvent event = {touchBtn, BUTTON_EVENT_PRESS};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
        xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR touchHandler1() { touchButtonHandler(TOUCH_1); }
void IRAM_ATTR touchHandler2() { touchButtonHandler(TOUCH_2); }
void IRAM_ATTR touchHandler3() { touchButtonHandler(TOUCH_3); }
void IRAM_ATTR touchHandler4() { touchButtonHandler(TOUCH_4); }
void IRAM_ATTR touchHandler5() { touchButtonHandler(TOUCH_5); }

void buttonManagerTask(void *pvParameters) {
    buttonEvent event;
    while (true) {
        if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY)) {
            while (xQueueReceive(buttonQueue, &event, 0)) {  
                switch (event.button) {
                    case BUTTON_CONFIRM:
                        buttonStateFlags.confirmFlag = true;
                        buttonStateFlags.buttoninputFlag = true;
                        buttonStateFlags.inputFlag = true;
                        break;
                    case BUTTON_EXIT:
                        buttonStateFlags.exitFlag = true;
                        buttonStateFlags.buttoninputFlag = true;
                        buttonStateFlags.inputFlag = true;
                        break;
                    case BUTTON_UP:
                        buttonStateFlags.upFlag = true;
                        buttonStateFlags.buttoninputFlag = true;
                        buttonStateFlags.inputFlag = true;
                        break;
                    case BUTTON_DOWN:
                        buttonStateFlags.downFlag = true;
                        buttonStateFlags.buttoninputFlag = true;
                        buttonStateFlags.inputFlag = true;
                        break;
                    case TOUCH_1:
                        buttonStateFlags.touch1Flag = true;
                        buttonStateFlags.inputFlag = true;
                        buttonStateFlags.touchInputFlag = true;
                        break;
                    case TOUCH_2:
                        buttonStateFlags.touch2Flag = true;
                        buttonStateFlags.inputFlag = true;
                        buttonStateFlags.touchInputFlag = true;
                        break;
                    case TOUCH_3:
                        buttonStateFlags.touch3Flag = true;
                        buttonStateFlags.inputFlag = true;
                        buttonStateFlags.touchInputFlag = true;
                        break;
                    case TOUCH_4:
                        buttonStateFlags.touch4Flag = true;
                        buttonStateFlags.inputFlag = true;
                        buttonStateFlags.touchInputFlag = true;
                        break;
                    case TOUCH_5:
                        buttonStateFlags.touch5Flag = true;
                        buttonStateFlags.inputFlag = true;
                        buttonStateFlags.touchInputFlag = true;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void initbuttonManager() {
    attachInterrupt(BUTTON_CONFIRM_PIN, buttonConfirmPress, RISING);
    attachInterrupt(BUTTON_EXIT_PIN, buttonExitPress, RISING);
    attachInterrupt(BUTTON_UP_PIN, buttonUpPress, RISING);
    attachInterrupt(BUTTON_DOWN_PIN, buttonDownPress, RISING);
    touchAttachInterrupt(TOUCH_1_SEGMENT_PIN, touchHandler1, TOUCH_1_SEGMENT_THRESHOLD);
    touchAttachInterrupt(TOUCH_2_SEGMENT_PIN, touchHandler2, TOUCH_2_SEGMENT_THRESHOLD);
    touchAttachInterrupt(TOUCH_3_SEGMENT_PIN, touchHandler3, TOUCH_3_SEGMENT_THRESHOLD);
    touchAttachInterrupt(TOUCH_4_SEGMENT_PIN, touchHandler4, TOUCH_4_SEGMENT_THRESHOLD);
    touchAttachInterrupt(TOUCH_5_SEGMENT_PIN, touchHandler5, TOUCH_5_SEGMENT_THRESHOLD);

    buttonQueue = xQueueCreate(10, sizeof(buttonEvent));
    buttonSemaphore = xSemaphoreCreateBinary();  // Binary semaphore for ISR sync

    xTaskCreatePinnedToCore(buttonManagerTask, "buttonManager", 10000, NULL, 5, &buttonTask, 1);
}

void buttonManager::createTask() {
    initbuttonManager();
}
