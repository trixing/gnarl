#include "gnarl.h"

#include <stdlib.h>
#include <unistd.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <esp_log.h>

#include "voltage.h"

#define VBAT		ADC1_CHANNEL_6
#define VUSB		ADC1_CHANNEL_7
#define DEFAULT_VREF	1100
#define NUM_SAMPLES	10

// 0 dB attenuation = 0 to 1.1V voltage range
// 11 dB attenuation = 0 to 3.9V voltage range
#define ADC_ATTEN ADC_ATTEN_DB_11

// With 10M + 2M voltage divider, ADC voltage is 1/6 of actual value, enter 6
// With 100k + 100k voltage divider, ADC voltage is 1/2 of actual value, enter 2
#define ADC_SCALE 2

static esp_adc_cal_characteristics_t *adc_chars;


static int voltage_read(int channel) {
	int raw = 0;
	for (int i = 0; i < NUM_SAMPLES; i++) {
		int r = adc1_get_raw(channel);
		raw += r;
	//	ESP_LOGD(TAG, "Channel: %d raw: %4d\n", channel, r);
	}
	raw /= NUM_SAMPLES;
	int voltage = esp_adc_cal_raw_to_voltage(raw, adc_chars) * ADC_SCALE;
	ESP_LOGD(TAG, "Channel: %d Voltage: %d mV    raw:  %4d    0x%03X\n", channel, voltage, raw, raw);
	return voltage;
}

char *voltage_string(int mV, char *buf) {
	char *p = buf;
	if (mV < 0) {
		*p++ = '-';
		mV = -mV;
	}
	sprintf(p, "%d.%03d V", mV / 1000, mV % 1000);
	return buf;
}

static volatile int vusb, vbat;

// int voltage_usb(void) { return vusb; }
// int voltage_bat(void) { return vbat; }
int voltage_usb(void) { return voltage_read(VUSB); }
int voltage_bat(void) { return voltage_read(VBAT); }

static void voltage_loop(void *unused) {
	for (;;) {
		vusb = voltage_read(VUSB);
		vbat = voltage_read(VBAT);
		usleep(1e6);
	}
}

void voltage_init(void) {
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(VBAT, ADC_ATTEN);
	adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
	// xTaskCreate(voltage_loop, "voltage", 512, 0, tskIDLE_PRIORITY + 2, NULL);
}
