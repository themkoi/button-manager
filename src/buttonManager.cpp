#include "oledManager.h"

buttonManager buttons;

TaskHandle_t buttonTask = NULL;

// Queue and mutex handles
QueueHandle_t buttonQueue;
SemaphoreHandle_t buttonMutex;

void buttonManager::oledDisplay()
{
    Action action = OLED_DISPLAY;
    xQueueSend(buttonQueue, &action, portMAX_DELAY);
}

void buttonManager::oledFadeOut()
{
    Action action = OLED_FADE_OUT;
    xQueueSend(buttonQueue, &action, portMAX_DELAY);
}

void buttonManager::oledFadeIn()
{
    Action action = OLED_FADE_IN;
    xQueueSend(buttonQueue, &action, portMAX_DELAY);
}

void buttonManager::oledDisable()
{
    Action action = OLED_DISABLE;
    xQueueSend(buttonQueue, &action, portMAX_DELAY);
}

void buttonManager::oledEnable()
{
    Action action = OLED_ENABLE;
    xQueueSend(buttonQueue, &action, portMAX_DELAY);
}

void buttonManager::sendCustomCommand(uint8_t command)
{
    ActionData actionData;
    actionData.action = OLED_CUSTOM_COMMAND;
    actionData.param1 = command;
    xQueueSend(buttonQueue, &actionData, portMAX_DELAY);
}

void buttonManager::stopScrolling()
{
    ActionData actionData;
    actionData.action = OLED_STOP_SCROLL;
    xQueueSend(buttonQueue, &actionData, portMAX_DELAY);
}

void buttonManager::startScrollingLeft(uint8_t startPage, uint8_t endPage, uint8_t speed)
{
    ActionData actionData;
    actionData.action = OLED_SCROLL_LEFT;
    actionData.param1 = startPage;
    actionData.param2 = endPage;
    actionData.param3 = speed;
    xQueueSend(buttonQueue, &actionData, portMAX_DELAY);
}

void buttonManager::startScrollingRight(uint8_t startPage, uint8_t endPage, uint8_t speed)
{
    ActionData actionData;
    actionData.action = OLED_SCROLL_RIGHT;
    actionData.param1 = startPage;
    actionData.param2 = endPage;
    actionData.param3 = speed;
    xQueueSend(buttonQueue, &actionData, portMAX_DELAY);
}

// Communication to the display
void disableImplementation()
{
    if (manager.ScreenEnabled == true)
    {
        manager.ScreenEnabled = false;
        delay(100);
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        delay(100);
    }
}

void oledEnableImplementation()
{
    if (manager.ScreenEnabled == false)
    {
        manager.ScreenEnabled = true;
        delay(100);
        display.ssd1306_command(SSD1306_DISPLAYON);
        delay(100);
    }
}

void oledDisplayImplementation()
{
    vTaskDelay(pdMS_TO_TICKS(1));
    display.display();
    vTaskDelay(pdMS_TO_TICKS(1));
}

void oledFadeOutImplementation()
{
    if (fading || displaying)
        return; // Check if already fading or displaying

    fading = true; // Set fading flag to true

    delay(100);
    // Fade out
    for (int dim = 150; dim >= 1; dim -= 10)
    {
        display.ssd1306_command(0x81); // Contrast control command
        display.ssd1306_command(dim);  // Set contrast value
        delay(10);
    }

    delay(50);

    for (int dim2 = 34; dim2 >= 1; dim2 -= 17)
    {
        display.ssd1306_command(0xD9); // Pre-charge period command
        display.ssd1306_command(dim2); // Set pre-charge period
        delay(20);
    }
    delay(100);

    fading = false; // Reset fading flag
}

void oledFadeInImplementation()
{
    fading = true; // Set fading flag to true

    delay(100);
    // Fade in
    for (uint8_t dim = 1; dim <= 160; dim += 10)
    {
        display.ssd1306_command(0x81);
        display.ssd1306_command(dim);
        delay(10);
    }

    delay(50);

    for (uint8_t dim2 = 1; dim2 <= 34; dim2 += 17)
    {
        display.ssd1306_command(0xD9);
        display.ssd1306_command(dim2);
        delay(30);
    }
    delay(100);

    fading = false; // Reset fading flag
}

void sendCommand(uint8_t command)
{
    delay(10);
    display.ssd1306_command(command);
    delay(10);
}

// Function to start horizontal scroll left with customizable speed
void startScrollLeftImplementation(uint8_t startPage, uint8_t endPage, uint8_t speed)
{
    uint8_t interval;
    switch (speed)
    {
    case 5:
        interval = 0b000;
        break;
    case 64:
        interval = 0b001;
        break;
    case 128:
        interval = 0b010;
        break;
    case 256:
        interval = 0b011;
        break;
    case 3:
        interval = 0b100;
        break;
    case 4:
        interval = 0b101;
        break;
    case 25:
        interval = 0b110;
        break;
    case 2:
        interval = 0b111;
        break;
    default:
        interval = 0b000;
        break;
    }
    delay(10);

    // Command for horizontal scroll left
    sendCommand(0x27);      // Command for left horizontal scroll
    sendCommand(0x00);      // Dummy byte
    sendCommand(startPage); // Start page address
    sendCommand(interval);  // Time interval
    sendCommand(endPage);   // End page address
    sendCommand(0x00);      // Dummy bytes
    sendCommand(0xFF);
    sendCommand(0x2F); // Activate scroll
    delay(10);
}

// Function to start horizontal scroll right with customizable speed
void startScrollRightImplementation(uint8_t startPage, uint8_t endPage, uint8_t speed)
{
    uint8_t interval;
    switch (speed)
    {
    case 5:
        interval = 0b000;
        break;
    case 64:
        interval = 0b001;
        break;
    case 128:
        interval = 0b010;
        break;
    case 256:
        interval = 0b011;
        break;
    case 3:
        interval = 0b100;
        break;
    case 4:
        interval = 0b101;
        break;
    case 25:
        interval = 0b110;
        break;
    case 2:
        interval = 0b111;
        break;
    default:
        interval = 0b000;
        break;
    }
    delay(10);

    // Command for horizontal scroll right
    sendCommand(0x26);      // Command for right horizontal scroll
    sendCommand(0x00);      // Dummy byte
    sendCommand(startPage); // Start page address
    sendCommand(interval);  // Time interval
    sendCommand(endPage);   // End page address
    sendCommand(0x00);      // Dummy bytes
    sendCommand(0xFF);
    sendCommand(0x2F); // Activate scroll
    delay(10);
}

void stopScrollImplementation()
{
    delay(1);
    display.stopscroll();
    delay(1);
}

void customCommandImplementation(uint8_t command)
{
    delay(100);
    display.ssd1306_command(command);
    delay(100);
}

#define DEBOUNCE_TIME_MS 1

unsigned long lastActionTimestamp = 0;
Action lastAction = OLED_NO_ACTION;

void buttonManagerTask(void *pvParameters)
{
    ActionData actionData;
    while (true)
    {
        if (xQueueReceive(buttonQueue, &actionData, portMAX_DELAY))
        {
            unsigned long currentMillis = millis();

            if (actionData.action != lastAction)
            {
                xSemaphoreTake(buttonMutex, portMAX_DELAY);

                switch (actionData.action)
                {
                case BUTTON_CONFIRM:
                    Serial.println("Processing OLED Fade In");
                    oledFadeInImplementation();
                    break;
                case BUTTON_EXIT:
                    Serial.println("Processing OLED Fade Out");
                    oledFadeOutImplementation();
                    break;
                case BUTTON_DOWN:
                    vTaskDelay(pdMS_TO_TICKS(5));
                    oledDisplayImplementation();
                    break;
                case BUTTON_UP:
                    Serial.println("Processing OLED Enable");
                    oledEnableImplementation();
                    break;
                default:
                    Serial.println("Unknown action");
                    break;
                }

                xSemaphoreGive(buttonMutex);
                lastAction = actionData.action;
                lastActionTimestamp = currentMillis;
            }
            vTaskSuspend(NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Adjust delay as needed
    }
}

void exitIntBut()
{
    interruptedButton = Back;
    resumeButtonTask();
}

void confirmIntBut()
{
    interruptedButton = Menu;
    resumeButtonTask();
}

void upIntBut()
{
    interruptedButton = Up;
    resumeButtonTask();
}

void downIntBut()
{
    interruptedButton = Down;
    resumeButtonTask();
}

void initbuttonManager()
{
    attachInterrupt(BUTTON_CONFIRM, confirmIntBut, LOW);
    attachInterrupt(BUTTON_EXIT, exitIntBut, LOW);
    attachInterrupt(BUTTON_DOWN, downIntBut, LOW);
    attachInterrupt(BUTTON_UP, upIntBut, LOW);

    buttonQueue = xQueueCreate(1, sizeof(ActionData));
    buttonMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(
        buttonManagerTask, /* Task function. */
        "buttonManager",   /* String with name of task. */
        10000,             /* Stack size in words. */
        NULL,              /* Parameter passed as input of the task */
        5,                 /* Priority of the task. */
        buttonTask,        /* Task handle. */
        1                  /* Core where the task should run. */
    );
}

void buttonManager::createTask()
{
    initbuttonManager();
}
