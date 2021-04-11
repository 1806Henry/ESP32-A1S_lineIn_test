#include "I2S.h"

void I2S_Init()
{
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = 44100,
      .bits_per_sample = i2s_bits_per_sample_t(16),
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 64,
	  .use_apll = false
    };
	
    i2s_pin_config_t pin_config;
    pin_config.bck_io_num = 27;
    pin_config.ws_io_num = 26;
	pin_config.data_out_num = -1;
	pin_config.data_in_num = 35;

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, 44100, i2s_bits_per_sample_t(16), I2S_CHANNEL_MONO);
 // i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_0);  

}

int I2S_Read(char *data, int numData)
{
  return i2s_read_bytes(I2S_NUM_0, (char *)data, numData, portMAX_DELAY);
}

void I2S_Write(char *data, int numData)
{
  i2s_write_bytes(I2S_NUM_0, (const char *)data, numData, portMAX_DELAY);
}
