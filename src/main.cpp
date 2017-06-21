// Go into idle sleep mode (until next 1 ms timer tick) when task chain empty.
#define _TASK_SLEEP_ON_IDLE_RUN

// Support Status Requests for tasks.
#define _TASK_STATUS_REQUEST

// Support task IDs and control points.
#define _TASK_WDT_IDS

// Support local task storage.
#define _TASK_LTS_POINTER

#include <TaskScheduler.h>

// USER SETTINGS ************************************************************

// Device name (also functions as hostname for WiFi purposes).
#define DEV_NAME          "weatherstation"

// INCLUDES *****************************************************************

// BMP280 support.
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// WiFi support.
#include <ESP8266WiFi.h>

// Expose Espressif SDK functionality
extern "C"
{
#include "user_interface.h"
}

// TASK DECLARATIONS ********************************************************

void _sensorMonitor();
bool _sensorStartup();

// Task for the sensor monitoring function.
Task _tSensor (2UL * TASK_SECOND, TASK_FOREVER, &_sensorMonitor, NULL, false, &_sensorStartup, NULL);

// SENSOR SUPPORT *************************************************************

Adafruit_BMP280 bmp;

// Perform initial setup of the sensors.
bool _sensorStartup()
{
  return bmp.begin(0x76);
}

// Monitor the sensors.
void _sensorMonitor()
{
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
}

// TASK SCHEDULER ***********************************************************

Scheduler taskManager;

// ENTRYPOINTS **************************************************************

bool woken = false;

// Called once when the board is reset (i.e., initialises or awakens from
// [deep] sleep).
void setup()
{
  // Set serial monitor baud rate.
  Serial.begin(57600);

  // Where did we come from?
  const rst_info * resetInfo = system_get_rst_info();
  woken = resetInfo->reason == 5;

  // Enable debug output.
  Serial.setDebugOutput(true);

  Serial.println (DEV_NAME);
  Serial.println ("booting...");

  // Initialise the task manager.
	taskManager.init();

  // Add the system tasks to the task manager and enable them.
  taskManager.addTask(_tSensor);
  _tSensor.enable();
}

// Loops endlessly after setup() finishes and for so long as the board is
// powered and awake.
void loop()
{
	taskManager.execute();
}
