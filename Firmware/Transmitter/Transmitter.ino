#include <SPI.h>
#include <LoRa.h>
#include <ESP32Servo.h> // Library for Servo

// --- LORA PINS (MATCHING YOUR WIRING) ---
#define LORA_SS    5   // GPIO5 (D5) -> NSS
#define LORA_RST   14  // GPIO14 (D14) -> RST
#define LORA_DIO0  15  // GPIO15 (D15) -> DIO0
// -----------------

// --- NEW SENSOR & SERVO PINS ---
#define SENSOR_PIN 34 // Pin "S" from HW-038 Sensor (ADC1_CH6)
#define SERVO_PIN  12 // Signal Pin (Orange) from SG90 Servo
// -----------------

Servo myservo; // Create servo object

// --- IMPORTANT: SENSOR CALIBRATION ---
// You MUST adjust these values based on your hardware.
// Check Serial Monitor when sensor is completely DRY (record the number)
// Check Serial Monitor when sensor is FULLY SUBMERGED (record the number)
const int SENSOR_DRY = 0;    // EXAMPLE: Analog value when dry (e.g., 0)
const int SENSOR_WET = 4000; // EXAMPLE: Analog value when submerged (e.g., 4000)
// (Note: Depending on the sensor/wiring, values might be inverted. Adjust accordingly)
// -----------------


void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Transmitter (Sensor Station)");

  // Setup LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) { // MUST MATCH RECEIVER FREQUENCY
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Setup Servo
  myservo.attach(SERVO_PIN);
  myservo.write(0); // Start at 0 degrees
  
  // Setup Sensor Pin
  pinMode(SENSOR_PIN, INPUT); // Set sensor pin as input
}

void loop() {
  // 1. Read Water Level Sensor
  int rawValue = analogRead(SENSOR_PIN);
  
  // 2. Convert to Percentage (0-100%)
  // Use map() to map your calibration values
  // Note: Mapping from SENSOR_DRY to SENSOR_WET becomes 0 -> 100
  long levelPercent = map(rawValue, SENSOR_DRY, SENSOR_WET, 0, 100);
  
  // Constrain value to stay within 0-100 (in case of outlier readings)
  levelPercent = constrain(levelPercent, 0, 100);

  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" | Percent: ");
  Serial.print(levelPercent);
  Serial.println("%");
  
  // 3. Create Data Packet (JSON)
  char jsonBuffer[50];
  // JSON Format: {"level": 50}
  snprintf(jsonBuffer, 50, "{\"level\":%d}", (int)levelPercent);

  // 4. Send LoRa Packet
  LoRa.beginPacket();
  LoRa.print(jsonBuffer);
  LoRa.endPacket();
  
  Serial.print("Sending packet: ");
  Serial.println(jsonBuffer);

  // 5. Move Servo (logic based on water level)
  if (levelPercent >= 85) {
      myservo.write(90); // Move to 90 degrees
      delay(500);
  } else { 
      myservo.write(0);  // Move to 0 degrees
      delay(500);
  }

  // Total delay approx 10 seconds (including servo delays)
  delay(5000); 
}