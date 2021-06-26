# This example demonstrates that 2 BME68X sensors can be used over the I2C port 1
# It is assumed that
# A BME68X sensor is connected via I2C address 0x76 (default address for BME680)
# A BME68X sensor is connected via I2C address 0x77 (default address for BME688)

from bme68x import BME68X
import bme68xConstants as bme
import bsecConstants as bsec
from time import sleep

sensors = [BME68X(bme.BME68X_I2C_ADDR_LOW, bsec.BSEC_ENABLE),
           BME68X(bme.BME68X_I2C_ADDR_HIGH, bsec.BSEC_ENABLE)]
for i in range(0, 10):
    print('BME680 ')
    print(sensors[0].get_data())
    print('BME688 ')
    print(sensors[1].get_data())
    sleep(3)
