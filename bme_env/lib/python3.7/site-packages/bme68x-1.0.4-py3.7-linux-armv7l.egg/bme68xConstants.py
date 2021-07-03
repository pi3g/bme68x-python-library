# BME68X I2C addresses
BME68X_I2C_ADDR_LOW = 0x76
BME68X_I2C_ADDR_HIGH = 0x77

# BME68X chip identifier
BME68X_CHIP_ID = 0x61

# BME68X return code definitions
# SUCCESS
BME68X_OK = 0

# ERRORS
# Null pointer passed
BME68X_E_NULL_PTR = -1
# Communication failure
BME68X_E_COM_FAIL = -2
# Sensor not found
BME68X_E_DEV_NOT_FOUND = -3
# Incorrect length parameter
BME68X_E_INVALID_LENGTH = -4
# Self test fail error
BME68X_E_SELF_TEST = -5

# WARNINGS
# Define a valid operation mode
BME68X_W_DEFINE_OP_MODE = 1
# No new data was found
BME68X_W_NO_NEW_DATA = 2
# Define the shared heating duration
BME68X_W_DEFINE_SHD_HEATR_DUR = 3

# Chip ID register
BME68X_REG_CHIP_ID = 0xd0
# Variant ID register
BME68X_REG_VARIANT_ID = 0xF0

# Enable
BME68X_ENABLE = 0x01
# Disable
BME68X_DISABLE = 0x00

# Low gas variant
BME68X_VARIANT_GAS_LOW = 0x00
# High gas variant
BME68X_VARIANT_GAS_HIGH = 0x01

# OVERSAMPLING SETTING MACROS
# Switch off measurement
BME68X_OS_NONE = 0
# Perform 1 measurement
BME68X_OS_1X = 1
# Perform 2 measurements
BME68X_OS_2X = 2
# Perform 4 measurements
BME68X_OS_4X = 3
# Perform 8 measurements
BME68X_OS_8X = 4
# Perform 16 measurements
BME68X_OS_16X = 5

# IRR FILTER SETTINGS
# Switch off the filter
BME68X_FILTER_OFF = 0
# Filter coefficient of 2
BME68X_FILTER_SIZE_1 = 1
# Filter coefficient of 4
BME68X_FILTER_SIZE = 2
# Filter coefficient of 8
BME68X_FILTER_SIZE_7 = 3
# Filter coefficient of 16
BME68X_FILTER_SIZE_15 = 4
# Filter coefficient of 32
BME68X_FILTER_SIZE_31 = 5
# Filter coefficient of 64
BME68X_FILTER_SIZE_63 = 6
# Filter coefficient of 128
BME68X_FILTER_SIZE_127 = 7

# ODR / STANDBY TIME MACROS
# Standby time of 0.59ms
BME68X_ODR_0_59_MS = 0
# Standby time of 62.5ms
BME68X_ODR_62_5_MS = 1
# Standby time of 125ms
BME68X_ODR_125_MS = 2
# Standby time of 250ms
BME68X_ODR_250_MS = 3
# Standby time of 500ms
BME68X_ODR_500_MS = 4
# Standby time of 1s
BME68X_ODR_1000_MS = 5
# Standby time of 10ms
BME68X_ODR_10_MS = 6
# Standby time of 20ms
BME68X_ODR_20_MS = 7
# No standby time
BME68X_ODR_NONE = 8

# OPERATING MODE _MACROS
# Sleep mode
BME68X_SLEEP_MODE = 0
# Forced mode
BME68X_FORCED_MODE = 1
# Parallel mode
BME68X_PARALLEL_MODE = 2
# Sequential mode
BME68X_SEQUENTIAL_MODE = 3

# GAS MEASUREMENT MACROS
# Disable gas measurement
BME68X_DISABLE_GAS_MEAS = 0x00
# Enable gas measurement low
BME68X_ENABLE_GAS_MEAS_L = 0x01
# Enable gas measurement high
BME68X_ENABLE_GAS_MEAS_H = 0x02

# HEATER CONTROL MACROS
# Enable heater
BME68X_ENABLE_HEATER = 0x00
# Disable heater
BME68X_DISABLE_HEATER = 0x01

# MEASUREMENT VALUE RANGE MACROS
# Temperature min degC
BME68X_MIN_TEMPERATURE = 0
# Temperature max degC
BME68X_MAX_TEMPERATURE = 60
# Pressure min Pa
BME68X_MIN_PRESSURE = 90000
# Pressure max Pa
BME68X_MAX_PRESSURE = 110000
# Humidity min %rH
BME68X_HUMIDITY_MIN = 20
# Humidity max %rh
BME68X_HUMIDITY_MAX = 80
