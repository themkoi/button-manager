#include "buttonManager.h"

buttonManager buttons;

TaskHandle_t buttonTask = NULL;
QueueHandle_t buttonQueue;
SemaphoreHandle_t buttonMutex;

#define DEBOUNCE_TIME_MS 50
volatile unsigned long lastDebounceTime[6] = {0}; // For debounce tracking, including the touch button

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

bool buttonManager::checkConfirm()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    if (buttonStateFlags.confirmFlag)
    {
        inputDetected = true;
        buttonStateFlags.confirmFlag = false; // Reset the confirm flag
    }
    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkExit()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    if (buttonStateFlags.exitFlag)
    {
        inputDetected = true;
        buttonStateFlags.exitFlag = false; // Reset the exit flag
    }
    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkUp()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    if (buttonStateFlags.upFlag)
    {
        inputDetected = true;
        buttonStateFlags.upFlag = false; // Reset the up flag
    }
    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkDown()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    if (buttonStateFlags.downFlag)
    {
        inputDetected = true;
        buttonStateFlags.downFlag = false; // Reset the down flag
    }
    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

bool buttonManager::checkTouch()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    if (buttonStateFlags.touchFlag)
    {
        inputDetected = true;
        buttonStateFlags.touchFlag = false; // Reset the touch flag
    }
    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

// Checks for any input (including touch)
bool buttonManager::checkInput()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    if (buttonStateFlags.confirmFlag || buttonStateFlags.exitFlag ||
        buttonStateFlags.upFlag || buttonStateFlags.downFlag ||
        buttonStateFlags.touchFlag)
    {
        inputDetected = true;
    }
    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

// Checks for button input only (excluding touch)
bool buttonManager::checkButtonInput()
{
    bool inputDetected = false;
    xSemaphoreTake(buttonMutex, portMAX_DELAY);
    if (buttonStateFlags.confirmFlag || buttonStateFlags.exitFlag ||
        buttonStateFlags.upFlag || buttonStateFlags.downFlag)
    {
        inputDetected = true;
    }
    xSemaphoreGive(buttonMutex);
    return inputDetected;
}

// Button press functions for physical buttons
void IRAM_ATTR buttonConfirmPress()
{
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime[BUTTON_CONFIRM] > DEBOUNCE_TIME_MS)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreTakeFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        buttonStateFlags.confirmFlag = true;
        xSemaphoreGiveFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        lastDebounceTime[BUTTON_CONFIRM] = currentTime;
    }
}

void IRAM_ATTR buttonExitPress()
{
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime[BUTTON_EXIT] > DEBOUNCE_TIME_MS)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreTakeFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        buttonStateFlags.exitFlag = true;
        xSemaphoreGiveFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        lastDebounceTime[BUTTON_EXIT] = currentTime;
    }
}

void IRAM_ATTR buttonUpPress()
{
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime[BUTTON_UP] > DEBOUNCE_TIME_MS)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreTakeFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        buttonStateFlags.upFlag = true;
        xSemaphoreGiveFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        lastDebounceTime[BUTTON_UP] = currentTime;
    }
}

void IRAM_ATTR buttonDownPress()
{
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime[BUTTON_DOWN] > DEBOUNCE_TIME_MS)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreTakeFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        buttonStateFlags.downFlag = true;
        xSemaphoreGiveFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        lastDebounceTime[BUTTON_DOWN] = currentTime;
    }
}

void IRAM_ATTR touchButtonHandler()
{
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime[TOUCH_BUTTON] > DEBOUNCE_TIME_MS)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreTakeFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        buttonStateFlags.touchFlag = true;
        xSemaphoreGiveFromISR(buttonMutex, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        lastDebounceTime[TOUCH_BUTTON] = currentTime;
    }
}

// Task to Process Button Events
void buttonManagerTask(void *pvParameters)
{
    buttonEvent event;
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for notification
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

void initbuttonManager()
{
    attachInterrupt(BUTTON_CONFIRM_PIN, buttonConfirmPress, LOW);
    attachInterrupt(BUTTON_EXIT_PIN, buttonExitPress, LOW);
    attachInterrupt(BUTTON_UP_PIN, buttonUpPress, LOW);
    attachInterrupt(BUTTON_DOWN_PIN, buttonDownPress, LOW);

    touchAttachInterrupt(TOUCH_BUTTON_PIN, touchButtonHandler, TOUCH_BUTTON_THRESHOLD);

    buttonQueue = xQueueCreate(10, sizeof(buttonEvent));
    buttonMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(
        buttonManagerTask, /* Task function. */
        "buttonManager",   /* String with name of task. */
        10000,             /* Stack size in words. */
        NULL,              /* Parameter passed as input of the task */
        5,                 /* Priority of the task. */
        &buttonTask,       /* Task handle. */
        1                  /* Core where the task should run. */
    );
}

void buttonManager::createTask()
{
    initbuttonManager();
}
