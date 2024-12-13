#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <M5StickCPlus2.h>

TaskHandle_t Task1;
TaskHandle_t Task2;

float laccel[3]={0.0, 0.0, 0.0};
float lgyro[3]={0.0, 0.0, 0.0};
SemaphoreHandle_t mutex;

void TaskPollIMU(void *pvParameters) {
  Serial.write("Poll IMU Started..\r\n");
  while (1) {
    auto imu_update = StickCP2.Imu.update();
    if (imu_update) {
        M5.Lcd.setCursor(0, 40);
        M5.Lcd.clear();  // Delay 100ms 延迟100ms

        auto data = M5.Imu.getImuData();
        xSemaphoreTake(mutex, portMAX_DELAY);
        for (int i = 0; i < 3; i++) {
            laccel[i] = data.accel.value[i];
        }
        for (int i = 0; i < 3; i++) {
            lgyro[i] = data.gyro.value[i];
        }
        xSemaphoreGive(mutex);
        Serial.print("IMU Data - Accel X: "); Serial.print(data.accel.x);
        Serial.print(", Y: "); Serial.print(data.accel.y);
        Serial.print(", Z: "); Serial.println(data.accel.z);

        M5.Lcd.printf("IMU:\r\n");
        M5.Lcd.printf("%0.2f %0.2f %0.2f\r\n", data.accel.x,
                                data.accel.y, data.accel.z);
        M5.Lcd.printf("%0.2f %0.2f %0.2f\r\n", data.gyro.x,
                                data.gyro.y, data.gyro.z);
    }
    vTaskDelay(10);
  }
}

void TaskBLE(void *pvParameters) {
  Serial.write("BLE Started..\r\n");
  BLEDevice::init("M5_IMU");
  BLEDevice::setMTU(100);
  BLEServer *pServer = BLEDevice::createServer();
  // Set up BLE characteristics and services here
  BLEService *pIMUService = pServer->createService("509B0001-EBE1-4AA5-BC51-11004B78D5CB"); // Create the service

 // Create characteristics
  BLECharacteristic *pSensorCharacteristic = pIMUService->createCharacteristic(
    "509B0002-EBE1-4AA5-BC51-11004B78D5CB",
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  BLECharacteristic *pCalibrationCharacteristic = pIMUService->createCharacteristic(
    "509B0003-EBE1-4AA5-BC51-11004B78D5CB",
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  pIMUService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  while (1) {
    // Handle BLE communication
    xSemaphoreTake(mutex, portMAX_DELAY);
    String sensorData = String(laccel[0]) + "," + String(laccel[1]) + "," + String(laccel[2]) + "," + String(lgyro[0]) + "," + String(lgyro[1]) + "," + String(lgyro[2]);
    xSemaphoreGive(mutex);
    pSensorCharacteristic->setValue(sensorData.c_str());
    pSensorCharacteristic->notify();
    vTaskDelay(100); // BLE handling interval
  }
}

void setup() {
  vTaskDelay(1000);
  Serial.begin(115200);

  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0,10);

  mutex = xSemaphoreCreateBinary();
  if (mutex == NULL) {
      Serial.println("Failed to create mutex");
      while (1); // Halt if mutex creation fails
  }
  xSemaphoreGive(mutex);
  // Create tasks
  xTaskCreatePinnedToCore(TaskPollIMU, "PollIMU", 10000, NULL, 1, &Task1, 1); // Core 1
  xTaskCreatePinnedToCore(TaskBLE, "BLE", 10000, NULL, 1, &Task2, 0); // Core 0
}

void loop() {
  vTaskDelay(10);
  //TaskPollIMU(NULL);
  // Empty loop as tasks are handled in separate cores
}