### Deskripsi

Safesistance adalah sebuah alat berbentuk gelang yang dipasangkan pada lengan untuk membantuk mendeteksi gerakan jatuh pada pengguna dan gestur meminta pertolongan (SOS), di mana sinyal ini akan direspon dengan notifikasi dari chatbot di smartphone. Cara kerja dari alat ini adalah sensor gyroscope dan accelerometer dihubungkan dengan mikrokontroler untuk proses ekstraksi data, kemudian data tersebut diolah dan melalui proses _training_ dengan model Deep Learning. Penggunaan Deep Learning ini adalah untuk "mengenali" gerakan si pengguna yang nantinya akan memberi _output_ berupa klasifikasi gerakan.  

### Files
- [Jupyter Notebook: Model Training](https://github.com/trinadia/safesistance/blob/main/safesistance_train_model.ipynb)
- [Model in .h file](https://github.com/trinadia/safesistance/blob/main/model/model%20(2).h)
- [Arduino sketch: Deployment in Arduino](https://github.com/trinadia/safesistance/blob/main/imu_datacollect_withspace.ino)
### Komponen Perangkat Keras
1. Waistband
2. Bot Telegram

### First Development: WaveHelp
[<img src="https://github.com/trinadia/safesistance/blob/main/wavehelp.jpg" width="50%">](https://www.youtube.com/watch?v=qZU4iV3DNYg "WaveHelp")
