#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Definiert die analogen Eingänge
int AINF_0 = A0;
int AINF_1 = A1;
int AINF_2 = A2;
int AINF_3 = A3;

// Variablen für die analogen Werte in mV
int AINF_0_mv = 0;
int AINF_1_mv = 0;
int AINF_2_mv = 0;
int AINF_3_mv = 0;

// WiFi Settings
const char* ssid = "HERZOG";
const char* password = "Herzog1234";

// IOT Settings
// POST --data "{"temperature":42,"humidity":73}" https://demo.thingsboard.io/api/v1/ABC123/telemetry --header "Content-Type:application/json"
const char* server = "http://demo.thingsboard.io/api/v1/bG3gZepT5dmBFaoHJtNP/telemetry";

void setup() {
  // Initialisierung der seriellen Schnittstelle mit 115'222 baud
  Serial.begin(115200);
  delay(10);

  // Initialisierung der analogen Eingänge
  pinMode(AINF_0, INPUT);
  pinMode(AINF_1, INPUT);
  pinMode(AINF_2, INPUT);
  pinMode(AINF_3, INPUT);

  // Verbindungsaufbau zu WLAN
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}
int counter = 0;
void loop() { 
  // Liest die analogen Werte vom Port
  ain_get_values();

  // Gibt die analogen Werte über die serielle Schnittstelle aus
  ain_print_values();

  // Sendet die Werte an die IOT Plattform
  if(counter > 10){
  WiFiClient client;
  HTTPClient http;

  http.begin(client, server);
  http.addHeader("Content-Type", "application/json");

  // Vorbereitung der JSON Nachricht
  StaticJsonDocument<200> doc;
  //doc["temperature"] = AINF_0_mv;
  doc["AINF_0[mV]"] = AINF_0_mv;
  doc["AINF_1[mV]"] = AINF_1_mv;
  doc["AINF_2[mV]"] = AINF_2_mv;
  doc["AINF_3[mV]"] = AINF_3_mv;

  String data = String("");
  serializeJson(doc, data);
  
  int httpResponseCode = http.POST(data);
  //Serial.printf("httpResponseCode:%d\n",httpResponseCode);
  http.end();
  counter = 0;}
  
  // Warte für 2000ms
  delay(100);
  counter += 1;
}



void ain_get_values() {
  // read all the analog inputs in millivolts und mittelt sie mit Hilfe eines gleitenden Mittelwert-Filters
  AINF_0_mv = 0.7 * AINF_0_mv + 0.3 * analogReadMilliVolts(AINF_0);
  AINF_1_mv = 0.7 * AINF_1_mv + 0.3 * analogReadMilliVolts(AINF_1);
  AINF_2_mv = 0.7 * AINF_2_mv + 0.3 * analogReadMilliVolts(AINF_2);
  AINF_3_mv = 0.7 * AINF_3_mv + 0.3 * analogReadMilliVolts(AINF_3);
}

void ain_print_values() {
  // Gibt die analogen Werte über die serielle Schnittstelle aus
  Serial.printf("AINF_0[mV]:%d,", AINF_0_mv);
  Serial.printf("AINF_1[mV]:%d,", AINF_1_mv);
  Serial.printf("AINF_2[mV]:%d,", AINF_2_mv);
  Serial.printf("AINF_3[mV]:%d\n", AINF_3_mv);
}
