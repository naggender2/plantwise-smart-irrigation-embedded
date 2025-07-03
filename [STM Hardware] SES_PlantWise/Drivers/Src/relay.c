// relay.c
#include "relay.h"
#include "mqtt.h"

/* Define the GPIO port and pin for the relay module */
#define RELAY_GPIO_PORT GPIOB
#define RELAY_GPIO_PIN  GPIO_PIN_11  // PB11 for relay control

static bool relay_state = false;

void relay_init(void) {
    /* Enable GPIO clock for relay control port */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = RELAY_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RELAY_GPIO_PORT, &GPIO_InitStruct);

    /* Start with pump OFF (active-HIGH: RESET = OFF) */
    HAL_GPIO_WritePin(RELAY_GPIO_PORT, RELAY_GPIO_PIN, GPIO_PIN_SET);
		relay_state = false;
}

void relay_set(bool on)
{
    if (on && !relay_state) {
        /* 1) quiesce Wi-Fi radio */
        esp_wifi_lowpower(true);
        HAL_Delay(150);                        /* beacons clear */

        /* 2) energise relay */
        HAL_GPIO_WritePin(RELAY_GPIO_PORT,RELAY_GPIO_PIN,GPIO_PIN_RESET);
        relay_state = true;

        /* 3) small settle delay then restore RF */
        HAL_Delay(400);                       /* coil current peak */
        esp_wifi_lowpower(false);

    } else if (!on && relay_state) {
        HAL_GPIO_WritePin(RELAY_GPIO_PORT,RELAY_GPIO_PIN,GPIO_PIN_SET);
        relay_state = false;
    }
}
