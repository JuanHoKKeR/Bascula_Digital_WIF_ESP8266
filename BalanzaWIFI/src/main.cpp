#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "HX711.h"
#include "Pushbutton.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R2, /* reset=*/ U8X8_PIN_NONE); //Rotacion 180 grados

// Pins
#define DOUT         12  // Pin DOUT del HX711 D6
#define SCK          13  // Pin CLK del HX711 D7
#define tara_button  14  // Botón de tara     D5
#define mode_button  15  // Botón de cambio de modo D8

Pushbutton button(tara_button);
Pushbutton button2(mode_button);

HX711 scale;
int reading;
int lastReading;
///////// Cambia aquí con tu masa calibrada ////////////
#define CALIBRATION_FACTOR 407.1693// masa calibrada a añadir
///////////////////////////////////////////////////////
int mode = 0;
float oz_conversion = 0.035274;
float pound_conversion = 0.00220462;
String currentUnit = "g";

// WiFi credentials
const char *ssid = "Your SSID";
const char *password = "Your Password";

ESP8266WebServer server(80);

void handleRoot() {
  String html = "<html><head>";
  html += "<style>";
  html += "body { font-size: 24px; text-align: center; }";
  html += "#weightValue { font-size: 48px; }";
  html += "img { width: 300px; height: 300px; }";
  html += "</style>";
  html += "</head><body>";
  html += "<h1>Balanza en Tiempo Real</h1>";
  html += "<img src='https://naylampmechatronics.com/img/cms/Blog/Tutorial%20HX711/Dise%C3%B1o%20Balanza.PNG' alt='Bascula' /><br/>";
  html += "<div id='weightValue'>Cargando...</div>";
  html += "<div>";
  html += "<h2>Unidad Actual: <span id='currentUnit'>" + currentUnit + "</span></h2>";
  html += "<button onclick='changeUnit(\"g\")'>Gramos</button>";
  html += "<button onclick='changeUnit(\"ml\")'>Mililitros</button>";
  html += "<button onclick='changeUnit(\"oz\")'>Onzas</button>";
  html += "<button onclick='changeUnit(\"lb\")'>Libras</button>";
  html += "</div>";
  html += "<script>";
  html += "function updateWeight() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.onreadystatechange = function() {";
  html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "      document.getElementById('weightValue').innerHTML = 'Peso: ' + xhr.responseText;";
  html += "    }";
  html += "  };";
  html += "  xhr.open('GET', '/getWeight', true);";
  html += "  xhr.send();";
  html += "}";
  html += "function changeUnit(unit) {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.onreadystatechange = function() {";
  html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "      document.getElementById('currentUnit').textContent = unit;";
  html += "      currentUnit = unit;";
  html += "    }";
  html += "  };";
  html += "  xhr.open('GET', '/setUnit?unit=' + unit, true);";
  html += "  xhr.send();";
  html += "}";
  html += "setInterval(updateWeight, 500);"; // Actualiza el peso cada 2 segundos (ajusta el intervalo según tus necesidades)
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleGetWeight() {
  server.send(200, "text/plain", String(reading));
}

void handleSetUnit() {
  String unit = server.arg("unit");
  if(unit == "g" ){
    mode = 0;
  }
  else if(unit == "ml"){
    mode = 1;
  }
  else if(unit == "oz"){
    mode = 2;
  }
  else if (unit == "lb")
  {
    mode = 3;
  }
  
  currentUnit = unit;// Agrega aquí la lógica para cambiar la unidad de medida
  server.send(200, "text/plain", unit);
}
void displayWeight(int weight){
  oled.clearDisplay();
  oled.setCursor(0, 10); oled.println("Weight:");
  oled.setCursor(0, 30); oled.println(weight);
  oled.print(" ");oled.print("g");
  oled.sendBuffer();  
}
void displaymL(int weight){
  oled.clearDisplay();
  oled.setCursor(0, 10); oled.println("Volume:");
  oled.setCursor(0, 30); oled.println(weight);
  oled.print(" ");oled.print("ml");
  oled.sendBuffer();  
}
void displayOz(int weight){
  oled.clearDisplay();
  oled.setCursor(0, 10); oled.println("Ounce:");
  oled.setCursor(0, 30); oled.println(weight);
  oled.print(" ");oled.print("Oz");
  oled.sendBuffer();  
}
void displayPounds(int weight){
  oled.clearDisplay();
  oled.setCursor(0, 10); oled.println("Pounds:");
  oled.setCursor(0, 30); oled.println(weight);
  oled.print(" ");oled.print("lb");
  oled.sendBuffer();  
}


void setup() {
  Wire.begin();
  oled.begin();
  oled.setFont(u8g2_font_ncenB08_tr);
  oled.clearBuffer();
  Serial.begin(115200); // Iniciar puerto serial para depuración
  Serial.println("Initializing scale...");
  scale.begin(DOUT, SCK); // Iniciar el HX711
  scale.set_scale(CALIBRATION_FACTOR);   // this value is obtained by calibrating the scale with known weights
  scale.tare();  

  // Conéctate a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
 // Imprime la dirección IP asignada por el router
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
  oled.clearBuffer(); oled.setCursor(0,50);oled.print("Connected to WiFi. IP address: "); oled.setCursor(0,60);
  oled.print(WiFi.localIP()); oled.sendBuffer();

  // Configura el manejador de la ruta raíz para mostrar el peso
  server.on("/", HTTP_GET, handleRoot);
  // Configura el manejador para obtener el peso
  server.on("/getWeight", HTTP_GET, handleGetWeight);
  // Inicia el servidor web
  // Configura el manejador para cambiar la unidad de medida
  server.on("/setUnit", HTTP_GET, handleSetUnit);
  // Inicia el servidor web
  server.begin(); 
}

void loop() {
  server.handleClient();
  oled.setCursor(0,50);oled.print("Connected to WiFi. IP address: "); oled.setCursor(0,60);oled.print(WiFi.localIP()); oled.sendBuffer();
  if (button.getSingleDebouncedPress()){
    Serial.print("tare...");
    oled.clearBuffer(); oled.print("tare..."); oled.sendBuffer();
    scale.tare();
  }
  if(button2.getSingleDebouncedPress()){
    mode = mode + 1;
    if(mode > 3)
    {
      mode = 0;
    }
  }

  if (scale.wait_ready_timeout(200)) {
    reading = round(scale.get_units(10));
    if (reading != lastReading){
      if(mode==0){
        Serial.print("Weight: ");
        Serial.println(reading);
        displayWeight(reading);
        currentUnit = "g";
      }
      else if(mode==1){
        Serial.print("Volume: ");
        Serial.println(reading);
        displaymL(reading);
        currentUnit = "ml";
      }
      else if(mode==2){
        Serial.print("Ounces: ");
        reading = reading*oz_conversion;
        Serial.println(reading);
        displayOz(reading);
        currentUnit = "oz";
      }
      else{
        Serial.print("Pounds: ");
        reading = reading*pound_conversion;
        Serial.println(reading);
        displayPounds(reading);
        currentUnit = "lb";
      }
    }
    lastReading = reading;
  }
  else {
    Serial.println("HX711 not found.");
    oled.clearBuffer(); oled.print("HX711 not found."); oled.sendBuffer();
  }
  delay(100);
}
