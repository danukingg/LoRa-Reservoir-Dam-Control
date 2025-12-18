# LoRa-Based Water Level Telemetry & Automatic Sluice Gate Control

![ESP32](https://img.shields.io/badge/ESP32-DevKit%20V1-blue?style=for-the-badge&logo=espressif)
![LoRa](https://img.shields.io/badge/Connectivity-LoRa%20SX1278-orange?style=for-the-badge)
![Node-RED](https://img.shields.io/badge/Dashboard-Node--RED-red?style=for-the-badge&logo=node-red)
![Status](https://img.shields.io/badge/Status-Field%20Tested-success?style=for-the-badge)

This project is an **IoT solution** designed to monitor reservoir water levels in real-time and automatically control floodgates using **Long Range (LoRa)** communication technology.

The system addresses the limitations of manual monitoring by providing a robust, low-power telemetry system capable of transmitting data up to **5 km** (Line of Sight) and integrating with **Node-RED** for dashboard monitoring and email alerts.

## 📸 System Architecture

| **Transmitter Unit (Reservoir)** | **Receiver Unit (Monitoring Post)** |
|:--------------------------------:|:-----------------------------------:|
| <img src="hardware/Transmitter.jpeg" height="250" alt="Transmitter Wiring"> | <img src="hardware/Receiver.jpeg" height="250" alt="Receiver Wiring"> |
| *Reads sensor & controls servo* | *Receives LoRa packet & pushes to MQTT* |

## 🛠️ Tech Stack & Components

Based on the implementation design:

| Category | Component / Technology | Function |
| :--- | :--- | :--- |
| **Microcontroller** | **ESP32 DevKit V1** | Main processing unit for both Tx and Rx nodes. |
| **Communication** | **LoRa SX1278 (433 MHz)** | Long-range wireless data transmission (tested up to 100m non-LOS). |
| **Sensors** | **HW-038 (Analog)** | Measures water level based on conductivity. |
| **Actuator** | **Servo MG996R** | Controls the miniature sluice gate mechanism. |
| **Protocol** | **MQTT & JSON** | Data transport from Receiver to Server. |
| **Dashboard** | **Node-RED** | Visualization, logic flow, and Email Notification system. |

## 🧠 How It Works

### 1. Transmitter Unit (At the Reservoir)
* Reads the water level percentage (0-100%).
* **Automatic Control:** If the water level exceeds **85%** (Danger Threshold), the system automatically opens the sluice gate (Servo moves to 90°) to release water.
* Encodes data into **JSON** format and transmits it via LoRa.

### 2. Receiver Unit (Monitoring Station)
* Receives the LoRa packet via radio frequency.
* Connects to Wi-Fi and publishes the payload to an **MQTT Broker** (HiveMQ).

### 3. Node-RED Dashboard
* Subscribes to the MQTT topic `esp32/workshop/data`.
* Parses the JSON data.
* **Alert System:** If the water level is critical (>85%), it triggers an **Email Notification** to the administrator containing the water level and RSSI signal strength.

## 📊 Performance Test Results

Field testing yielded the following results:

| Distance | Water Level | Data Transmission | Servo Response |
| :--- | :--- | :--- | :--- |
| **0 m** | 0% | ✅ Success | Closed (0°) |
| **10 m** | 90% | ✅ Success | **Open (90°)** |
| **50 m** | 30% | ✅ Success | Closed (0°) |
| **100 m** | 100% | ✅ Success | **Open (90°)** |

> **Result:** The system achieved a **100% data success rate** up to 100 meters in open area testing, with stable RSSI levels.

## 📂 Folder Structure

* `/firmware`: Contains `.ino` source code for both Transmitter and Receiver units.
* `/hardware`: Wiring diagrams and schematic designs.
* `/dashboard`: Node-RED flow configuration (`flow.json`).
* `/docs`: Full academic journal and flowcharts.

## 🚀 Getting Started

Follow these steps to deploy and test the system locally.

### 1. Prerequisites
Before starting, ensure you have the following hardware and software:
* 2× ESP32 DevKit V1
* 2× LoRa SX1278 Modules (433 MHz)
* 1× HW-038 Water Level Sensor
* 1× MG996R Servo Motor
* **Software:** Arduino IDE & Node-RED (Local or VPS)

### 2. Hardware Setup
1.  **Transmitter Unit:** Assemble the ESP32, Sensor, and Servo according to the wiring diagram in `/hardware`.
    > **Note:** Use a stable external power supply for the Servo to prevent ESP32 brownouts.
2.  **Receiver Unit:** Connect the second ESP32 with the LoRa module.
3.  Ensure GPIO connections match the pin definitions in the firmware code.

### 3. Firmware Deployment
**For Transmitter:**
1.  Open `/firmware/transmitter_unit/Transmitter.ino` in Arduino IDE.
2.  Select Board: **ESP32 Dev Module**.
3.  Upload the code.

**For Receiver:**
1.  Open `/firmware/receiver_unit/Receiver.ino`.
2.  Update the Wi-Fi credentials (`ssid` & `password`) and MQTT Broker address.
3.  Upload the code.

### 4. Dashboard Setup (Node-RED)
1.  Run Node-RED locally or on your server.
2.  Import the flow file: `Menu` > `Import` > Select `/dashboard/flows.json`.
3.  Deploy the flow and access the UI at `http://localhost:1880/ui`.

---

## 📡 System Data Flow

The data travels through the system in the following order:

`Water Level Sensor` → `ESP32 (Tx)` → **LoRa RF** → `ESP32 (Rx)` → **MQTT Broker** → `Node-RED` → `Email Alert`

**Automatic Control Logic:**
* When water level > **85%** (Danger Threshold) ➡️ Servo Opens (**90°**).
* When water level < **85%** (Safe) ➡️ Servo Closes (**0°**).

---

## 🧪 Limitations & Future Improvements

**Current Limitations:**
* Field testing range is currently limited to **100 meters** (Open Area).
* System relies on external power; no battery backup yet.
* No encryption on LoRa packets.

**Future Roadmap:**
* [ ] Long-range testing (> 1 km LOS).
* [ ] Implement Solar + Battery power management.
* [ ] Add predictive flood control using Machine Learning.
