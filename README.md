# Bascula_Digital_WIF_ESP8266
 *Project Description:*

This project aims to create a system for measuring both weight and volume using a load cell. Load cells are a type of strain gauge that changes its resistance in response to deformation, making them useful for measuring the deformation caused by applied weight.

 *Hardware Used:*

We implement these load cells using an HX711 module and the ESP8266 microcontroller to read the Wheatstone bridge.

 *Project Components:*
This project includes two main code components:
1. Gauge Calibration: This code is designed for calibrating the load cell with your specific setup.
2. Weighing Machine: This code enables the use of a weighing machine with additional features, such as an SH1106 OLED display, buttons for selecting measurement types, and the option to use the ESP8266's Wi-Fi capabilities for reading and storing measurements through a local server.

![Image of the assembly with the Esp, HX711 and the oled screen](https://github.com/JuanHoKKeR/Bascula_Digital_WIF_ESP8266/blob/master/ProjectImagen.jpg?raw=true)

![Image of WebServer](https://github.com/JuanHoKKeR/Bascula_Digital_WIF_ESP8266/blob/master/WebServer.JPG?raw=true)
