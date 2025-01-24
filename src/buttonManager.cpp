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
    bool touchFlag;
} buttonFlags;

buttonFlags buttonStateFlags; // Store individual button flags

// Debounce time (in milliseconds)
#define DEBOUNCE_TIME 150

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

    if (buttonStateFlags.touchFlag)
    {
        inputDetected = true;
        buttonStateFlags.touchFlag = false;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

// Checks for any input (including touch)
bool buttonManager::checkInput()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.confirmFlag || buttonStateFlags.exitFlag ||
        buttonStateFlags.upFlag || buttonStateFlags.downFlag ||
        buttonStateFlags.touchFlag)
    {
        inputDetected = true;
    }
    delay(1);

    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

// Checks for button input only (excluding touch)
bool buttonManager::checkButtonInput()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    delay(1);

    if (buttonStateFlags.confirmFlag || buttonStateFlags.exitFlag ||
        buttonStateFlags.upFlag || buttonStateFlags.downFlag)
    {
        inputDetected = true;
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

void IRAM_ATTR touchButtonHandler()
{
    uint32_t currentTime = millis();
    if (currentTime - lastPressTimeTouch > DEBOUNCE_TIME)
    {
        lastPressTimeTouch = currentTime;
        buttonEvent event = {TOUCH_BUTTON, BUTTON_EVENT_PRESS};
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

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
                break;
            case BUTTON_EXIT:
                buttonStateFlags.exitFlag = true;
                break;
            case BUTTON_UP:
                buttonStateFlags.upFlag = true;
                break;
            case BUTTON_DOWN:
                buttonStateFlags.downFlag = true;
                break;
            case TOUCH_BUTTON:
                buttonStateFlags.touchFlag = true;
                break;
            default:
                break;
            }
            xSemaphoreGive(buttonMutex);
        }
    }
}

// Initialization Function
void initbuttonManager()
{
    attachInterrupt(BUTTON_CONFIRM_PIN, buttonConfirmPress, FALLING);
    attachInterrupt(BUTTON_EXIT_PIN, buttonExitPress, FALLING);
    attachInterrupt(BUTTON_UP_PIN, buttonUpPress, FALLING);
    attachInterrupt(BUTTON_DOWN_PIN, buttonDownPress, FALLING);
    touchAttachInterrupt(TOUCH_BUTTON_PIN, touchButtonHandler, TOUCH_BUTTON_THRESHOLD);

    buttonQueue = xQueueCreate(10, sizeof(buttonEvent));
    buttonMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(
        buttonManagerTask, "buttonManager", 10000, NULL, 5, &buttonTask, 1);
}

void buttonManager::createTask()
{
    initbuttonManager();
}
