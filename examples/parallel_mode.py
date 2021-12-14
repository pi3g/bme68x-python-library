# This example tests and demonstrates the BME68X_PARALLEL_MODE
# Without use of BSEC and
# With use of BSEC

from bme68x import BME68X
import bme68xConstants as cnst
import bsecConstants as bsec
from time import sleep

temp_prof = [320, 100, 100, 100, 200, 200, 200, 320, 320, 320]
dur_prof = [5, 2, 10, 30, 5, 5, 5, 5, 5, 5]

print('PARALLEL MODE W/O BSEC')
sensor = BME68X(cnst.BME68X_I2C_ADDR_HIGH, 0)
print(sensor.set_heatr_conf(cnst.BME68X_ENABLE,
      temp_prof, dur_prof, cnst.BME68X_PARALLEL_MODE))
print(sensor.get_data())

print('\n\nPARALLEL MODE WITH BSEC')
sensor = BME68X(cnst.BME68X_I2C_ADDR_HIGH, 0)
sensor.set_sample_rate(bsec.BSEC_SAMPLE_RATE_HIGH_PERFORMANCE)


def get_data(sensor):
    data = {}
    try:
        data = sensor.get_digital_nose_data()
    except Exception as e:
        print(e)
        sleep(0.1)
        return None
    if data == {} or data == None:
        sleep(0.1)
        return None
    else:
        return data


bsec_data = get_data(sensor)
while bsec_data == None:
    bsec_data = get_data(sensor)
print(bsec_data)
