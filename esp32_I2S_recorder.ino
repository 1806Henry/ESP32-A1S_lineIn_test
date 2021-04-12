#include "Arduino.h"
#include <FS.h>
#include "Wav.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "AC101.h"
#include "I2S.h"

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

#define IIS_SCLK                    27
#define IIS_LCLK                    26
#define IIS_DSIN                    25

static AC101 ac;

const int record_time = 10;  // second
const char filename[] = "/ttttt.wav";

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
  
  Serial.printf("Connect to AC101 codec... ");
  while (not ac.begin(IIC_DATA, IIC_CLK))
  {
    Serial.printf("Failed!\n");
    delay(1000);
  }
  
  ac.SetLineSource();
   ac.SetI2sWordSize(AC101::WORD_SIZE_16_BITS);
ac.SetI2sSampleRate(AC101::SAMPLE_RATE_44100);
ac.SetI2sClock(AC101::BCLK_DIV_16, false, AC101::LRCK_DIV_32, false);
ac.SetI2sMode(AC101::MODE_SLAVE);
ac.SetI2sWordSize(AC101::WORD_SIZE_16_BITS);
ac.SetI2sFormat(AC101::DATA_FORMAT_I2S);

/* nur bei activen output */
ac.SetVolumeSpeaker(3);
ac.SetVolumeHeadphone(99);

/* funktioniert auch mit aktivem output */
ac.SetLineSource();

// Enable amplifier
pinMode(GPIO_PA_EN, OUTPUT);
digitalWrite(GPIO_PA_EN, HIGH);

  
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
