All examples despite 2sensors.py assume that a BME68X sensor
is connected via I2c port 1 with de address 0x77.

If you are using address 0x76, change the constructor call
of BME68X accordingly (constants.BME68X_I2C_ADDR_LOW).

If running any example returns a
Remote I/O error this most likely means the sensor is not connected properly
In this case check if all wires are connected firmly and
if you are using the right I2C address.