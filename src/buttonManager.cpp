#include "buttonManager.h"
#include <Arduino.h>

buttonManager buttons;

TaskHandle_t buttonTask = NULL;
QueueHandle_t buttonQueue;
SemaphoreHandle_t buttonMutex;

// Struct for button events
typedef enum
{
    BUTTON_EVENT_PRESS
} buttonEventType;

typedef struct
{
    buttonType button;
    buttonEventType event;
} buttonEvent;

// Structure for button state with individual flags
typedef struct
{
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

buttonFlags buttonStateFlags; // Store individual button flags

// Debounce time (in milliseconds)
#define DEBOUNCE_TIME 50

// Last press times for debouncing
volatile uint32_t lastPressTimeConfirm = 0;
volatile uint32_t lastPressTimeExit = 0;
volatile uint32_t lastPressTimeUp = 0;
volatile uint32_t lastPressTimeDown = 0;
volatile uint32_t lastPressTimeTouch = 0;

bool buttonManager::checkConfirm()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.confirmFlag)
    {
        inputDetected = true;
        buttonStateFlags.confirmFlag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkExit()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.exitFlag)
    {
        inputDetected = true;
        buttonStateFlags.exitFlag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkUp()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.upFlag)
    {
        inputDetected = true;
        buttonStateFlags.upFlag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkDown()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.downFlag)
    {
        inputDetected = true;
        buttonStateFlags.downFlag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkTouch()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.touchInputFlag)
    {
        inputDetected = true;
        buttonStateFlags.touchInputFlag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkFirstSegment()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.touch1Flag)
    {
        inputDetected = true;
        buttonStateFlags.touch1Flag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkSecondSegment()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.touch2Flag)
    {
        inputDetected = true;
        buttonStateFlags.touch2Flag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkThirdSegment()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.touch3Flag)
    {
        inputDetected = true;
        buttonStateFlags.touch3Flag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkFourthSegment()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.touch4Flag)
    {
        inputDetected = true;
        buttonStateFlags.touch4Flag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkFifthSegment()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.touch5Flag)
    {
        inputDetected = true;
        buttonStateFlags.touch5Flag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkInput()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.confirmFlag || buttonStateFlags.exitFlag ||
        buttonStateFlags.upFlag || buttonStateFlags.downFlag ||
        buttonStateFlags.touchInputFlag || buttonStateFlags.inputFlag)
    {
        inputDetected = true;
        buttonStateFlags.inputFlag = false; // Reset the flag
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkButtonInput()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.confirmFlag || buttonStateFlags.exitFlag ||
        buttonStateFlags.upFlag || buttonStateFlags.downFlag ||
        buttonStateFlags.buttoninputFlag)
    {
        inputDetected = true;
        buttonStateFlags.buttoninputFlag = false; // Reset the flag
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

// Button press interrupt handlers with debounce
void IRAM_ATTR buttonConfirmPress()
{
    uint32_t currentTime = millis();
    if (currentTime - lastPressTimeConfirm > DEBOUNCE_TIME)
    {
        lastPressTimeConfirm = currentTime;
        buttonEvent event = {BUTTON_CONFIRM, BUTTON_EVENT_PRESS};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR buttonExitPress()
{
    uint32_t currentTime = millis();
    if (currentTime - lastPressTimeExit > DEBOUNCE_TIME)
    {
        lastPressTimeExit = currentTime;
        buttonEvent event = {BUTTON_EXIT, BUTTON_EVENT_PRESS};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR buttonUpPress()
{
    uint32_t currentTime = millis();
    if (currentTime - lastPressTimeUp > DEBOUNCE_TIME)
    {
        lastPressTimeUp = currentTime;
        buttonEvent event = {BUTTON_UP, BUTTON_EVENT_PRESS};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR buttonDownPress()
{
    uint32_t currentTime = millis();
    if (currentTime - lastPressTimeDown > DEBOUNCE_TIME)
    {
        lastPressTimeDown = currentTime;
        buttonEvent event = {BUTTON_DOWN, BUTTON_EVENT_PRESS};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR touchButtonHandler(gpio_num_t segment)
{
    uint32_t currentTime = millis();
    if (currentTime - lastPressTimeTouch > DEBOUNCE_TIME)
    {
        lastPressTimeTouch = currentTime;
        if (segment == TOUCH_1_SEGMENT_PIN)
        {
            buttonEvent event = {TOUCH_1, BUTTON_EVENT_PRESS};
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        else if (segment == TOUCH_2_SEGMENT_PIN)
        {
            buttonEvent event = {TOUCH_2, BUTTON_EVENT_PRESS};
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        else if (segment == TOUCH_3_SEGMENT_PIN)
        {
            buttonEvent event = {TOUCH_3, BUTTON_EVENT_PRESS};
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        else if (segment == TOUCH_4_SEGMENT_PIN)
        {
            buttonEvent event = {TOUCH_4, BUTTON_EVENT_PRESS};
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        else if (segment == TOUCH_5_SEGMENT_PIN)
        {
            buttonEvent event = {TOUCH_5, BUTTON_EVENT_PRESS};
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

void IRAM_ATTR touchHandler1() { touchButtonHandler(TOUCH_1_SEGMENT_PIN); }
void IRAM_ATTR touchHandler2() { touchButtonHandler(TOUCH_2_SEGMENT_PIN); }
void IRAM_ATTR touchHandler3() { touchButtonHandler(TOUCH_3_SEGMENT_PIN); }
void IRAM_ATTR touchHandler4() { touchButtonHandler(TOUCH_4_SEGMENT_PIN); }
void IRAM_ATTR touchHandler5() { touchButtonHandler(TOUCH_5_SEGMENT_PIN); }

// Task to Process Button Events
void buttonManagerTask(void *pvParameters)
{
    buttonEvent event;
    while (true)
    {
        if (xQueueReceive(buttonQueue, &event, portMAX_DELAY))
        {
            xSemaphoreTake(buttonMutex, portMAX_DELAY);
            switch (event.button)
            {
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
            xSemaphoreGive(buttonMutex);
        }
        delay(5);
    }
}

// Initialization Function
void initbuttonManager()
{
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
    buttonMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(
        buttonManagerTask, "buttonManager", 10000, NULL, 5, &buttonTask, 1);
}

void buttonManager::createTask()
{
    initbuttonManager();
}
