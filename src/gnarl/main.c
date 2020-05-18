#include "gnarl.h"

#include <stdio.h>

#include <esp_wifi.h>
#include <esp_task_wdt.h>

#include "display.h"
#include "rfm95.h"

#define PUMP_FREQUENCY 868600000

void app_main() {
	ESP_LOGD(TAG, "enable watchdag with a timeout of %d seconds", WDT_TIMEOUT_SECONDS);
	esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);
	ESP_LOGI(TAG, "%s", SUBG_RFSPY_VERSION);
	rfm95_init();
	ESP_LOGD(TAG, "post rfm95 init");
	uint8_t v = read_version();
	ESP_LOGD(TAG, "radio version %d.%d", version_major(v), version_minor(v));
	set_frequency(PUMP_FREQUENCY);
	ESP_LOGD(TAG, "frequency set to %d Hz", read_frequency());
	display_init();
	gnarl_init();
}
