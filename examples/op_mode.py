# This example tests and demonstrates the operation modes
# FORCED_MODE, PARALLEL_MODE and SEQUENTIAL_MODE
# Without use of BSEC and
# With use of BSEC

from bme68x import BME68X
import bme68xConstants as cnst
import bsecConstants as bsec

temp_prof = [320, 100, 100, 100, 200, 200, 200, 320, 320, 320]
dur_prof = [5, 2, 10, 30, 5, 5, 5, 5, 5, 5]

print('TEST W/O BSEC')
sensor = BME68X(cnst.BME68X_I2C_ADDR_HIGH, bsec.BSEC_DISABLE)
print('\nFORCED MODE TEST')
print(sensor.set_heatr_conf(cnst.BME68X_ENABLE, 320, 100, cnst.BME68X_FORCED_MODE))
print(sensor.get_data())
print('\nPARALLEL MODE TEST')
print(sensor.set_heatr_conf(cnst.BME68X_ENABLE,
      temp_prof, dur_prof, cnst.BME68X_PARALLEL_MODE))
print(sensor.get_data())
print('\nSEQUENTIAL MODE TEST')
print(sensor.set_heatr_conf(cnst.BME68X_ENABLE,
      temp_prof, dur_prof, cnst.BME68X_SEQUENTIAL_MODE))
print(sensor.get_data())

print('\n\nDATA WITH BSEC')
sensor = BME68X(cnst.BME68X_I2C_ADDR_HIGH, bsec.BSEC_ENABLE)
print('\nSEQUENTIAL MODE TEST')
print(sensor.set_heatr_conf(cnst.BME68X_ENABLE,
      temp_prof, dur_prof, cnst.BME68X_SEQUENTIAL_MODE))
print(sensor.get_bsec_data())
print('\nPARALLEL MODE TEST')
print(sensor.set_heatr_conf(cnst.BME68X_ENABLE,
      temp_prof, dur_prof, cnst.BME68X_PARALLEL_MODE))
print(sensor.get_bsec_data())
print('\nFORCED MODE TEST')
print(sensor.set_heatr_conf(cnst.BME68X_ENABLE, 320, 100, cnst.BME68X_FORCED_MODE))
print(sensor.get_bsec_data())
