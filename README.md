# 🌱 Air Pollution Monitoring and Mitigation System

## 📌 Project Overview

Our project is an **Air Pollution Monitoring and Mitigation System** that detects ambient air pollution and tests how effectively a multi-stage filtration system cleans particulate matter ($\text{PM}_{2.5}$) and toxic gases in real time.

By placing a PM sensor **before** the filters (at the front intake) and a second PM sensor **after** the filters (at the exhaust), the system calculates and displays the exact percentage of air pollution removed.

The intake fan, filtration stack, and sensors are located **inside the bottle (chamber)**, while the Arduino Uno R4 WiFi, OLED display, Status LEDs, NeoPixel Bargraphs, Power Modules, and Control Circuit are mounted **beside the bottle**.

---

## 🎯 Objectives

* Measure particulate matter ($\text{PM}_{2.5}$) at both intake and exhaust to show real-time filtration reduction percentage.
* Detect Total Volatile Organic Compounds (TVOC / Gas).
* Filter fine particles and hazardous gases through physical and chemical filter layers.
* Measure temperature and humidity to correct optical dust sensor readings.
* Display live metrics on an OLED screen and stream data to the **Blynk Cloud**.
* Use dual **10-LED RGB NeoPixel Strips** to compare $\text{PM}_{2.5}$ levels before intake vs. after exhaust (1–10 severity levels).
* Use **Green and Red Status LEDs** to show overall air quality status.
* Automatically adjust fan purification speed based on Air Quality Index (AQI).

---

## 🌬️ How the Airflow Works

The front blower fan forces polluted air into the top of the bottle, pushing it through the sensors, past the filter layers, and blowing clean air out through the bottom outlet.

```text
         AIR IN
            ↓
   ┌────────────────┐
   │ Blower Fan     │  <-- 5500 RPM high-pressure turbine (Front Intake)
   │ HM3301 #1 (IN) │  <-- Measures incoming polluted air (PM2.5 IN)
   │ DHT11          │
   │ SGP30 TVOC     │
   │                │
   │ Raw Cotton     │  <-- Pre-filter (large dust/hair/coarse debris)
   │ Cotton Pad     │  <-- Secondary medium particle capture layer
   │ Teabag Layer   │  <-- Porous micro-mesh & organic odor/gas absorber
   │ HEPA Filter    │  <-- Traps fine sub-micron particulates (PM2.5)
   │                │
   │ HM3301 #2 (OUT)│  <-- Measures clean filtered air (PM2.5 OUT)
   └────────┬───────┘
            ↓
         AIR OUT

```

---

## 🧴 Inside the Bottle (Chamber)

* **High-Speed Front Blower Fan (5500 RPM / 15W):** Mounted at the front intake to actively blow polluted air directly into the filter stack.
* **Dual HM3301 $\text{PM}_{2.5}$ Sensors:** Sensor #1 measures raw polluted air at the intake. Sensor #2 measures clean air at the exhaust. Comparing both gives the exact filtration reduction percentage.
* **DHT11 Sensor:** Measures temperature and relative humidity so the system can adjust optical sensor readings when air moisture is high.
* **SGP30 Gas Sensor:** Measures Total Volatile Organic Compounds (TVOC in ppb) for gas pollution.
* **Raw Cotton:** Acts as an initial coarse pre-filter to catch large dust particles, hair, and floating fibers.
* **Cotton Pad:** High-density layered fibers that trap medium-sized airborne dust and particles.
* **Teabag Layer:** Fine porous plant-fiber mesh that provides micro-stage physical screening and light gas/odor filtration.
* **HEPA Filter:** High-efficiency particle layer that traps tiny micro-particles down to 0.3 microns ($\text{PM}_{2.5}$).

---

## 🔌 Electronics Beside the Bottle

* **Arduino Uno R4 WiFi:** Main controller processing sensor data, controlling fan speeds, updating displays, and sending Wi-Fi data to Blynk Cloud.
* **TCA9548A I2C Multiplexer (Purple Board):** Lets the Arduino talk to two identical HM3301 sensors without signal conflicts.
* **MOSFET Control Circuit:** Safely controls fan speed using an IRLZ44N MOSFET, 200 Ohm resistor, 20k Ohm resistor, 1N4007 diode, and DC Jack module.
* **OLED Display (SSD1306):** Local 128x64 screen showing live AQI, $\text{PM}_{2.5}$ before, $\text{PM}_{2.5}$ after, reduction percentage, TVOC gas, fan speed %, temperature, and humidity.

```text
┌─────────────────────┐
│ AQI       : 82      │
│ PM2.5 bf  : 65 ug/m³│
│ PM2.5 af  : 10 ug/m³│
│ REDUCTION : 84.6%   │
│ VOC       : 10 ppb  │
│ FAN       : 78%     │
│ T: 29C    H: 68%    │
└─────────────────────┘

```

* **Status LEDs (Pins D7 & D8):**
* 🟢 **Solid Green LED (D7):** Healthy Air (AQI 0–50)
* 🟢 **Blinking Green LED (D7):** Moderate Air (AQI 51–100)
* 🔴 **Solid Red LED (D8):** Unhealthy Air (AQI > 100)


* **Dual 10-LED RGB NeoPixel Strips (Pins D2 & D3):**
* **Pin D2 Strip ($\text{PM}_{2.5}$ IN):** Visualizes incoming raw particulate matter severity (1 to 10 lights)
* **Pin D3 Strip ($\text{PM}_{2.5}$ OUT):** Visualizes filtered output particulate matter severity (1 to 10 lights)
* **Color Zones:** LEDs 1–3 (🟢 Green), LEDs 4–6 (🟡 Yellow), LEDs 7–10 (🔴 Red)


* **LiPo Rider Plus & Battery:** Powers the Arduino, OLED, sensors, and LED lights portably via USB.
* **DC Wall Supply + DC Jack Module:** Powers the high-power blower fan separately so it doesn't overload the Arduino.

---

## ⚙️ How the System Works

1. The **Front Blower Fan** drives polluted air into the intake chamber.
2. **HM3301 Sensor #1** reads incoming $\text{PM}_{2.5}$ levels (`PM2.5 bf`) and updates the **D2 RGB Strip ($\text{PM}_{2.5}$ IN)**.
3. **SGP30 Sensor** reads gas levels (TVOC) with humidity correction from the **DHT11**.
4. The Arduino calculates overall **AQI** and sets fan speed:
* **AQI 0–50 (Healthy):** Solid Green LED ON | Fan Speed = **40% (PWM 100)**
* **AQI 51–100 (Moderate):** Blinking Green LED | Fan Speed = **80% (PWM 200)**
* **AQI > 100 (Unhealthy):** Solid Red LED ON | Fan Speed = **90% (PWM 230)**


5. Air is forced through Raw Cotton ➔ Cotton Pad ➔ Teabag Layer ➔ HEPA Filter.
6. Clean air passes **HM3301 Sensor #2** at the output (`PM2.5 af`) and updates the **D3 RGB Strip ($\text{PM}_{2.5}$ OUT)**.
7. Arduino calculates filtration reduction percentage and sends telemetry to the **OLED Screen** and **Blynk IoT App**.

---

## 🛠️ Technical Problems & Hardware Resolutions

During system design, three main hardware challenges were identified and solved:

### Resolution 1: Dual I2C Address Conflict

* **Problem:** Both HM3301 dust sensors have the exact same factory I2C address (`0x40`). Plugging both directly into the Arduino causes data corruption and crashes the system.
* **Resolution:** Added a **TCA9548A I2C Multiplexer module (Purple Board)**. It allows the Arduino to switch between Channel 0 (Intake Sensor) and Channel 1 (Exhaust Sensor) seamlessly.

### Resolution 2: High Fan Power Demand

* **Problem:** The 15W high-speed blower fan draws up to 3 Amps of current. Arduino digital pins can only output 0.015 Amps, and battery modules max out at 2 Amps. Connecting the fan directly to the Arduino will burn the microcontroller.
* **Resolution:** Used an **IRLZ44N MOSFET Circuit** connected to an external high-current power supply via a **DC Jack Module**. The Arduino pin only sends a tiny 5V control signal to the MOSFET gate, while external power runs directly to the fan.

### Resolution 3: Shared Ground & Motor Spike Protection

* **Problem:** High-power motors create inductive voltage spikes when turning off, which can destroy digital electronics or reset the microcontroller.
* **Resolution:** Added a **1N4007 flyback diode** across the fan terminals to absorb voltage spikes, a **20k Ohm pull-down resistor** to keep the fan off during bootup, and linked the DC Jack ground directly to Arduino GND so control signals stay stable.

---

## 🔗 System Layout

```text
                🧴 BOTTLE (CHAMBER)                                     ELECTRONICS
          ┌───────────────────────────────────┐
          │ 15W Front Blower Fan              │◄─── D5 (PWM Control Signal)
 AIR IN ─►│ HM3301 #1 (Intake PM)             │
          │ SGP30 Gas Sensor                  │
          │ DHT11 Temp & Humidity             │
          │                                   │        ┌──────────────────────┐
          │ Raw Cotton ──► Cotton Pad         │        │ TCA9548A Multiplexer │
          │       │                           │◄─I2C──│ (Channel 0 & 1)      │
          │ Teabag Layer ──► HEPA Filter      │        └──────────┬───────────┘
          │       │                           │                   │ I2C Main Bus
          │ HM3301 #2 (Exhaust PM)            │                   ▼
          └───────────────┬───────────────────┘        ┌──────────────────────┐
                          │                            │                      │──────(WiFi)─────► ☁️ BLYNK CLOUD
                       AIR OUT                         │                      │
                                                       │  Arduino Uno R4 WiFi │──────(I2C)──────► 🖥️ OLED DISPLAY
                                                       │                      │
                                                       └─┬───┬───┬───┬───┬────┘
                                                         │   │   │   │   │
                                                         │   │   │   │   └──D7 ──► 🟢 GREEN LED (Status)
                                                         │   │   │   └──────D8 ──► 🔴 RED LED (Status)
                                                         │   │   └──────────D2 ──► 📊 10-LED RGB Strip (PM2.5 IN)
                                                         │   └──────────────D3 ──► 📊 10-LED RGB Strip (PM2.5 OUT)
                                                         │
                                                        D5 (PWM)
                                                         │
                                                         ▼
                                            ┌──────────────────────────┐
                                            │ IRLZ44N MOSFET Driver    │◄─── Wall Power Adapter (DC Jack)
                                            │ (Resistors + Diode)      │
                                            └──────────────────────────┘
                                                         │
                                                         ▼
                                              LiPo Rider + Battery (USB) ──► Powers Arduino & Sensors

```

---

## 🧩 Main Components

| Component | Pin / Interface | Function |
| --- | --- | --- |
| **Arduino Uno R4 WiFi** | Main Controller | Brain of system, data processing, Wi-Fi telemetry |
| **TCA9548A Multiplexer** | I2C (`0x70`) | Resolves address conflict for dual PM sensors |
| **HM3301 PM Sensor #1** | TCA Channel 0 (`SD0/SC0`) | Measures raw incoming particulate matter (`PM2.5 bf`) |
| **HM3301 PM Sensor #2** | TCA Channel 1 (`SD1/SC1`) | Measures clean exhaust particulate matter (`PM2.5 af`) |
| **SGP30 Gas Sensor** | I2C (`0x58`) | Measures TVOC toxic gas concentration |
| **DHT11 Sensor** | Digital Pin `D6` | Measures temperature and humidity |
| **OLED Display (SSD1306)** | I2C (`0x3C`) | Local readout showing AQI, values, and reduction % |
| **Green Status LED** | Digital Pin `D7` | Solid ON (Healthy), Blinking (Moderate) |
| **Red Status LED** | Digital Pin `D8` | Solid ON (Unhealthy) |
| **PM IN NeoPixel Strip** | Digital Pin `D2` | 10-LED RGB Bargraph ($\text{PM}_{2.5}$ intake severity meter) |
| **PM OUT NeoPixel Strip** | Digital Pin `D3` | 10-LED RGB Bargraph ($\text{PM}_{2.5}$ exhaust severity meter) |
| **15W Blower Fan** | Digital Pin `D5` (PWM) | Front air intake driver, pushes air through filters (5500 RPM) |
| **MOSFET Control Board** | Circuit Interface | IRLZ44N + Resistors + Diode for safe fan switching |
| **DC Jack Module** | External Power | Plug interface for high-current fan power adapter |
| **LiPo Rider Plus & Battery** | Battery Power | Portable power supply for Arduino and 5V sensors |
| **Multi-Stage Filter Stack** | Mechanical Chamber | Physical & chemical filtration (Raw Cotton/Cotton Pad/Teabag/HEPA) |

---

## 🌱 Environmental Purpose & SDG Alignment

### SDG 3 – Good Health and Well-Being

This project directly aligns with **SDG 3 (Good Health and Well-Being)**, specifically **Target 3.9**, by demonstrating real-time indoor air purification and pollution monitoring. By actively removing hazardous airborne fine particles ($\text{PM}_{2.5}$) and chemical gases (TVOC) from breathing space, the system mitigates respiratory exposure risks and helps safeguard personal lung health.

---

## ⚠️ Safety & Technical Notes

* **High-Current Blower Power:** The 15W high-speed blower fan draws up to 3 Amps under load. It **must be powered via an external power supply through an IRLZ44N MOSFET driver circuit and DC Jack module**, and never directly from an Arduino I/O pin or low-current battery board.
* **Sensor Calibration & Accuracy:** Prototype optical dust measurements are subject to air velocity and humidity fluctuations. Environmental measurements serve as an operational demonstration of relative purification performance rather than certified laboratory readings.
