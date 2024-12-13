# esp32_ble_imu
A BLE IMO (Accel/Gyro) device implementation, for M5StickCplus2/Esp32.
Intented for real time motion tracking and sensor data collection.

Currently using the M5StickCPlus2 library for Display and IMU.
Esp32 Native BLE implementation.


# Bluetooth Low Energy (BLE) Service

## Service UUID
**Service:** `509B0001-EBE1-4AA5-BC51-11004B78D5CB`

## Characteristics

### Read/Notify Characteristic
**Characteristic UUID:** `509B0002-EBE1-4AA5-BC51-11004B78D5CB`

#### Description
This characteristic returns a comma-separated list of floats representing sensor data in the following format:
```
accelx,accely,accelz,gyrox,gyroy,gyroz
```
- **accelx**: Acceleration along the X-axis in Gs

- **accely**: Acceleration along the Y-axis in Gs

- **accelz**: Acceleration along the Z-axis in Gs

- **gyrox**: Angular velocity around the X-axis

- **gyroy**: Angular velocity around the Y-axis

- **gyroz**: Angular velocity around the Z-axis


#### Example Value
```
0.03,-0.01,1.01,0.34,-0.23,0.14
```