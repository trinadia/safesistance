#include <TensorFlowLite.h>

#include <Arduino_BMI270_BMM150.h> // IMU sensor

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "model.h" // model

const float accelerationThreshold = 2.0;
const int numSamples = 119;

int samplesRead = numSamples;

namespace {
tflite::ErrorReporter* tflErrorReporter = nullptr; //ErrorReposter : public class
const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr; //MicroInterpreter : 
TfLiteTensor* input_tensor = nullptr;
TfLiteTensor* output_tensor = nullptr;

constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));
}

const char* GESTURES[] = {
  "none",
  "fall"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");

  Serial.println();

  // --- MAPPING THE MODEL ---
  tflModel = tflite::GetModel(model);
  
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
  // - alt :  if (model ->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }
  // ---OPS RESOLVER ----
  static tflite::AllOpsResolver tflOpsResolver;

  // --- ERROR REPORTER 
  static tflite::MicroErrorReporter micro_error_reporter;
  tflErrorReporter = &micro_error_reporter;

  // --- CREATING INTERPRETER ---
  static tflite::MicroInterpreter interpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize);
  tflInterpreter = &interpreter;

  tflInterpreter->AllocateTensors();

  // --- INSPECTING INPUT TENSOR ---
  input_tensor = tflInterpreter->input(0);
  output_tensor = tflInterpreter->output(0);
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;

  // wait for significant motion
  while (samplesRead == numSamples) {
    if (IMU.accelerationAvailable()) {
      // read the acceleration data
      IMU.readAcceleration(aX, aY, aZ);

      // sum up the absolutes
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      // check if it's above the threshold
      if (aSum >= accelerationThreshold) {
        // reset the sample read count
        samplesRead = 0;
        break;
      }
    }
  }

  while (samplesRead < numSamples) {
    // check if new acceleration AND gyroscope data is available
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      // read the acceleration and gyroscope data
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // --- RUNNING INFERENCE ON INPUT --- 
      // normalization
      input_tensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      input_tensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
      input_tensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
      input_tensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      input_tensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      input_tensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

      samplesRead++;

      if (samplesRead == numSamples) {
        // check that it succeeds
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Invoke failed!");
          while (1);
          return;
        }

        // --- OUTPUT ACTIONS ---
        for (int i = 0; i < NUM_GESTURES; i++) {
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);
        }
        Serial.println();
      }
    }
  }
}
