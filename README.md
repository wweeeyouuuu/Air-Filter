# Air Pollution Monitoring and Mitigation System

## 1. Project Overview

Our project is an **Air Pollution Monitoring and Mitigation System** designed to detect air pollution and help reduce the impact of polluted air.

The system uses sensors to measure **particulate matter and gases in the air**. The readings are processed by an **Arduino Uno R4 WiFi**. An RGB LED shows the air-quality condition using different colours.

A **12V 5015 DC blower** can also be used to control airflow through a filter or ventilation system when needed.

---

## 2. Problem

Air pollution can be caused by:

* Vehicle exhaust
* Industrial activities
* Burning of materials
* Dust
* Smoke
* Fine particles in the air
* Harmful gases

Small particles such as **PM2.5** are especially important because they are small enough to remain suspended in the air.

Our project aims to monitor these pollutants and provide an easy-to-understand warning when the air quality becomes unhealthy.

---

## 3. Objectives

The objectives of our project are to:

1. Measure particulate matter such as **PM2.5 and PM10**.
2. Detect changes in air quality using a **gas sensor**.
3. Display air-quality information clearly.
4. Use an **RGB LED** to indicate the air-quality condition.
5. Use the Arduino Uno R4 WiFi as the main controller.
6. Provide a system that can potentially help reduce exposure to polluted air.

---

## 4. Main Components

| Component              | Function                                                      |
| ---------------------- | ------------------------------------------------------------- |
| Arduino Uno R4 WiFi    | Controls the whole system and processes sensor readings       |
| HM330X Particle Sensor | Measures particulate matter such as PM2.5 and PM10            |
| SGP30 Gas Sensor       | Measures air-quality-related gas levels such as TVOC and eCO₂ |
| RGB LED                | Shows the air-quality condition using different colours       |
| 0.96" OLED Display     | Displays sensor readings and system information               |
| 5015 12V DC Blower     | Moves air through the system/filter                           |
| 12V DC Adapter         | Provides power for the 12V blower                             |
| Relay/MOSFET           | Allows the Arduino to control the blower safely               |
| Wires                  | Connects the components                                       |
| Filter material        | Helps remove some particles from the airflow                  |

---

## 5. How It Works

The system works in several stages:

### Step 1 — Air enters the system

Air containing dust, particles or gases enters the monitoring area.

### Step 2 — Sensors measure the air

The particle sensor measures particulate matter such as PM2.5 and PM10.

The gas sensor detects changes in air-quality-related gases.

### Step 3 — Arduino processes the readings

The sensors send their readings to the **Arduino Uno R4 WiFi**.

The Arduino compares the readings with the selected air-quality limits.

### Step 4 — RGB LED indicates the condition

The RGB LED changes colour depending on the measured condition.

For example:

* 🟢 **Green** — Healthy
* 🟡 **Yellow** — Moderate
* 🔴 **Red** — Unhealthy

### Step 5 — Blower helps move air

The 5015 blower moves air through the system.

Because the blower operates at **12V**, it should be powered by a separate 12V DC power supply rather than directly from an Arduino pin.

---

## 6. Environmental Conditions

Environmental conditions can affect particle measurements.

Important factors include:

### Humidity

High humidity can cause some particles to absorb water and become larger, which can affect sensor readings.

### Temperature

Temperature changes can affect air properties and sensor performance.

### Wind and airflow

Air movement can change how particles reach the sensor.

### Rain

Rain can remove some particles from the air, causing particle concentrations to decrease.

Therefore, environmental conditions should be considered when comparing measurements.

---

## 7. System Flow

```text
             AIR
              ↓
      ┌───────────────┐
      │ Particle      │
      │ Sensor        │
      └───────┬───────┘
              │
              ↓
      ┌───────────────┐
      │ Gas Sensor    │
      └───────┬───────┘
              │
              ↓
      ┌───────────────────┐
      │ Arduino Uno R4    │
      │ WiFi              │
      └───────┬───────────┘
              │
       ┌──────┴──────┐
       ↓             ↓
   RGB LED         OLED
       │
       ↓
 Air Quality     Sensor
  Indicator      Readings

              +
              │
              ↓
        ┌───────────┐
        │  Blower   │
        │   12V     │
        └───────────┘
              ↑
              │
        12V Adapter
```

---

## 8. Why Use an External Power Supply?

The Arduino is mainly used as the **controller**.

The 5015 blower requires **12V**, while the Arduino cannot safely provide the required power directly from its GPIO pins.

Therefore:

**12V adapter → powers blower**

**Arduino → controls blower**

This prevents the blower from drawing too much current from the Arduino.

---

## 9. Possible Applications

This system could be developed for:

* Schools
* Laboratories
* Workshops
* Indoor environmental monitoring
* Areas near roads
* Industrial areas
* Environmental education projects

It can also be used as a STEM project to demonstrate how sensors, Arduino, electronics and environmental science can work together.

---

## 10. Advantages

* Real-time air-quality monitoring
* Easy-to-understand RGB indication
* Can display actual sensor readings
* Uses Arduino Uno R4 WiFi
* Can potentially be connected to IoT monitoring
* Helps students understand air pollution
* Can be expanded with additional sensors

---

## 11. Limitations

The sensors used in a prototype may not have the same accuracy as professional air-quality monitoring equipment.

Environmental conditions such as humidity, temperature, wind and rain can also affect measurements.

Therefore, the project should be considered a **school-level environmental monitoring prototype**, not a replacement for professional air-quality monitoring stations.

---

## 12. Future Improvements

Possible future improvements include:

* Add Blynk IoT for remote monitoring.
* Add temperature and humidity sensors.
* Store measurements on an SD card.
* Add a real-time clock (RTC).
* Improve the air-filtering system.
* Automatically control the blower based on pollution levels.
* Create graphs showing changes in PM2.5 and PM10.
* Add more air-quality sensors for comparison.

---

## 13. Project Goal

The main goal of this project is to **detect air pollution, show the air-quality condition clearly, and explore a practical way to help manage polluted air using sensors, an Arduino and an airflow/filtering system.**
