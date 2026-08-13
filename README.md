# 🌱 Air Pollution Monitoring and Mitigation System

## 📌 Project Overview

Our project is an **Air Pollution Monitoring and Mitigation System** that detects air pollution and tests how effectively a filtration system can reduce particulate matter.

The system uses **two PM sensors** to measure the air before and after filtration. This allows us to compare the readings and calculate the percentage of particle reduction.

The filtration system is placed **inside a bottle**, while the Arduino, OLED display, RGB LED, relay/MOSFET and power supply are placed **beside the bottle**.

---

## 🎯 Objectives

* Detect particulate matter such as PM2.5 and PM10.
* Measure air quality before and after filtration.
* Filter particles from polluted air.
* Calculate the percentage of particle reduction.
* Measure temperature and humidity.
* Detect air-quality-related gases.
* Display the results on an OLED.
* Use an RGB LED to show the air-quality condition.

---

## 🌬️ How the Airflow Works

Polluted air enters the bottle and passes through the sensors and filtration layers.

```text
         AIR IN
           ↓
   ┌───────────────┐
   │ PM SENSOR #1  │
   │  BEFORE       │
   │  FILTRATION   │
   │               │
   │ DHT22         │
   │ Gas Sensor    │
   │               │
   │ PRE-FILTER    │
   │ Coffee Filter │
   │               │
   │ COTTON        │
   │               │
   │ ACTIVATED     │
   │ CARBON        │
   │               │
   │ HEPA FILTER   │
   │               │
   │ PM SENSOR #2  │
   │  AFTER        │
   │  FILTRATION   │
   └───────┬───────┘
           ↓
    STRONG DC BLOWER
           ↓
        AIR OUT
```

---

## 🧴 Inside the Bottle

The bottle acts as the **airflow and filtration chamber**.

### PM Sensor #1

Measures the particle concentration **before filtration**.

### DHT11

Measures:

* Temperature
* Humidity

These measurements are useful because environmental conditions can affect particle readings.

Your PM sensor likely works by **detecting how particles scatter light**. Humidity can change the particles themselves—for example, some particles absorb water and become larger—which can change how much light they scatter. That can make an optical PM sensor report a different concentration even when the actual dry particle mass hasn't changed proportionally. Research reviewed by EPA has found that relative humidity can affect low-cost optical PM measurements.

### Gas Sensor

Measures air-quality-related gases such as TVOC and eCO₂, depending on the sensor used.

### Coffee Filter

Acts as a **pre-filter** to catch some larger particles.

### Cotton

Provides another physical filtration layer.

### Activated Carbon

Helps adsorb certain gases and organic compounds and can help reduce some odors.

### HEPA Filter

Helps capture very fine particles.

### PM Sensor #2

Measures the particle concentration **after filtration**.

### Strong DC Blower

Moves the air through the entire system.

---

## 🔌 Electronics Beside the Bottle

The electronic components are kept **outside the bottle**.

### Arduino Uno R4 WiFi

The main controller that receives sensor data, processes the readings and controls the system.

### OLED Display

Shows the measurements and filtration results.

Example:

```text
┌─────────────────────┐
│     OLED DISPLAY    │
│ AQI SCORE: 82       │
│ PM2.5 bf: 65 μ g/m³ │
│ PM2.5 af: 24 μ g/m³ │
│ VOC GAS : 10 ppb    │
│ REDUCTION: 63%      │
│ T: 29°C    H: 68%   │
└─────────────────────┘
```

### RGB LED

Shows the air-quality condition:

```text
🟢 Green  = Healthy
🟡 Yellow = Moderate
🔴 Red    = Unhealthy
```

### LiPo Rider Plus

Manages the rechargeable LiPo battery and provides power to the project components

### LiPo Battery

Stores electrical energy and supplies power to the Arduino and other components when the system is running without a direct power adapter

---

## 📊 Before vs After

The two PM sensors allow us to measure the effectiveness of the filter.

For example:

```text
BEFORE = 65
AFTER  = 24
```

The reduction is calculated using:

```text
Reduction (%) = (Before - After) / Before × 100
```

Example:

```text
Reduction = (65 - 24) / 65 × 100
          ≈ 63%
```

Therefore, the system measured approximately **63% particle reduction** in this example.

---

## ⚙️ How the System Works

1. Polluted air enters the bottle.
2. PM Sensor #1 measures the particles before filtration.
3. The DHT11 measures temperature and humidity.
4. The gas sensor measures air-quality-related gases.
5. Air passes through the coffee filter.
6. Air passes through the cotton layer.
7. Air passes through activated carbon.
8. Air passes through the HEPA filter.
9. PM Sensor #2 measures the particles after filtration.
10. The blower moves the air through the system.
11. The Arduino compares the two PM readings.
12. The percentage reduction is calculated.
13. The OLED displays the results.
14. The RGB LED shows the air-quality condition.

---

## 🔗 System Layout

```text
                    🧴 BOTTLE                         ELECTRONICS

              ┌───────────────────┐
              │ PM SENSOR #1      │
AIR IN ──────→│ BEFORE FILTRATION │
              │                   │        ┌───────────────────┐
              │ DHT11             │───────→│   OLED DISPLAY    │
              │ Gas Sensor        │        └───────────────────┘
              │                   │
              │ Coffee Filter     │               🔴 🟡 🟢
              │       ↓           │              RGB LED
              │ Cotton            │
              │       ↓           │        ┌───────────────────┐
              │ Activated Carbon  │───────→│ Arduino Uno R4    │
              │       ↓           │        │       WiFi        │
              │ HEPA Filter       │        └─────────┬─────────┘
              │       ↓           │                  │
              │ PM SENSOR #2      │           ┌──────┴──────────┐
              │ AFTER FILTRATION  │           │ Lipo Rider Plus │
              │       ↓           │           └──────┬──────────┘
              │ Strong DC Blower  │                  │
              └────────┬──────────┘                  ↓
                       ↓                      ┌──────────────┐
                    AIR OUT                   │ Battery      │
                                              └──────────────┘
```

---

## 🧩 Main Components

| Component           | Function                                      |
| ------------------- | --------------------------------------------- |
| Arduino Uno R4 WiFi | Main controller                               |
| PM Sensor #1        | Measures particles before filtration          |
| PM Sensor #2        | Measures particles after filtration           |
| DHT11               | Measures temperature and humidity             |
| Gas Sensor          | Measures air-quality-related gases            |
| Coffee Filter       | Pre-filtration                                |
| Cotton              | Physical filtration                           |
| Activated Carbon    | Adsorption of certain gases/organic compounds |
| HEPA Filter         | Fine particle filtration                      |
| Strong DC Blower    | Moves air through the system                  |
| OLED Display        | Displays measurements and reduction           |
| RGB LED             | Shows air-quality condition                   |
| LiPo Rider Plus     | Manages and supplies battery power            |
| Lipo Battery        | Provides power to the system                  |
| Jumper Wires        | Connects the electronic components            |
| Bottle              | Airflow and filtration chamber                |

---

## 🌱 Environmental Purpose

The project does more than simply detect pollution. It also **tests a possible mitigation method** by filtering the polluted air and comparing the particle concentration before and after filtration.

This allows us to show:

**Polluted Air → Measurement → Filtration → Measurement → Reduction**

The project can be used as a STEM demonstration to help students understand **air pollution, particulate matter, sensors, filtration and environmental protection**.

---

## 🌍 Sustainable Development Goal (SDG)

### SDG 13 – Climate Action

Our project supports **SDG 13: Climate Action** by monitoring air pollution and testing a filtration method that can reduce particulate matter in polluted air.

---

## ⚠️ Important Note

The prototype's readings can be affected by environmental conditions such as **temperature, humidity, airflow and particle concentration**.

The calculated reduction is based on the measurements from the prototype and should not be treated as professional air-quality certification.

The **12V blower must be powered by a suitable external 12V DC power supply** and should not be powered directly from an Arduino GPIO pin.
