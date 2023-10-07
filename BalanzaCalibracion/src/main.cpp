#include <Arduino.h>
#include <HX711.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R2, /* reset=*/ U8X8_PIN_NONE); //Rotacion 180 grados

#define DOUT         12  // Pin DOUT del HX711 D6
#define SCK          13  // Pin CLK del HX711 D5


HX711 scale;
long ajustepromedio = 0L;

void setup() {
  Wire.begin();
  oled.begin();
  oled.setFont(u8g2_font_ncenB08_tr);
  oled.clearBuffer();
  Serial.begin(9600); // Iniciar puerto serial para depuración
  scale.begin(DOUT, SCK); // Iniciar el HX711
}

void loop(){
  if (scale.is_ready()) {
    scale.set_scale();    
    oled.clearBuffer();
    Serial.println("Tare... remove any weights from the scale.");
    oled.setCursor(0, 10); 
    oled.print("Tare... remove any");
    oled.setCursor(0, 20); 
    oled.print("weights from the scale.");
    oled.sendBuffer();

    delay(5000);
    scale.tare();
    oled.clearBuffer();
    oled.setCursor(0, 10); 
    oled.print("Tare done...");
    oled.setCursor(0, 20); 
    oled.print("weights from the scale.");
    oled.sendBuffer();
    Serial.println("Tare done...");
    Serial.print("weights from the scale.");
    delay(5000);
    for(int i=0;i<10;i++){
      long reading = scale.get_units(10);
      ajustepromedio += reading;
      Serial.print("Reading: ");Serial.println(reading);
      oled.clearBuffer(); 
      oled.setCursor(0, 40);oled.print("Reading: ");
      oled.setCursor(60, 40);oled.print(reading);
      oled.sendBuffer();
      delay(250);
    }
    ajustepromedio = ajustepromedio/10;
    Serial.print("Result: ");Serial.println(ajustepromedio);
    oled.clearBuffer();
    oled.setCursor(60, 40); 
    oled.print("Result: ");
    oled.setCursor(60, 60); 
    oled.print(ajustepromedio);
    oled.sendBuffer();
    delay(5000);
  } 
  else {
    Serial.println("HX711 not found.");
    oled.clearBuffer();
    oled.setCursor(0, 10); 
    oled.print("HX711 not found.");
    oled.sendBuffer();
  }
  delay(1000);
}