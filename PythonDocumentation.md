This module provides the BME68X sensor as Python class
import the module via <import bme68x> or import the class via <from bme68x import BME68X>

To use the BME68X API constants, import bme68xConstants.py via <import bme68xConstants.py as cnst>
To use the BSEC constants, import bsecConstants.py via <import bsecConstants as bsec>


Constructor
BME68X(i2c_addr (int), use_bsec (int))
- Create and initialize new BME68X sensor object, initialize I2C interface
- Args:
    - i2c_addr (int): I2C address of the BME68X sensor (execute <i2cdetect -y 1> in terminal to look up the i2c address, either 0x76 or 0x77)
    - use_bsec (int): Enable/disable use of BSEC library (enable = 1 disable = 0, see bsecConstants.py)
- Returns:
    - 0 for success
    - non 0 for failure


Methods
close_i2c()
- Close the I2C port
- Returns:
  - 0 for success
  - < 0 for error

open_i2c(i2c_addr (int))
- Open the I2C port and establish connection to i2c_addr
- Args:
  - i2c_addr (int): I2C address
- Returns:
  - 0 for success
  - throws Error otherwise

get_variant()
- Get the Sensor variant
- Returns
  - "BME680" in case of BME680
  - "BME688" in case of BME688
  - "UNKOWN" otherwise

set_temp_offset(t_offs (int))
- Set the temperature offset to be subtracted from 25 degC
- Args:
    - t_offs (int): offset in degC
- Returns:
    - 0 for success
    - throws Error otherwise

set_conf(os_hum (int), os_press (int), os_temp (int), filter (int), odr (int))
- Configure the oversampling rates for humidity pressure and temperature and set the filter and odr settings of the BME68X sensor
- Args:
    - os_hum (int): Oversampling rate for humidity
    - os_ press (int): Oversampling rate for pressure
    - os_temp (int): Oversampling rate for temperature
    - filter (int): Filter setting
    - odr (int): ODR (Output Data Rate) setting
- Returns:
    - 0 for success
    - non 0 for failure
- Higher oversampling rates ,filter settings and odr settings reduce (data) noise but decrease the performance

set_heatr_conf(enable (int), temperature_profile ([int]), duration_profile ([int]), operation_mode (int))
- Configure the heater
- Args:
    - enable (int): Enable/disable gas measurement (BME68X_ENABLE to enable, BME68X_DISABLE to disable)
    - temperature_profile ([int]): Up to 10 temperature values in degC for each heater profile step (only 1 in FORCED MODE)
    - duration_profile ([int]): Up to 10 duration values in milliseconds for each heater profile step (only 1 in FORCED MODE)
    - operation_mode (int): Set operation mode (either BME68X_FORCED_MODE, BME68X_PARALLEL_MODE or BME68X_SEQUENTIAL_MODE)
- Returns:
    - 0 for success
    - non 0 for failure

get_data()
- Measure data under current configuration
- Returns:
    - list of physical measurement values, without use of BSEC (in FORCED MODE)
    - array of lists of physical measurement values, without use of BSEC (in PARALLEL MODE or SEQUENTIAL MODE)
    - sample_nr (int)
    - timestamp (int) in milliseconds
    - temperature (double) in degC
    - pressure (double) in hPa (hecto Pascal)
    - humidity (double) in %rH (relative humidity)
    - gas_resistance (double) in kOhm (kilo Ohm)
    - status (int)


get_bsec_data()
- Measure data under current configuration and process it using BSEC
- Returns:
    - list of physical and virtual measurement values including IAQ, uses BSEC2.0 (in FORCED MODE)
    - array of lists of physical and virtual measurement values including IAQ, uses BSEC2.0 (in PARALLEL MODE or SEQUENTIAL MODE)
    - sample_nr (int)
    - timestamp (int) in nanoseconds
    - iaq (double) Index ranges from 0-500
    - iaq_accuracy (int) calibration status ranges from 0 (calibrating) to 3 (fully calibrated)
    - temperature (double) in degC, temperature processed by BSEC
    - raw_temperature (double) in degC, raw sensor output
    - raw_pressure (double) in hPa (hecto Pascal), raw sensor output
    - humidity (double) in %rH (relative humidity), humidity processed by BSEC
    - raw_humidity (double) in %rH (relative humidity), raw sensor output
    - raw_gas (double) in kOhm (kilo Ohm), raw sensor output
    - static_iaq (double) Index ranges from 0-500, unscaled
    - static_iaq_accuracy (int) calibration status ranges from 0 (calibrating) to 3 (fully calibrated)
    - co2_equivalent (double), CO2 equivalent estimate in ppm (parts per million)
    - co2_accuracy (int) calibration status ranges from 0 (calibrating) to 3 (fully calibrated)
    - breath_voc_equivalent (double) breath VOC concentration estimate in ppm (parts per million)
    - breath_voc_accuracy (int) calibration status ranges from 0 (calibrating) to 3 (fully calibrated)
    - comp_gas_value (double) in log_10(Ohm), temperature and humidity compensated gas resistance value, try 10^(comp_gas_value) to recieve value in Ohm
    - comp_gas_accuracy (int) calibration status ranges from 0 (calibrating) to 3 (fully calibrated)
    - gas_percentage (double) 
    - gas_percentage_accuracy (int) calibration status ranges from 0 (calibrating) to 3 (fully calibrated)
- Requires use_bsec to be set to BSEC_ENABLE
- co2_equivalent and breath_voc_equivalent are derived using static_iaq and lab results
- They assume the sensor is used indoors and that humans are the source of the air pollution