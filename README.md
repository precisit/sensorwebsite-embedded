# bankvader_emb

**Embedded software for Precisit project "Bänkväder"**

**Project built using PlatformIO, with Continuous integration via GitLab CI.**


Compile project using PlatformIO:
```
pio run
```

Upload code to target Arduino MKR Wifi 1010:
```
pio run --target upload
```

**In order to connect Arduino to AWS IoT, you need to register the device with AWS IoT Core, use instructions below**
[Arduino AWS IoT Core](https://create.arduino.cc/projecthub/Arduino_Genuino/securely-connecting-an-arduino-mkr-wifi-1010-to-aws-iot-core-a9f365)
