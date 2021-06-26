# This example demonstrates the FORCED MODE
# First with BSEC disabled
# Then with BSEC enabled

from bme68x import BME68X
import bme68xConstants as cst
import bsecConstants as bsec
from time import sleep

print('TESTING FORCED MODE WITHOUT BSEC')
bme = BME68X(cst.BME68X_I2C_ADDR_HIGH, bsec.BSEC_DISABLE)
print(bme.get_data())
sleep(3)
print()
print('TESTING FORCED MODE WIT BSEC')
bme = BME68X(cst.BME68X_I2C_ADDR_HIGH, bsec.BSEC_ENABLE)
print(bme.get_bsec_data())
sleep(3)
