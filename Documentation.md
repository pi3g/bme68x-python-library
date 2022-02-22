bme68x-python-library
=====================

This module supports the BME68X sensor as a Python class.<br>Import the module via `<import bme68x>` or import the class via `<from bme68x import BME68X>`
- To use the BME68X API constants, import bme68xConstants.py via `<import bme68xConstants.py as cnst>` 
- To use the BSEC constants, import bsecConstants.py via `<import bsecConstants as bsec>`

The BME68x module is a Python Extension implemented as "C" program containing functions that are invoked from within Python. 
To understand more on this type of cPython integration visit [Real Python]
(https://realpython.com/build-python-c-extension-module/)
and  [Python Documentation]
(https://docs.python.org/3/c-api/index.html)


<H1>Constructor</H1>
BME68X(i2c_addr (int), use_bsec (int))<br>

- Create and initialize new BME68X sensor object, initialize I2C interface
- Args:
    - i2c_addr (int): I2C address of the BME68X sensor (execute <i2cdetect -y 1> in terminal to look up the i2c address, either 0x76 or 0x77)
    - debug_mode (int): Enable/disable debug messages (enable = 1, disable = 0)
- Returns:
    - BME68X instance

<H1>Methods</H1>

init_bme68x()
- initialises the sensor
_The constructor does this and more - see above. KMcA_

print_dur_prof()
- Walks the duration profile array and prints it (to the tty)

enable_debug_mode()
- Enable debug [ sets debug_mode=1 ]
- Args: None

disable_debug_mode()
- Disable debug [sets debug_mode=0 ]
- Args: None

get_sensor_id()
- Returns the unique sensor ID or null
- Args: None

get_chip_id()
- Returns the device chip ID or null
- Args: None

close_i2c()
- Close the I2C port
- Args: None
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

_I2C Port/Bus 0 and Port/Bus 1 are defined in BME68x and Bus 1 is the default (/dev/i2c-1). I2C Bus 3 and 4 can be enabled on a PI but would require BME68x code enhancement. Note: I2C Bus 0 and 2 are reserved for internal use on the PI. - KMcA_

get_variant()
- Get the Sensor variant
- Args: None
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
    - Throws Error otherwise
<p>The assumption is that the heater will distort (raise) the temperature of the device, so the offset is subtracted.
The developer will need to calibrate this based on the planned mode of operation (ULP, LP, HP). 
If you have no baseline to compare to your sensor, search online for your local METAR station for an accurate data set covering temperature, pressure, humidity, etc.</p>

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

_Filter settings pass pressure and temperature data through the IIR filter, which removes short-term fluctuations in pressure.
The algorithm is `((<last val>) * (C -1) + <Curr Val>)/C `
The coefficient C is one of 0, 1, 3, 7, 15, 31, 63, 127._ 

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
 
set_sample_rate(float)
- Sets the sampling rate for all virtual sensors
- Args: One of the following
  - BSEC_SAMPLE_RATE_ULP <p> ULP has a drain of \< 0.1 mA and an update rate of 300 sec </p>
  - BSEC_SAMPLE_RATE_LP <p> LP is for interactive displays and has a drain of \<1 mA and an update rate of 3 sec </p>
  - BSEC_SAMPLE_RATE_HIGH_PERFORMANCE <p> is for continuous sampling </p>
  - See bsecConstants.py
- Returns:
  - 0 for sucess
<p>The BME68x library deals with the state model of the Bosch sensor, but it needs to be understood.
The steps are Configure & Start  -> Sleep until measurement is finished -> Read Data from Sensor -> Run BSEC Steps -> Sleep until sensor control signals ready. 
The sample rate, and heater configuration impact the duration of the sleep whilst waiting for data. </p>

- Bosch quote typical durations for:
  - "Sleep until measurement is finished" duration
  - 0.190 seconds for LP mode
  - 2.0 seconds for ULP mode
  - "Sleep until sensor control signals" duration 
  - 2.8 seconds for LP mode
  - 298 seconds for ULP mode
<p>
There is no point in hammering get_data()/bsec_get_data() in a tight loop. 
However, also  do not leave it too long. For example: when running the sensor
in low-power mode the intended sample period is ~3 s (.19 + 2.8), and the difference between two consecutive
measurements must not exeed 150% of 3 s which is 4.5 s (or expect 0 values to be returned).
For ULP Mode the intended sample period is 300 sec, so do not exceed 450sec.</p>

<p>The v1.3 examples typically have a get data loop testing for "null" return, seeing lots of "nulls" means the sensor was not ready.
Adjust your sleep() accordingly. </p>

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
- **__Requires version of bme68x-python-library that contains BSEC**_
- co2_equivalent and breath_voc_equivalent are derived using static_iaq and lab results
- They assume the sensor is used indoors and that humans are the source of the air pollution.

subscribe_gas_estimates()
- Args
- Returns
Subscribe argument must be int number of gas estimates (0 - 4)

subscribe_ai_classes()
- Args
- Returns
Subscribe to all gas estimates

update_bsec_subscription()
- Args: For each virtual sensor, items must be tuples (sensor_id, sample_rate)
- Returns: Result

By default all virtual sensors are enabled in high power mode.
```C
requested_virtual_sensors[0].sample_rate = BSEC_SAMPLE_RATE_HIGH_PERFORMANCE;
```
This function supports an update to low power mode (BSEC_SAMPLE_RATE_LP) for each virtual sensor. 
Note: The BOSCH integration guide has a table on page 8, Section 1.2.4 Supported Virtual Sensor Output Signals, providing details for each virtual sensor. 
The sample rates ULP, LP, HP,HTR and SEL are (1/300, 1/3, 1,1/heater step duration, 1/scan duration) Hz respectively.


get_digital_nose_data()
- Args: none
- Returns: 

The BSEC libray supports up to 4 separate smells loaded into the sensor.
These are listed in the virtual sensor table on page 8 of the Integration guide
as BSEC_OUTPUT_GAS_ESTIMATE_1 to 4


get_bsec_version()
- Args: none
- Returns: string (major, minor, major_bugfix, minor_bugfix)

get_bsec_conf()
- Args : none
- Returns: gets the BSEC conf and returns it a list of integers.

See get_bsec_state() and set_bsec_state() below, as the approach is similar but the array of Integer values is larger at 2277 (197 for state). 
 
set_bsec_conf( config ([int]))
- Args: config - list of integers
  - Assuming the config is in a file it will need to be read and converted from bytes to int. See the get_data.py example in the pi3g github repository BME688CheeseMeatDetector
- Returns: 0 success Null on failure
- Once processed into an array of integers this is passed into the function.

<p> The config is typically produced by the Bosch AI studio, with up to 4 Classes defined.
Each Class is based on training from sample data of a smell (meat, coffee, air, cheese, etc) and exported as a single config file.
Default configs are provided in the BOSCH BSEC library 2.0.6.1, and the Pi3g code is set to import an appropriate config on initialisation. </p>

get_bsec_state()
- Args: none
- Returns: Array in Int values (197)

After burn-in of the sensor for approx 24 hours, the sensor state can be saved ready to be re-used. 
In python the state is held as an array of Int values. Typically, writing the state array to a file will write it as a string.
For example this fragment of code writes 197 integers plus the [] wrapper.

```python
state_file = open(state_path, 'w')
state_file.write(str(bme.get_bsec_state())) 
```
 
The viewed file looks like this:

```
[1, 6, 0, 2, 189, 1, 0, 0, 0, 0, 0, 0, 173, 0, 0,.......... , 0, 0, 105, 44, 0, 0]
```
set_bsec_state()
- Args: Array in Int values (197)
- Returns: 0 on sucess and Null on error

If the state file was written to file as above for get_bsec_state() then it will need to be read and processed from a string to an array of Int.
In the code snippet below the file is read and sliced [1:-1] to remove the [] wrapper and then split on a comma delimiter to give a list of strings. 
The last step iterates the list and converts String to Integer. 
```
conf_file = open(conf_path, 'r')
conf_str =  conf_file.read()[1:-1]
conf_list = conf_str.split(",")
conf_int = [int(x) for x in conf_list]
```
Why save the state for later? After sensor burn in the accuracy will likely reach state 3 (High Accuracy), and restoring this state data helps the sensor get back to that accuracy quickly.
From a cold start of the sensor, setting a known good saved state will not be an instant step to accuracy 3, but it will speed up the journey from state 0 - 3.  

enable_gas_estimates()
- Args: none
- Returns: 0 on sucess and Null on error

Enable all 4 gas estimates

disable_gas_estimate()
- Args: none
- Returns: 0 on sucess and Null on error

Disable all 4 gas estimates
