/*
 * Dual BNO055 quaternion stream @ 100 Hz.
 * Sensor 1 (brazo): I2C Wire D21=SDA, D22=SCL, addr 0x28
 * Sensor 2 (antebrazo): I2C Wire1 D18=SDA, D19=SCL, addr 0x28
 * Salida: w1,x1,y1,z1,w2,x2,y2,z2\n (solo números, una línea por muestra)
 */
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// Brazo: bus I2C principal (D21, D22)
Adafruit_BNO055 bno1 = Adafruit_BNO055(-1, 0x28, &Wire);
// Antebrazo: segundo bus I2C (D18, D19)
Adafruit_BNO055 bno2 = Adafruit_BNO055(-1, 0x28, &Wire1);

static bool bno1_ok = false;
static bool bno2_ok = false;

const uint32_t PERIOD_US = 10000;  // 100 Hz = 10 ms
uint32_t next_sample_us = 0;

void setup() {
  Serial.begin(115200);

  // Inicialización sensor 1 (brazo) en Wire — D21 SDA, D22 SCL
  Wire.begin(21, 22);
  if (!bno1.begin()) {
    Serial.println("ERR_BNO1");
    bno1_ok = false;
  } else {
    bno1_ok = true;
  }

  // Inicialización sensor 2 (antebrazo) en Wire1 — D18 SDA, D19 SCL
  Wire1.begin(18, 19);
  if (!bno2.begin()) {
    Serial.println("ERR_BNO2");
    bno2_ok = false;
  } else {
    bno2_ok = true;
  }

  if (!bno1_ok || !bno2_ok) {
    for (;;) { delay(1000); }
  }

  next_sample_us = micros();
}

void loop() {
  uint32_t now_us = micros();
  if ((int32_t)(now_us - next_sample_us) < 0) {
    return;
  }
  next_sample_us += PERIOD_US;
  // Evitar acumulación de retraso si el loop se retrasa
  if ((int32_t)(now_us - next_sample_us) > 0) {
    next_sample_us = now_us;
  }

  imu::Quaternion q1 = bno1.getQuat();
  imu::Quaternion q2 = bno2.getQuat();

  // Formato: w1,x1,y1,z1,w2,x2,y2,z2
  Serial.print(q1.w(), 2);
  Serial.print(',');
  Serial.print(q1.x(), 2);
  Serial.print(',');
  Serial.print(q1.y(), 2);
  Serial.print(',');
  Serial.print(q1.z(), 2);
  Serial.print(',');
  Serial.print(q2.w(), 2);
  Serial.print(',');
  Serial.print(q2.x(), 2);
  Serial.print(',');
  Serial.print(q2.y(), 2);
  Serial.print(',');
  Serial.print(q2.z(), 2);
  Serial.println();
}
