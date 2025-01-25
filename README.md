### Description

Safesistance is a device worn on the arm to help detect falling motions and SOS gestures from the user. These signals are responded to with notifications from a chatbot on a smartphone. The device operates by connecting gyroscope and accelerometer sensors to a microcontroller for data extraction. The extracted data is then processed and trained using a Deep Learning model. The purpose of using Deep Learning is to "recognize" the user's movements, which will produce an output in the form of motion classification.

### Files
- [Jupyter Notebook: Model Training](https://github.com/trinadia/safesistance/blob/main/safesistance_train_model.ipynb)
- [Model in .h file](https://github.com/trinadia/safesistance/blob/main/model/model%20(2).h)
- [Arduino sketch: Deployment in Arduino](https://github.com/trinadia/safesistance/blob/main/imu_datacollect_withspace.ino)
### Components (Hardware & Software)
1. Arduino BLE 33 Sense with built-in IMU sensor
2. Telegram Bot

### First Development: WaveHelp
[<img src="https://github.com/trinadia/safesistance/blob/main/wavehelp.jpg" width="50%">](https://www.youtube.com/watch?v=qZU4iV3DNYg "WaveHelp")
