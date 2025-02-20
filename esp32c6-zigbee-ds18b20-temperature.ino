#include <OneWire.h>
#include <DallasTemperature.h>
#include "Zigbee.h"

/* Zigbee temperature sensor configuration */
#define TEMP_SENSOR_ENDPOINT_NUMBER 10
#define ONE_WIRE_BUS 4 // GPIO 4 for DS18B20 data line
uint8_t button = BOOT_PIN;

// Optional Time cluster variables
struct tm timeinfo;
struct tm *localTime;
int32_t timezone = 46800;  // NZDT (UTC +13:00)

ZigbeeTempSensor zbTempSensor = ZigbeeTempSensor(TEMP_SENSOR_ENDPOINT_NUMBER);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

/************************ Temp sensor *****************************/
static void temp_sensor_value_update(void *arg) {
  for (;;) {
    // Request temperature measurement
    sensors.requestTemperatures();
    // Read temperature sensor value
    float tsens_value = sensors.getTempCByIndex(0);
    Serial.printf("Updated temperature sensor value to %.2f°C\r\n", tsens_value);
    // Update temperature value in Temperature sensor EP
    zbTempSensor.setTemperature(tsens_value);
    delay(5000);
  }
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);

  // Init button switch
  pinMode(button, INPUT_PULLUP);

  // Initialize DS18B20 sensor
  sensors.begin();

  // Optional: set Zigbee device name and model
  zbTempSensor.setManufacturerAndModel("Espressif", "ZigbeeTempSensor");

  // Set minimum and maximum temperature measurement value
  zbTempSensor.setMinMaxValue(-55, 125); // DS18B20 range

  // Optional: Set tolerance for temperature measurement in °C (lowest possible value is 0.01°C)
  zbTempSensor.setTolerance(0.5);

  // Optional: Time cluster configuration (default params, as this device will receive time from coordinator)
  zbTempSensor.addTimeCluster();

  // Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbTempSensor);

  Serial.println("Starting Zigbee...");
  // When all EPs are registered, start Zigbee in End Device mode
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Zigbee started successfully!");
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Optional: If time cluster is added, time can be read from the coordinator
  timeinfo = zbTempSensor.getTime();
  timezone = zbTempSensor.getTimezone();

  Serial.println("UTC time:");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  time_t local = mktime(&timeinfo) + timezone;
  localTime = localtime(&local);

  Serial.println("Local time with timezone:");
  Serial.println(localTime, "%A, %B %d %Y %H:%M:%S");

  // Start Temperature sensor reading task
  xTaskCreate(temp_sensor_value_update, "temp_sensor_update", 2048, NULL, 10, NULL);

  // Set reporting interval for temperature measurement in seconds, must be called after Zigbee.begin()
  // min_interval and max_interval in seconds, delta (temp change in 0.1 °C)
  // if min = 1 and max = 0, reporting is sent only when temperature changes by delta
  // if min = 0 and max = 10, reporting is sent every 10 seconds or temperature changes by delta
  // if min = 0, max = 10 and delta = 0, reporting is sent every 10 seconds regardless of temperature change
  zbTempSensor.setReporting(1, 60, 2);
}

void loop() {
  // Checking button for factory reset
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    zbTempSensor.reportTemperature();
  }
  delay(100);
}
