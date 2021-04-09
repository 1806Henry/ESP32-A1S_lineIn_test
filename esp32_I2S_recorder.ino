#include "Arduino.h"
#include <FS.h>
#include "Wav.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_CS         13
#define SPI_MOSI      15
#define SPI_MISO       2
#define SPI_SCK       14
#define I2S_DSIN      25
#define I2S_BCLK      27
#define I2S_LRC       26
#define I2S_MCLK       0
#define I2S_DOUT      35

// I2C GPIOs
#define IIC_CLK       32
#define IIC_DATA      33


const int record_time = 10;  // second
const char filename[] = "/test.wav";

const int headerSize = 44;
const int waveDataSize = record_time * 88000;
const int numCommunicationData = 8000;
const int numPartWavData = numCommunicationData/4;
byte header[headerSize];
char communicationData[numCommunicationData];
char partWavData[numPartWavData];
File file;

void setup() {
   Serial.begin(115200);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  pinMode(SD_CS, OUTPUT);
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  delay(4000);
   Serial.println("start");
  CreateWavHeader(header, waveDataSize);
  SD.remove(filename);
  file = SD.open(filename, FILE_WRITE);
  if (!file) return;
  file.write(header, headerSize);
    pinMode(35, INPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  I2S_Init();
  
  for (int j = 0; j < waveDataSize/numPartWavData; ++j) {
    I2S_Read(communicationData, numCommunicationData);
    for (int i = 0; i < numCommunicationData/8; ++i) {
      partWavData[2*i] = communicationData[8*i + 2];
      partWavData[2*i + 1] = communicationData[8*i + 3];
    }
    file.write((const byte*)partWavData, numPartWavData);
  }
  file.close();
  Serial.println("finish");
}

void loop() {
}

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
