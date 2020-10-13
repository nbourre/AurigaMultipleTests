/**
 * \par Copyright (C), 2012-2016, MakeBlock
 * @file    ColorLoopTest.ino
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2015/09/02
 * @brief   Description: this file is sample code for RGB LED, It used for random color change
 *
 * Function List:
 * 1. bool MeRGBLed::setColorAt(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
 * 2. void MeRGBLed::show()
 * 3. void MeRGBLed::setpin(uint8_t port)
 * \par History:
 * <pre>
 * <Author>     <Time>        <Version>      <Descr>
 * Mark Yan     2015/09/02    1.0.0          rebuild the old lib
 * Gregory Toto 2017/03/01    1.0.1          added Auriga support
</pre>
 */

#include <MeAuriga.h>
#include <Wire.h>

#define ALLLEDS        0

#ifdef MeAuriga_H

// Auriga on-board light ring has 12 LEDs
#define LEDNUM  12

// on-board LED ring, at PORT0 (onboard)
MeRGBLed led( 0, LEDNUM );

#else

#define LEDNUM  14

// external device
MeRGBLed led( PORT_3 );

#endif

MeSoundSensor mySound(PORT_6);
MeGyro gyro(0, 0x69);

const int tempPin = A0;
int tempSensorValue = 0;
float tempOutput = 0;

float j, f, k;
int serial_refresh_rate = 200;
int serial_acc = 0;
int dt = 0;
int ct = millis();
int pt = 0;

void setup()
{
#ifdef MeAuriga_H
    // LED Ring controller is on Auriga D44/PWM
    led.setpin( 44 );
#endif
  Serial.begin(115200);
  
  gyro.begin();

  ct = millis();
  pt = 0;
}

void loop()
{
  // Board timing
  ct = millis();
  dt = ct - pt;
  pt = ct;
  
  led_loop_task();
  measure_sound();
  
  //gyro_task();
  temp_task();  
  show_text();
}

int sound_cnt = 0;
float sound_avg = 0;
short sound_rst = 1;

void measure_sound() {
  if (sound_rst != 0) {
    sound_cnt = 0;
    sound_avg = 0.0;

    sound_rst = 0;
  }
  
  sound_cnt++;
  
  sound_avg += mySound.strength();
}

int gyro_acc = 0;
int gyro_int = 10;

void gyro_task() {
  gyro_acc += dt;

  if (gyro_acc < gyro_int) return;

  gyro_acc = 0;
  
  gyro.update();
  Serial.print("X:");
  Serial.print(gyro.getAngleX() );
  Serial.print(" Y:");
  Serial.print(gyro.getAngleY() );
  Serial.print(" Z:");
  Serial.println(gyro.getAngleZ() );

}


void show_text() {
  serial_acc += dt;
  
  if (serial_acc < serial_refresh_rate) return;

  serial_acc = serial_acc % serial_refresh_rate;

  // Resetting sound values
  sound_avg /= sound_cnt;
  sound_rst = 1;

  
  Serial.print("Sound = ");
  Serial.print(sound_avg);

  Serial.print("\tTemperature = ");
  Serial.print(tempOutput);

  Serial.println();
}

void led_loop_task()
{
  for (uint8_t t = 0; t < LEDNUM; t++ )
  {
    uint8_t red	= 8 * (1 + sin(t / 2.0 + j / 4.0) );
    uint8_t green = 8 * (1 + sin(t / 1.0 + f / 9.0 + 2.1) );
    uint8_t blue = 8 * (1 + sin(t / 3.0 + k / 14.0 + 4.2) );
    led.setColorAt( t, red, green, blue );
  }
  led.show();

  j += random(1, 6) / 6.0;
  f += random(1, 6) / 6.0;
  k += random(1, 6) / 6.0;
}

/**
Temperature values
Src : https://github.com/search?q=TERMISTORNOMINAL+auriga&type=code
*/

const int16_t TEMPERATURENOMINAL     = 25;    //Nominl temperature depicted on the datasheet
const int16_t SERIESRESISTOR         = 10000; // Value of the series resistor
const int16_t BCOEFFICIENT           = 3380;  // Beta value for our thermistor(3350-3399)
const int16_t TERMISTORNOMINAL       = 10000; // Nominal temperature value for the thermistor

float calculate_temp(int16_t In_temp)
{
  float media;
  float temperatura;
  media = (float)In_temp;
  // Convert the thermal stress value to resistance
  media = 1023.0 / media - 1;
  media = SERIESRESISTOR / media;
  //Calculate temperature using the Beta Factor equation

  temperatura = media / TERMISTORNOMINAL;              // (R/Ro)
  temperatura = log(temperatura); // ln(R/Ro)
  temperatura /= BCOEFFICIENT;                         // 1/B * ln(R/Ro)
  temperatura += 1.0 / (TEMPERATURENOMINAL + 273.15);  // + (1/To)
  temperatura = 1.0 / temperatura;                     // Invert the value
  temperatura -= 273.15;                               // Convert it to Celsius
  return temperatura;
}



float temp_task() {
    tempSensorValue = analogRead(tempPin);
    tempOutput = calculate_temp(tempSensorValue);
}
