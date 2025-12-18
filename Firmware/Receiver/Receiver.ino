#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>

// --- CHANGE THIS (WiFi) ---
const char* ssid = "Your Wifi";
const char* password = "Your Password";
// -----------------

// --- LORA PINS (MATCHING YOUR WIRING) ---
#define LORA_SS    5  // GPIO5 (D5) -> NSS
#define LORA_RST   14 // GPIO14 (D14) -> RST
#define LORA_DIO0  2  // GPIO2 (D2) -> DIO0
// -----------------

// MQTT Broker Details
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/workshop/data"; // MQTT Topic

WiFiClient espClient;
PubSubClient client(espClient);

// Global variables to hold LoRa messages
volatile bool loraDataReceived = false;
String loraMessage = "";
int loraRssi = 0;

void setup_wifi() {
  delay(10);
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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32-Gateway-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// NEW FUNCTION: Called when a LoRa packet is received
void onReceive(int packetSize) {
  if (packetSize == 0) return; // No packet received

  // Read packet
  loraMessage = "";
  while (LoRa.available()) {
    loraMessage += (char)LoRa.read();
  }
  
  loraRssi = LoRa.packetRssi();
  loraDataReceived = true; // Set flag indicating we have new data

  Serial.print("LoRa Received: '");
  Serial.print(loraMessage);
  Serial.print("' with RSSI: ");
  Serial.println(loraRssi);
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial monitor
  Serial.println("LoRa MQTT Gateway");

  // Setup WiFi and MQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // Setup LoRa
  Serial.println("Initializing LoRa...");
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(433E6)) { // Must match the Transmitter frequency
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initialized.");

  // Set receiver mode and register callback function
  LoRa.onReceive(onReceive);
  LoRa.receive();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check if there is new LoRa data (from onReceive function)
  if (loraDataReceived) {
    loraDataReceived = false; // Reset flag

    // --- Parse JSON from LoRa ---
    JSONVar loraData = JSON.parse(loraMessage);

    if (JSON.typeof(loraData) == "undefined") {
      Serial.println("Failed to parse LoRa JSON");
      LoRa.receive(); // Return to receive mode
      return;
    }

    // Add RSSI data to the JSON object
    loraData["rssi"] = loraRssi;

    // Convert back to string to send to MQTT
    String mqttPayload = JSON.stringify(loraData);
    
    // Publish complete JSON to MQTT
    if (client.publish(mqtt_topic, mqttPayload.c_str())) {
      Serial.print("Published to MQTT: ");
      Serial.println(mqttPayload);
    } else {
      Serial.println("MQTT Publish Failed.");
    }
    
    // Return LoRa to receive mode
    LoRa.receive();
  }
}