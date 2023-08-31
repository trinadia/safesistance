// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

const int numSamples = 119;
int samplesRead = 0;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);
  Serial.println("aX,aY,aZ,gX,gY,gZ");
}

void loop() {
  sensors_event_t a, g, temp;

  if (samplesRead >= numSamples) {
        // add an empty line if it's the last sample
        samplesRead = 0;
        Serial.println();
  }
  /* Get new sensor events with the readings */
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print(a.acceleration.x, 3);
  Serial.print(",");
  Serial.print(a.acceleration.y, 3);
  Serial.print(",");
  Serial.print(a.acceleration.z, 3);
  Serial.print(",");
  Serial.print(g.gyro.x, 3);
  Serial.print(",");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print(g.gyro.z);
  Serial.println("");

  samplesRead++;
}
