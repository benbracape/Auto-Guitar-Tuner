#include <Arduino.h>
#include <driver/i2s.h>
#include <dsps_fft2r.h>
#include "freertos/FreeRTOS.h"

#define SAMPLE_RATE 4000
const int BLOCK_SIZE = 1024;
int32_t samples[BLOCK_SIZE];
const i2s_port_t I2S_PORT = I2S_NUM_0;
#define READ_DELAY 3000 // millisec

void process_samples(void *pvParameters);

// put function declarations here:

void setup() {
  Serial.begin(115200);  // Use standard baud rate
  delay(2000);  // Longer delay for serial initialization
  Serial.println();
  Serial.println("=== ESP32 Starting ===");
  Serial.println("Serial communication working!");
  
  // Initialize DSP library for FFT
  esp_err_t ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
  if (ret != ESP_OK) {
    Serial.printf("DSP init failed: %d\n", ret);
  }
  
  i2s_config_t audio_in_i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = BLOCK_SIZE,
      .use_apll = false,
      .fixed_mclk = 0
  };

  esp_err_t err = i2s_driver_install(I2S_PORT, &audio_in_i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }

  // configure which pin on the ESP32 is connected to which pin on the mic:
  i2s_pin_config_t audio_in_pin_config = {
      .bck_io_num = I2S_PIN_NO_CHANGE,
      .ws_io_num = 25,  // CLK pin - A1 on the board
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = 32   // DAT pin - D32 on the board
  };

  err = i2s_set_pin(I2S_PORT, &audio_in_pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }
  
  Serial.println("I2S PDM mic initialized successfully");
  xTaskCreatePinnedToCore(process_samples, "PDM_reader", 4096, NULL, 1, NULL, 1);
}

void process_samples(void *pvParameters) {
    while(1){
      size_t bytes_read;
      esp_err_t result = i2s_read(I2S_PORT, 
      (void *)samples, 
      BLOCK_SIZE * sizeof(int32_t),
      &bytes_read,
      portMAX_DELAY);
      
      if (result == ESP_OK && bytes_read > 0) {
        // Just print a few sample values to see if we're getting data
        static int counter = 0;
        if (counter++ % 50 == 0) {  // Print every 50th batch
          // Show both raw 32-bit values and shifted 16-bit values
          Serial.printf("Raw: 0x%08X, 0x%08X | Shifted: %d, %d | Bytes: %d\n", 
                       samples[0], samples[1], samples[0] >> 16, samples[1] >> 16, bytes_read);
        }
      } else {
        Serial.printf("I2S error: %d\n", result);
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      
      vTaskDelay(1); // Prevent watchdog issues
    }
}

void loop() {
  // put your main code here, to run repeatedly:

}

// put function definitions here: