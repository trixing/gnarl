#include <stdio.h>

#include <esp_wifi.h>
#include <esp_task_wdt.h>

#include "display.h"
#include "gnarl.h"
#include "voltage.h"
#include "rfm95.h"

#define PUMP_FREQUENCY 868600000

void app_main(void) {
	// #define LED		GPIO_NUM_2  // ?? according to blink app
	// #define LED		GPIO_NUM_25 // ?? according to schematic
	// gpio_set_direction(LED, GPIO_MODE_OUTPUT);
	// gpio_set_level(LED, on_off);

	ESP_LOGD(TAG, "enable watchdog with a timeout of %d seconds", WDT_TIMEOUT_SECONDS);
	esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);
	ESP_LOGI(TAG, "%s", SUBG_RFSPY_VERSION);
	rfm95_init();
	uint8_t v = read_version();
	ESP_LOGD(TAG, "radio version %d.%d", version_major(v), version_minor(v));
	set_frequency(PUMP_FREQUENCY);
	ESP_LOGD(TAG, "frequency set to %d Hz", read_frequency());
	display_init();
	voltage_init();
	gnarl_init();
}
