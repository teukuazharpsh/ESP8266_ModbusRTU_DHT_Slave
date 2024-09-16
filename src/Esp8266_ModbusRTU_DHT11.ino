#include <ModbusRTU.h>
#include <SoftwareSerial.h>
#include <esp8266Pin.h>
#include "DHT.h"

#include <Adafruit_ADS1X15.h>

//Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */


#define REGN 10
#define SLAVE_ID 1
#define DHTPIN D8     
#define DHTTYPE DHT11

int AddresHumid = 1;
int AddresTempe = 0;
int AddresHic = 2;

ModbusRTU mb;
SoftwareSerial mySerial(D3, D4); // RX, TX
DHT dht(DHTPIN, DHTTYPE);

long  ts;

void setup() {
  Serial.begin(115200, SERIAL_8N1);
  Serial.println("Begin....");
  Serial.println("Hello!");
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  ads.setGain(GAIN_ONE);
  digitalWrite(D4,LOW);
  mySerial.begin(9600);
  dht.begin();
#if defined(ESP32) || defined(ESP8266)
  mb.begin(&mySerial);
#else
  mb.begin(&mySerial);
  //mb.begin(&Serial, RXTX_PIN);  //or use RX/TX direction control pin (if required)
  mb.setBaudrate(9600);
#endif
  mb.slave(SLAVE_ID);
  mb.addHreg(AddresTempe,0);
  mb.addHreg(AddresHumid,0);
  mb.addHreg(AddresHic,0);
}

void loop() {
  int16_t adc0;
  float volts0;
  int humidValue = dht.readHumidity();
  int tempValue = dht.readTemperature();
  int hic = dht.computeHeatIndex(tempValue, humidValue, false);
  adc0 = ads.readADC_SingleEnded(0);
  volts0 = ads.computeVolts(adc0);
  if (millis() > ts + 2000) {
    Serial.println("-----------------------------------------------------------");
    Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0); Serial.println("V");
    Serial.print(F("Humidity: "));
    Serial.print(humidValue);
    Serial.print(F("%  Temperature: "));
    Serial.print(tempValue);
    Serial.print(F("°C "));
    Serial.print(F("° Heat index: "));
    Serial.print(hic);
    Serial.println(F("°C "));    
    ts = millis();
  }
    //Temp
  mb.Hreg(AddresTempe, tempValue); // Initialize all holding registers to zero
  mb.Hreg(AddresHumid, humidValue); // Initialize all holding registers to zero
  mb.Hreg(AddresHic,hic);
  mb.task();
  yield();
}
