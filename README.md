# 🌱 Air Pollution Monitoring and Mitigation System

## 📌 Project Overview

Our project is an **Air Pollution Monitoring and Mitigation System** that detects air pollution and tests how effectively a filtration system can reduce particulate matter and toxic gases.

The filtration system is placed **inside a bottle**, while the Arduino Uno R4 WiFi, OLED display, Status LEDs, RGB NeoPixel Bargraphs, Lipo Rider Plus, and power management components are placed **beside the bottle**.

---

## 🎯 Objectives

* Detect particulate matter (PM2.5) and total volatile organic compounds (TVOC / Gas).
* Filter particles and gases from polluted air through physical and chemical filtration layers.
* Measure environmental conditions (temperature and humidity) for sensor accuracy.
* Display real-time telemetry on an OLED screen and stream data to the **Blynk Cloud**.
* Use dual **10-LED RGB NeoPixel Strips** to display 1–10 severity levels for PM2.5 and Gas.
* Use discrete **Green and Red Status LEDs** to indicate overall AQI state.
* Automatically adjust fan purification speed based on Air Quality Index (AQI).

---

## 🌬️ How the Airflow Works

Polluted air enters the bottle and passes through the sensors and filtration layers.

```text
         AIR IN
            ↓
   ┌────────────────┐
   │ HM3301 PM      │
   │ DHT11          │
   │ SGP30 TVOC     │
   │                │
   │ Coffee Filter  │
   │                │
   │ Cotton Layer   │
   │                │
   │ ACTIVATED      │
   │ CARBON         │
   │                │
   │ HEPA FILTER    │
   │                │
   │ 12V DC Fan     │
   └────────┬───────┘
            ↓
         AIR OUT

```

---

## 🧴 Inside the Bottle

The bottle acts as the **airflow and filtration chamber**.

### HM3301 PM2.5 Sensor

Measures real-time particulate matter concentration ($\mu\text{g/m}^3$) before filtration using laser light scattering.

### DHT11 Sensor

Measures **Temperature** and **Relative Humidity**. Humidity directly affects optical PM sensor accuracy because airborne particles absorb moisture and swell, altering light scattering. Measuring humidity enables accurate sensor compensation.

### SGP30 Gas Sensor

Measures Total Volatile Organic Compounds (**TVOC** in ppb) with absolute humidity compensation applied dynamically from the DHT11 readings.

### Coffee Filter

Acts as a **pre-filter** to catch coarse dust and large airborne debris.

### Cotton Layer

Provides secondary physical filtration for medium-sized particles.

### Activated Carbon

Adsorbs volatile organic compounds, chemical gases, and odors.

### HEPA Filter

Captures fine particulate matter down to 0.3 microns.

### 12V DC Fan (Transistor PWM Driven)

Pulls air through the entire filtration chamber at variable speeds based on AQI demand.

---

## 🔌 Electronics Beside the Bottle

The electronic control and visual indicators are kept **outside the bottle**.

### Arduino Uno R4 WiFi

The core microcontroller processing sensor data, controlling hardware outputs, driving displays, and syncing with the Blynk Cloud over Wi-Fi.

### OLED Display (SSD1306 128x64)

Provides a local user interface showing live AQI, PM2.5, TVOC, Fan Speed %, Temperature, and Humidity.

```text
┌─────────────────────┐
│ AQI: 82             │
│ PM2.5 : 65 ug/m³    │
│ VOC   : 10 ppb      │
│ FAN   : 78%         │
│ T: 29C   H: 68%     │
└─────────────────────┘

```

### 🚥 System Status LEDs (Pins D7 & D8)

Indicates the overall air quality classification based strictly on calculated **AQI**:

* 🟢 **Solid Green LED (Pin D7):** Healthy Air (AQI 0 – 50)
* 🟢 **Blinking Green LED (Pin D7):** Moderate Air (AQI 51 – 100)
* 🔴 **Solid Red LED (Pin D8):** Unhealthy Air (AQI > 100)

### 📊 Dual 10-LED RGB NeoPixel Bargraphs (Pins D2 & D3)

Two separate 10-LED addressable RGB strips visualize severe level meters (1–10 lights active):

* **Pin D2 Strip:** Displays PM2.5 severity level
* **Pin D3 Strip:** Displays TVOC Gas severity level
* **Color Zones per Strip:**
* LEDs 1–3: 🟢 **Green** (Low)
* LEDs 4–6: 🟡 **Yellow** (Moderate)
* LEDs 7–10: 🔴 **Red** (High)



### LiPo Rider Plus & LiPo Battery

Manages battery charging and boosts power to run the microcontroller setup portably.

---

## ⚙️ How the System Works

1. Polluted air enters the intake chamber.
2. The **HM3301 PM Sensor** reads PM2.5 particle density ($\mu\text{g/m}^3$).
3. The **SGP30 Gas Sensor** measures TVOC levels (ppb) with humidity compensation from the **DHT11**.
4. Arduino maps PM2.5 and TVOC values into **10-level bargraph outputs** for the NeoPixel strips on D2 and D3.
5. Arduino calculates overall **AQI** from PM2.5:
* **AQI 0–50 (Healthy):** Solid Green LED ON | Fan Speed = **PWM 100**
* **AQI 51–100 (Moderate):** Blinking Green LED | Fan Speed = **PWM 200**
* **AQI > 100 (Unhealthy):** Solid Red LED ON | Fan Speed = **PWM 230**


6. Air passes through Coffee Filter $\rightarrow$ Cotton $\rightarrow$ Activated Carbon $\rightarrow$ HEPA Filter.
7. Telemetry data updates on the local **OLED Display** and transmits over Wi-Fi to the **Blynk IoT Dashboard**.

---

## 🔗 System Layout

```text
                    🧴 BOTTLE                               ELECTRONICS

              ┌───────────────────┐
              │ HM3301 PM Sensor  │
AIR IN ──────→│ SGP30 Gas Sensor  │───────I2C───────┐
              │ DHT11 Sensor      │                 │
              │                   │                 ▼
              │ Coffee Filter     │        ┌───────────────────┐      ☁️ BLYNK CLOUD
              │        ↓          │        │                   │──────(WiFi Dashboard)
              │ Cotton Layer      │        │                   │
              │        ↓          │        │  Arduino Uno R4   │      🖥️ OLED DISPLAY
              │ Activated Carbon  │        │       WiFi        │──────(128x64 I2C)
              │        ↓          │        │                   │
              │ HEPA Filter       │        └─┬───┬───┬───┬───┬─┘
              │        ↓          │          │   │   │   │   │
              │ 12V DC Fan (PWM)  │◄──D5─────┘   │   │   │   └───D7 ──► 🟢 GREEN LED (Status)
              └────────┬──────────┘              │   │   └───────D8 ──► 🔴 RED LED (Status)
                       ↓                         │   └───────────D2 ──► 📊 10-LED RGB Strip (PM)
                    AIR OUT                      └───────────────D3 ──► 📊 10-LED RGB Strip (Gas)
                                                                 │
                                                                 ▼
                                                        ┌─────────────────┐
                                                        │ Lipo Rider Plus │◄──► 🔋 LiPo Battery
                                                        └─────────────────┘

```

---

## 🧩 Main Components

| Component | Pin / Interface | Function |
| --- | --- | --- |
| **Arduino Uno R4 WiFi** | Main MCU | Central processor, logic, Wi-Fi telemetry |
| **HM3301 PM Sensor** | I2C (`0x40`) | Measures PM2.5 concentration ($\mu\text{g/m}^3$) |
| **SGP30 Gas Sensor** | I2C (`0x58`) | Measures TVOC gas concentration (ppb) |
| **DHT11 Sensor** | Digital Pin `D6` | Measures Temperature and Relative Humidity |
| **OLED Display (SSD1306)** | I2C (`0x3C`) | Displays 128x64 live system telemetry |
| **Green Status LED** | Digital Pin `D7` | Solid ON (AQI 0–50), Blinking (AQI 51–100) |
| **Red Status LED** | Digital Pin `D8` | Solid ON (AQI > 100) |
| **PM NeoPixel Strip** | Digital Pin `D2` | 10-LED RGB Bargraph (PM2.5 Level 1–10) |
| **Gas NeoPixel Strip** | Digital Pin `D3` | 10-LED RGB Bargraph (TVOC Level 1–10) |
| **12V DC Blower Fan** | Digital Pin `D5` (PWM) | Air intake driver (PWM 100 / 200 / 230) |
| **NPN Transistor / MOSFET** | Circuit Interface | Safely switches 12V fan power via Arduino D5 |
| **Filter Media** | Physical Chamber | Coffee filter, Cotton, Activated Carbon, HEPA |
| **LiPo Rider Plus & Battery** | Power | Power management and portable battery supply |

---

## 🌱 Environmental Purpose & SDG Alignment

### SDG 13 – Climate Action & SDG 3 – Good Health and Well-Being

This project demonstrates actionable monitoring and air purification logic by comparing particulate concentration against multi-stage filtration capabilities in real time.

---

## ⚠️ Safety & Technical Note

* **12V Fan Power:** The 12V DC fan draws higher current than microcontrollers can supply. It **must be powered via an external 12V supply or boosted line via a driver transistor/MOSFET**, never directly from an Arduino I/O pin.
* **Sensor Accuracy:** Prototype measurements are affected by humidity and airflow velocity; values serve as an operational demonstration rather than certified laboratory readings.
