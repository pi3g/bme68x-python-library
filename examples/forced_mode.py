# This example demonstrates the FORCED MODE
# First with BSEC disabled
# Then with BSEC enabled

from bme68x import BME68X
import bme68xConstants as cst
import bsecConstants as bsec
from time import sleep

print('TESTING FORCED MODE WITHOUT BSEC')
bme = BME68X(cst.BME68X_I2C_ADDR_HIGH, 1)
# Configure sensor to measure at 320 degC for 100 millisec
bme.set_heatr_conf(cst.BME68X_FORCED_MODE, 320, 100, cst.BME68X_ENABLE)
print(bme.get_data())
sleep(3)
print('\nTESTING FORCED MODE WITH BSEC')
bme = BME68X(cst.BME68X_I2C_ADDR_HIGH, 1)
bme.set_sample_rate(bsec.BSEC_SAMPLE_RATE_LP)


def get_data(sensor):
    data = {}
    try:
        data = sensor.get_bsec_data()
    except Exception as e:
        print(e)
        return None
    if data == None or data == {}:
        sleep(0.1)
        return None
    else:
        sleep(3)
        return data


bsec_data = get_data(bme)
while bsec_data == None:
    bsec_data = get_data(bme)
print(bsec_data)
