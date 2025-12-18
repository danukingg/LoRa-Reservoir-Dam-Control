# LoRa-Based Water Level Telemetry & Automatic Sluice Gate Control

This project is an IoT solution designed to monitor reservoir water levels in real-time and automatically control floodgates using **Long Range (LoRa)** communication technology.

[cite_start]The system addresses the limitations of manual monitoring by providing a robust, low-power telemetry system capable of transmitting data up to **5 km** (Line of Sight) and integrating with **Node-RED** for dashboard monitoring and email alerts[cite: 425, 475].

## 📸 System Architecture
| **Transmitter Unit (Reservoir)** | **Receiver Unit (Monitoring Post)** |
|:--------------------------------:|:-----------------------------------:|
| ![Tx Wiring](hardware/wiring_transmitter.jpg) | ![Rx Wiring](hardware/wiring_receiver.jpg) |
| *Reads sensor & controls servo* | *Receives LoRa packet & pushes to MQTT* |


## 🛠️ Tech Stack & Components
[cite_start]Based on the implementation design[cite: 361, 442]:

| Category | Component / Technology | Function |
| :--- | :--- | :--- |
| **Microcontroller** | **ESP32 DevKit V1** | Main processing unit for both Tx and Rx nodes. |
| **Communication** | **LoRa SX1278 (433 MHz)** | Long-range wireless data transmission (up to 100m tested in non-LOS). |
| **Sensors** | **HW-038 (Analog)** | Measures water level based on conductivity. |
| **Actuator** | **Servo MG996R** | Controls the miniature sluice gate mechanism. |
| **Protocol** | **MQTT & JSON** | Data transport from Receiver to Server. |
| **Dashboard** | **Node-RED** | Visualization, logic flow, and Email Notification system. |

## 🧠 How It Works

### 1. Transmitter Unit (At the Reservoir)
* Reads the water level percentage (0-100%).
* [cite_start]**Automatic Control:** If the water level exceeds **85%** (Danger Threshold), the system automatically opens the sluice gate (Servo moves to 90°) to release water[cite: 363, 472].
* Encodes data into **JSON** format and transmits it via LoRa.

### 2. Receiver Unit (Monitoring Station)
* Receives the LoRa packet via radio frequency.
* [cite_start]Connects to Wi-Fi and publishes the payload to an **MQTT Broker** (HiveMQ)[cite: 426].

### 3. Node-RED Dashboard
* Subscribes to the MQTT topic `esp32/workshop/data`.
* Parses the JSON data.
* [cite_start]**Alert System:** If the water level is critical (>85%), it triggers an **Email Notification** to the administrator containing the water level and RSSI signal strength[cite: 479].

## 📊 Performance Test Results
[cite_start]Field testing yielded the following results[cite: 500, 501]:

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

---
*Based on the research: "Sistem Telemetri dan Telekontrol Ketinggian Air Waduk Berbasis LoRa SX1278"*
