#include <TensorFlowLite_ESP32.h>


#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
//#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "model (2).h" // Include your TensorFlow Lite model here

Adafruit_MPU6050 mpu;

const int numSamples = 312;
int samplesRead = 0;
int setCount = 0;

namespace {
  tflite::ErrorReporter* tflErrorReporter = nullptr;
  const tflite::Model* tflModel = nullptr;
  tflite::MicroInterpreter* tflInterpreter = nullptr;
  TfLiteTensor* inputTensor = nullptr;
  TfLiteTensor* outputTensor = nullptr;

  constexpr int tensorArenaSize = 8 * 1024;
  byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));
}

const char* GESTURES[] = {
  "none",
  "fall"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  // Try to initialize MPU6050
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

  // --- MAPPING THE MODEL ---
  tflModel = tflite::GetModel(model);

  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  // --- OPS RESOLVER ---
  static tflite::AllOpsResolver tflOpsResolver;

  // --- ERROR REPORTER ---
  static tflite::MicroErrorReporter micro_error_reporter;
  tflErrorReporter = &micro_error_reporter;

  // --- CREATING INTERPRETER ---
  static tflite::MicroInterpreter interpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, tflErrorReporter, nullptr);
  tflInterpreter = &interpreter;

  tflInterpreter->AllocateTensors();

  // --- INSPECTING INPUT TENSOR ---
  inputTensor = tflInterpreter->input(0);
  outputTensor = tflInterpreter->output(0);
}

void loop() {
  sensors_event_t a, g, temp;
  /* Get new sensor events with the readings */
  mpu.getEvent(&a, &g, &temp);

  float normalizedData[6];
  float accelerometerRange = 16.0; // Range is ±16g
  float gyroscopeRange = 250.0;    // Range is ±250 degrees per second

  normalizedData[0] = a.acceleration.x;
  normalizedData[1] = a.acceleration.y;
  normalizedData[2] = a.acceleration.z;
  normalizedData[3] = g.gyro.x;
  normalizedData[4] = g.gyro.y;
  normalizedData[5] = g.gyro.z;

  // Assuming 'inputTensor' is a pointer to your model's input tensor
  for (int i = 0; i < 6; i++) {
    inputTensor->data.f[i] = normalizedData[i];
  }

  samplesRead++;

  if (samplesRead == numSamples) {
    // Check that it succeeds
    TfLiteStatus invokeStatus = tflInterpreter->Invoke();
    if (invokeStatus != kTfLiteOk) {
      Serial.println("Invoke failed!");
      while (1);
      return;
    }

    // Loop through the output tensor values from the model and perform inference results processing
    for (int i = 0; i < NUM_GESTURES; i++) {
      Serial.print(GESTURES[i]);
      Serial.print(": ");
      Serial.println(outputTensor->data.f[i], 6);
    }
    Serial.println();

    // Reset sample count for the next set
    samplesRead = 0;
  }
}
