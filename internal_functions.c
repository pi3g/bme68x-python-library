#define _XOPEN_SOURCE 700
#define TEMP_OFFSET 0.0f
#define BME680_NUM_OUTPUTS 14
#define BME688_NUM_OUTPUTS 14

#include "internal_functions.h"

struct bme68x_data data[3];
uint32_t del_period;
uint32_t time_ms;
uint8_t n_fields;
static int8_t rslt;

uint16_t
get_max(uint16_t array[], int8_t len)
{
    uint16_t max = 0;
    for (int i = 0; i < len; i++)
    {
        if (array[i] > max)
        {
            max = array[i];
        }
    }
    return max;
}

void pi3g_delay_us(uint32_t duration_us, void *intf_ptr)
{
    struct timespec ts;
    ts.tv_sec = duration_us / 1000000;
    ts.tv_nsec = (duration_us % 1000000) * 1000;
    nanosleep(&ts, NULL);
}

int8_t pi3g_read(uint8_t regAddr, uint8_t *regData, uint32_t len, void *intf_ptr)
{
    rslt = BME68X_OK;
    if (write(*((uint8_t *)intf_ptr), &regAddr, 1) != 1)
    {
        perror("pi3g_read register");
        rslt = -1;
    }
    if (read(*((uint8_t *)intf_ptr), regData, len) != len)
    {
        perror("pi3g_read data");
        rslt = -1;
    }

    return rslt;
}

int8_t pi3g_write(uint8_t regAddr, const uint8_t *regData, uint32_t len, void *intf_ptr)
{
    rslt = BME68X_OK;
    uint8_t reg[len + 1];
    reg[0] = regAddr;

    for (int i = 1; i < len + 1; i++)
        reg[i] = regData[i - 1];

    if (write(*((uint8_t *)intf_ptr), reg, len + 1) != len + 1)
    {
        perror("pi3g_write");
        rslt = -1;
    }

    return rslt;
}

int8_t pi3g_set_conf(uint8_t os_hum, uint8_t os_pres, uint8_t os_temp, uint8_t filter, uint8_t odr, struct bme68x_conf *conf, struct bme68x_dev *bme)
{
    int8_t rslt = BME68X_OK;

    rslt = bme68x_get_conf(conf, bme);
    if (rslt < 0)
    {
        perror("bme68x_get_conf");
    }

    conf->os_hum = os_hum;
    conf->os_pres = os_pres;
    conf->os_temp = os_temp;
    conf->filter = filter;
    conf->odr = odr;

    rslt = bme68x_set_conf(conf, bme);
    if (rslt != BME68X_OK)
    {
        perror("bme68x_set_conf");
    }
    printf("SET BME68X CONFIG\n");
    return rslt;
}

int8_t pi3g_set_heater_conf_fm(uint8_t enable, uint16_t heatr_temp, uint16_t heatr_dur, struct bme68x_heatr_conf *heatr_conf, struct bme68x_dev *bme)
{
    int8_t rslt = BME68X_OK;
    heatr_conf->enable = enable;
    heatr_conf->heatr_temp = heatr_temp;
    heatr_conf->heatr_dur = heatr_dur;
    rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, heatr_conf, bme);
    if (rslt != BME68X_OK)
    {
        perror("bme68x_set_heatr_conf");
    }
    printf("SET HEATER CONFIG (FORCED MODE)\n");
    return rslt;
}

int8_t pi3g_set_heater_conf_pm(uint8_t enable, uint16_t temp_prof[], uint16_t dur_prof[], uint8_t profile_len, struct bme68x_conf *conf, struct bme68x_heatr_conf *heatr_conf, struct bme68x_dev *bme)
{
    int8_t rslt = BME68X_OK;
    heatr_conf->enable = enable;
    heatr_conf->heatr_temp_prof = temp_prof;
    heatr_conf->heatr_dur_prof = dur_prof;
    heatr_conf->shared_heatr_dur = 140 - (bme68x_get_meas_dur(BME68X_PARALLEL_MODE, conf, bme) / 1000);
    heatr_conf->profile_len = profile_len;
    rslt = bme68x_set_heatr_conf(BME68X_PARALLEL_MODE, heatr_conf, bme);
    if (rslt != BME68X_OK)
    {
        perror("bme68x_set_heatr_conf");
    }
    rslt = bme68x_set_op_mode(BME68X_PARALLEL_MODE, bme);
    if (rslt != BME68X_OK)
    {
        perror("bme68x_set_op_mode");
    }
    printf("SET HEATER CONFIG (PARALLEL MODE)\n");
    return rslt;
}

int8_t pi3g_set_heater_conf_sm(uint8_t enable, uint16_t temp_prof[], uint16_t dur_prof[], uint8_t profile_len, struct bme68x_heatr_conf *heatr_conf, struct bme68x_dev *bme)
{
    int8_t rslt = BME68X_OK;
    heatr_conf->enable = enable;
    heatr_conf->heatr_temp_prof = temp_prof;
    heatr_conf->heatr_dur_prof = dur_prof;
    heatr_conf->profile_len = profile_len;
    rslt = bme68x_set_heatr_conf(BME68X_SEQUENTIAL_MODE, heatr_conf, bme);
    if (rslt != BME68X_OK)
    {
        perror("bme68x_set_heatr_conf");
    }
    rslt = bme68x_set_op_mode(BME68X_SEQUENTIAL_MODE, bme);
    if (rslt != BME68X_OK)
    {
        perror("bme68x_set_op_mode");
    }
    printf("SET HEATER CONFIG (SEQUENTIAL MODE)\n");
    return rslt;
}

int64_t pi3g_timestamp_ns()
{
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);

    int64_t time_ns = (int64_t)(spec.tv_sec) * (int64_t)1000000000 + (int64_t)(spec.tv_nsec);
    return time_ns;
}

uint32_t pi3g_timestamp_us()
{
    return (uint32_t)(pi3g_timestamp_ns() / 1000);
}

uint32_t pi3g_timestamp_ms()
{
    return (uint32_t)(pi3g_timestamp_us() / 1000);
}

#ifdef BSEC
bsec_library_return_t bsec_set_sample_rate(float sample_rate, uint8_t variant_id)
{
    uint8_t n_requested_virtual_sensors;
    if (variant_id == BME68X_VARIANT_GAS_LOW)
    {
        n_requested_virtual_sensors = BME680_NUM_OUTPUTS;
    }
    else
    {
        n_requested_virtual_sensors = BME688_NUM_OUTPUTS;
    }
    bsec_sensor_configuration_t requested_virtual_sensors[n_requested_virtual_sensors];

    bsec_sensor_configuration_t required_sensor_settings[BSEC_MAX_PHYSICAL_SENSOR];
    uint8_t n_required_sensor_settings = BSEC_MAX_PHYSICAL_SENSOR;

    requested_virtual_sensors[0].sensor_id = BSEC_OUTPUT_IAQ;
    requested_virtual_sensors[0].sample_rate = sample_rate;
    requested_virtual_sensors[1].sensor_id = BSEC_OUTPUT_STATIC_IAQ;
    requested_virtual_sensors[1].sample_rate = sample_rate;
    requested_virtual_sensors[2].sensor_id = BSEC_OUTPUT_CO2_EQUIVALENT;
    requested_virtual_sensors[2].sample_rate = sample_rate;
    requested_virtual_sensors[3].sensor_id = BSEC_OUTPUT_BREATH_VOC_EQUIVALENT;
    requested_virtual_sensors[3].sample_rate = sample_rate;
    requested_virtual_sensors[4].sensor_id = BSEC_OUTPUT_RAW_TEMPERATURE;
    requested_virtual_sensors[4].sample_rate = sample_rate;
    requested_virtual_sensors[5].sensor_id = BSEC_OUTPUT_RAW_PRESSURE;
    requested_virtual_sensors[5].sample_rate = sample_rate;
    requested_virtual_sensors[6].sensor_id = BSEC_OUTPUT_RAW_HUMIDITY;
    requested_virtual_sensors[6].sample_rate = sample_rate;
    requested_virtual_sensors[7].sensor_id = BSEC_OUTPUT_RAW_GAS;
    requested_virtual_sensors[7].sample_rate = sample_rate;
    requested_virtual_sensors[8].sensor_id = BSEC_OUTPUT_STABILIZATION_STATUS;
    requested_virtual_sensors[8].sample_rate = sample_rate;
    requested_virtual_sensors[9].sensor_id = BSEC_OUTPUT_RUN_IN_STATUS;
    requested_virtual_sensors[9].sample_rate = sample_rate;
    requested_virtual_sensors[10].sensor_id = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE;
    requested_virtual_sensors[10].sample_rate = sample_rate;
    requested_virtual_sensors[11].sensor_id = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY;
    requested_virtual_sensors[11].sample_rate = sample_rate;
    requested_virtual_sensors[12].sensor_id = BSEC_OUTPUT_COMPENSATED_GAS;
    requested_virtual_sensors[12].sample_rate = sample_rate;
    requested_virtual_sensors[13].sensor_id = BSEC_OUTPUT_GAS_PERCENTAGE;
    requested_virtual_sensors[13].sample_rate = sample_rate;
    // if (variant_id == BME68X_VARIANT_GAS_HIGH)
    // {
    //     printf("n_requested_virtual_sensors = %d\n", n_requested_virtual_sensors);
    //     requested_virtual_sensors[14].sensor_id = BSEC_OUTPUT_GAS_ESTIMATE_1;
    //     requested_virtual_sensors[14].sample_rate = sample_rate;
    //     requested_virtual_sensors[15].sensor_id = BSEC_OUTPUT_GAS_ESTIMATE_2;
    //     requested_virtual_sensors[15].sample_rate = sample_rate;
    //     requested_virtual_sensors[16].sensor_id = BSEC_OUTPUT_GAS_ESTIMATE_3;
    //     requested_virtual_sensors[16].sample_rate = sample_rate;
    //     requested_virtual_sensors[17].sensor_id = BSEC_OUTPUT_GAS_ESTIMATE_4;
    //     requested_virtual_sensors[17].sample_rate = sample_rate;
    //     requested_virtual_sensors[18].sensor_id = BSEC_OUTPUT_RAW_GAS_INDEX;
    //     requested_virtual_sensors[18].sample_rate = sample_rate;
    // }

    return bsec_update_subscription(requested_virtual_sensors, n_requested_virtual_sensors, required_sensor_settings, &n_required_sensor_settings);
}

bsec_library_return_t bsec_read_data(struct bme68x_data *data, uint8_t *data_len, int64_t time_stamp, bsec_input_t *inputs, uint8_t *n_bsec_inputs, int32_t bsec_process_data, uint8_t op_mode, struct bme68x_dev *bme)
{
    if (bsec_process_data)
    {
        bsec_library_return_t bsec_rslt = bme68x_get_data(op_mode, data, data_len, bme);
        for (int i = 0; i < *data_len; i++)
        {
            if (data[i].status & BME68X_NEW_DATA_MSK)
            {
                /* Pressure to be processed by BSEC */
                if (bsec_process_data & BSEC_PROCESS_PRESSURE)
                {
                    /* Place presssure sample into input struct */
                    inputs[*n_bsec_inputs].sensor_id = BSEC_INPUT_PRESSURE;
                    inputs[*n_bsec_inputs].signal = data[i].pressure;
                    inputs[*n_bsec_inputs].time_stamp = time_stamp;
                    (*n_bsec_inputs)++;
                }
                /* Temperature to be processed by BSEC */
                if (bsec_process_data & BSEC_PROCESS_TEMPERATURE)
                {
                    /* Place temperature sample into input struct */
                    inputs[*n_bsec_inputs].sensor_id = BSEC_INPUT_TEMPERATURE;
#ifdef BME68X_FLOAT_POINT_COMPENSATION
                    inputs[*n_bsec_inputs].signal = data[i].temperature;
#else
                    inputs[*n_bsec_inputs].signal = data[i].temperature / 100.0f;
#endif
                    inputs[*n_bsec_inputs].time_stamp = time_stamp;
                    (*n_bsec_inputs)++;

                    /* Also add optional heatsource input which will be subtracted from the temperature reading to 
                     * compensate for device-specific self-heating (supported in BSEC IAQ solution)*/
                    inputs[*n_bsec_inputs].sensor_id = BSEC_INPUT_HEATSOURCE;
                    inputs[*n_bsec_inputs].signal = TEMP_OFFSET;
                    inputs[*n_bsec_inputs].time_stamp = time_stamp;
                    (*n_bsec_inputs)++;
                }
                /* Humidity to be processed by BSEC */
                if (bsec_process_data & BSEC_PROCESS_HUMIDITY)
                {
                    /* Place humidity sample into input struct */
                    inputs[*n_bsec_inputs].sensor_id = BSEC_INPUT_HUMIDITY;
#ifdef BME68X_FLOAT_POINT_COMPENSATION
                    inputs[*n_bsec_inputs].signal = data[i].humidity;
#else
                    inputs[*n_bsec_inputs].signal = data[i].humidity / 1000.0f;
#endif
                    inputs[*n_bsec_inputs].time_stamp = time_stamp;
                    (*n_bsec_inputs)++;
                }
                /* Gas to be processed by BSEC */
                if (bsec_process_data & BSEC_PROCESS_GAS)
                {
                    /* Check whether gas_valid flag is set */
                    if (data[i].status & BME68X_GASM_VALID_MSK)
                    {
                        /* Place sample into input struct */
                        inputs[*n_bsec_inputs].sensor_id = BSEC_INPUT_GASRESISTOR;
                        inputs[*n_bsec_inputs].signal = data[i].gas_resistance;
                        inputs[*n_bsec_inputs].time_stamp = time_stamp;
                        (*n_bsec_inputs)++;
                    }
                }
                /* Profile part */
                if (op_mode == BME68X_PARALLEL_MODE || op_mode == BME68X_SEQUENTIAL_MODE)
                {
                    inputs[*n_bsec_inputs].sensor_id = BSEC_INPUT_PROFILE_PART;
                    inputs[*n_bsec_inputs].signal = data[i].gas_index;
                    inputs[*n_bsec_inputs].time_stamp = time_stamp;
                    (*n_bsec_inputs)++;
                }
            }
        }
    }
    return BSEC_OK;
}

bsec_library_return_t bsec_process_data(bsec_input_t *bsec_inputs, uint8_t num_bsec_inputs)
{
    /* Output buffer set to the maximum virtual sensor outputs supported */
    bsec_output_t bsec_outputs[BSEC_NUMBER_OUTPUTS];
    uint8_t num_bsec_outputs = 0;
    uint8_t index = 0;

    bsec_library_return_t bsec_status = BSEC_OK;

    int64_t timestamp = 0;
    float iaq = 0.0f;
    uint8_t iaq_accuracy = 0;
    float temp = 0.0f;
    float raw_temp = 0.0f;
    float raw_pressure = 0.0f;
    float humidity = 0.0f;
    float raw_humidity = 0.0f;
    float raw_gas = 0.0f;
    float static_iaq = 0.0f;
    uint8_t static_iaq_accuracy = 0;
    float co2_equivalent = 0.0f;
    uint8_t co2_accuracy = 0;
    float breath_voc_equivalent = 0.0f;
    uint8_t breath_voc_accuracy = 0;
    float comp_gas_value = 0.0f;
    uint8_t comp_gas_accuracy = 0;
    float gas_percentage = 0.0f;
    uint8_t gas_percentage_accuracy = 0;

    /* Check if something should be processed by BSEC */
    if (num_bsec_inputs > 0)
    {
        /* Set number of outputs to the size of the allocated buffer */
        /* BSEC_NUMBER_OUTPUTS to be defined */
        num_bsec_outputs = BSEC_NUMBER_OUTPUTS;

        /* Perform processing of the data by BSEC 
           Note:
           * The number of outputs you get depends on what you asked for during bsec_update_subscription(). This is
             handled under bme680_bsec_update_subscription() function in this example file.
           * The number of actual outputs that are returned is written to num_bsec_outputs. */
        bsec_status = bsec_do_steps(bsec_inputs, num_bsec_inputs, bsec_outputs, &num_bsec_outputs);

        /* Iterate through the outputs and extract the relevant ones. */
        for (index = 0; index < num_bsec_outputs; index++)
        {
            switch (bsec_outputs[index].sensor_id)
            {
            case BSEC_OUTPUT_IAQ:
                iaq = bsec_outputs[index].signal;
                iaq_accuracy = bsec_outputs[index].accuracy;
                break;
            case BSEC_OUTPUT_STATIC_IAQ:
                static_iaq = bsec_outputs[index].signal;
                static_iaq_accuracy = bsec_outputs[index].accuracy;
                break;
            case BSEC_OUTPUT_CO2_EQUIVALENT:
                co2_equivalent = bsec_outputs[index].signal;
                co2_accuracy = bsec_outputs[index].accuracy;
                break;
            case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
                breath_voc_equivalent = bsec_outputs[index].signal;
                breath_voc_accuracy = bsec_outputs[index].accuracy;
                break;
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
                temp = bsec_outputs[index].signal;
                break;
            case BSEC_OUTPUT_RAW_PRESSURE:
                raw_pressure = bsec_outputs[index].signal;
                break;
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
                humidity = bsec_outputs[index].signal;
                break;
            case BSEC_OUTPUT_RAW_GAS:
                raw_gas = bsec_outputs[index].signal;
                break;
            case BSEC_OUTPUT_RAW_TEMPERATURE:
                raw_temp = bsec_outputs[index].signal;
                break;
            case BSEC_OUTPUT_RAW_HUMIDITY:
                raw_humidity = bsec_outputs[index].signal;
                break;
            case BSEC_OUTPUT_COMPENSATED_GAS:
                comp_gas_value = bsec_outputs[index].signal;
                comp_gas_accuracy = bsec_outputs[index].accuracy;
                break;
            case BSEC_OUTPUT_GAS_PERCENTAGE:
                gas_percentage = bsec_outputs[index].signal;
                gas_percentage_accuracy = bsec_outputs[index].accuracy;
                break;
            default:
                continue;
            }

            /* Assume that all the returned timestamps are the same */
            timestamp = bsec_outputs[index].time_stamp;
        }
    }
    return bsec_status;
}
#endif
