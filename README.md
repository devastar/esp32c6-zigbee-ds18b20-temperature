# ESP32-C6 Zigbee Temperature Sensor

The **ESP32-C6 Zigbee Temperature Sensor** project demonstrates how to use the ESP32-C6 board to read temperature data from a DS18B20 sensor and transmit it over a Zigbee network. The project is designed to work with the Arduino IDE and leverages available libraries for Zigbee communication and temperature sensing.

---

## Overview

This project turns your ESP32-C6 into a Zigbee-enabled temperature sensor node that:
- **Reads temperature data** from a connected DS18B20 sensor.
- **Sends the temperature readings** over Zigbee to a central coordinator or gateway.
- **Provides a starting point** for developing IoT solutions with embedded Zigbee connectivity.

---

## Code (.ino) Analysis

The accompanying Arduino sketch (`project.ino`) is structured into several key sections:

1. **Library Inclusions**  
   The top of the file includes the necessary libraries required by the project. For example:
   ```cpp
   #include <Zigbee.h>            // Handles Zigbee communication
   #include <OneWire.h>           // OneWire protocol library for DS18B20
   #include <DallasTemperature.h> // Library for DS18B20 temperature sensor
   ```
   Remove any other sensor libraries if present.

2. **Global Parameters & Configuration**  
   Global parameters are defined for both Zigbee and sensor configurations, such as:
   ```cpp
   // Zigbee network settings
   uint8_t zigbeeChannel = 15;
   uint16_t panID = 0x1A2B;
   uint8_t deviceID = 0x01;
   
   // DS18B20 Sensor settings
   #define SENSOR_PIN 4        // GPIO pin connected to the DS18B20 data line
   unsigned long measurementInterval = 5000; // Time between measurements in milliseconds
   ```
   **Note:** Ensure you have a 4.7kΩ pull-up resistor connected from the DS18B20 data line to the power supply (3.3V or 5V, depending on your board).

3. **`setup()` Function**  
   The `setup()` function initializes:
   ```cpp
   void setup() {
       Serial.begin(115200);
       // Initialize Zigbee communication
       Zigbee.begin(zigbeeChannel, panID, deviceID);
       
       // Initialize DS18B20 Sensor
       OneWire oneWire(SENSOR_PIN);
       DallasTemperature sensors(&oneWire);
       sensors.begin();
       
       // Additional setup (error handling, calibration, etc.)
   }
   ```
   **Important:** Consider declaring `OneWire oneWire` and `DallasTemperature sensors` as global variables if they need to be accessed in `loop()`.

4. **`loop()` Function**  
   The `loop()` function typically includes:
   ```cpp
   void loop() {
       // Request temperature measurement from the DS18B20 sensor
       sensors.requestTemperatures();
       float temperature = sensors.getTempCByIndex(0);
       
       // Transmit temperature data over Zigbee
       Zigbee.sendData(temperature);
       
       // Wait until the next measurement cycle
       delay(measurementInterval);
   }
   ```

---

## Library Details

- **Zigbee Library**:  
  Provides functions to:
  - Configure the Zigbee module.
  - Manage network join and data transmission.
  Ensure that the library version supports the ESP32-C6 architecture.

- **OneWire and DallasTemperature Libraries**:  
  These libraries are used to interface with the DS18B20 sensor:
  - **OneWire** handles the communication over the one-wire bus.
  - **DallasTemperature** simplifies retrieval of temperature readings.
  Install these libraries via the Arduino Library Manager if needed.

---

## Parameters to Adjust

Inside the `.ino` file, you can fine-tune behavior by modifying these parameters:

- **Sensor Pin:**  
  - `#define SENSOR_PIN 4`  
  Adjust this value to match the wiring for your DS18B20 sensor.

- **Zigbee Settings:**  
  - `uint8_t zigbeeChannel = 15;`  
  - `uint16_t panID = 0x1A2B;`  
  - `uint8_t deviceID = 0x01;`  
  Update these values to fit your Zigbee network configuration.

- **Measurement Interval:**  
  - `unsigned long measurementInterval = 5000; // in milliseconds`  
  Change this value to adjust how frequently the temperature is read and transmitted.

---

## Installation & Deployment

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/your_username/esp32-c6_zigbee_temperature.git
   cd esp32-c6_zigbee_temperature
   ```

2. **Install Required Libraries:**
   - Open Arduino IDE.
   - Navigate to **Sketch > Include Library > Manage Libraries...**
   - Search for and install:
     - **Zigbee** library (ensure compatibility with ESP32-C6)
     - **OneWire** library
     - **DallasTemperature** library

3. **Adjust the `.ino` File:**
   - Open the Arduino sketch file (`project.ino`).
   - Modify the parameters (sensor pin, Zigbee settings, etc.) as needed.

4. **Compile and Upload:**
   - Connect your ESP32-C6 board.
   - Select the correct board in **Tools > Board > ESP32C6**.
   - Compile and upload the sketch to your board.

---

## Flashing Instructions

To flash the firmware using the Arduino IDE, please make sure to follow these steps and notes:

1. **Install the ESP32 by Espressif Boards Library:**  
   - Go to **File > Preferences** in Arduino IDE.
   - In the **Additional Boards Manager URLs** field, add the following URL if it's not already present:  
     `https://dl.espressif.com/dl/package_esp32_index.json`
   - Open **Tools > Board > Boards Manager**, search for **ESP32 by Espressif Systems**, and install the package.

2. **Board and Port Setup:**  
   - Select your board under **Tools > Board > ESP32C6 Boards**.
   - Select the correct **Port** for your ESP32-C6.

3. **Set the Zigbee Mode to End Device:**  
   - In **Tools**, ensure that the Zigbee mode is set to **End Device** since this project is intended to operate as an end node in the Zigbee network.

4. **Select Partition Scheme:**  
   - In **Tools > Partition Scheme**, choose **Zigbee 4MB with SPIFFS**. This partition layout is required for proper Zigbee operation and storage.

5. **Enable USB CDC on Boot:**  
   - Ensure that the **USB CDC on Boot** option is enabled under **Tools**. This setting allows the serial monitor to function properly when the ESP32-C6 boots up.

6. **Compile and Flash:**  
   - Click the **Verify** button to compile the sketch.
   - Once the compilation is successful, click the **Upload** button to flash the firmware to your ESP32-C6 board.

7. **Open the Serial Monitor:**  
   - After flashing, open the Serial Monitor (**Tools > Serial Monitor**), set the baud rate to **115200**, and observe the output.

---

## Troubleshooting

- **Compilation Errors:**  
  Ensure all libraries are properly installed and that you have selected the correct board and port in the Arduino IDE.

- **Zigbee Connectivity Issues:**  
  Verify your Zigbee settings (channel, PAN ID) and ensure the device is within range of the Zigbee coordinator.

- **Sensor Read Errors:**  
  Double-check your wiring, especially the DS18B20 sensor connection and the pull-up resistor.

- **Flashing Issues:**  
  If you encounter problems, double-check that:
  - The **ESP32 by Espressif Boards** library is installed.
  - The board settings in Arduino IDE match the requirements (end device mode, correct partition, USB CDC enabled).

---

## Contributing

Contributions, bug reports, and feature requests are welcome! Please fork this repository and submit a pull request with your changes.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details. 