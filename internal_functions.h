#ifndef INTERNAL_FUNCTIONS_H_
#define INTERNAL_FUNCTIONS_H_

#define _XOPEN_SOURCE 700

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include "BME68x-Sensor-API/bme68x.h"
#include "BME68x-Sensor-API/bme68x_defs.h"

#ifdef BSEC
#include "BSEC_2.0.6.1_Generic_Release_04302021/algo/normal_version/bin/RaspberryPi/PiThree_ArmV6/bsec_interface.h"
#include "BSEC_2.0.6.1_Generic_Release_04302021/algo/normal_version/bin/RaspberryPi/PiThree_ArmV6/bsec_datatypes.h"
#endif

/* CPP guard */
#ifdef __cplusplus
extern "C"
{
#endif

    uint16_t get_max(uint16_t array[], int8_t len);

    void pi3g_delay_us(uint32_t duration_us, void *intf_ptr);

    int8_t pi3g_read(uint8_t regAddr, uint8_t *regData, uint32_t len, void *intf_ptr);

    int8_t pi3g_write(uint8_t regAddr, const uint8_t *regData, uint32_t len, void *intf_ptr);

    int8_t pi3g_set_conf(uint8_t os_hum, uint8_t os_pres, uint8_t os_temp, uint8_t filter, uint8_t odr, struct bme68x_conf *conf, struct bme68x_dev *bme);

    int8_t pi3g_set_heater_conf_fm(uint8_t enable, uint16_t heatr_temp, uint16_t heatr_dur, struct bme68x_heatr_conf *heatr_conf, struct bme68x_dev *bme);

    int8_t pi3g_set_heater_conf_pm(uint8_t enable, uint16_t temp_prof[], uint16_t dur_prof[], uint8_t profile_len, struct bme68x_conf *conf, struct bme68x_heatr_conf *heatr_conf, struct bme68x_dev *bme);

    int8_t pi3g_set_heater_conf_sm(uint8_t enable, uint16_t temp_prof[], uint16_t dur_prof[], uint8_t profile_len, struct bme68x_heatr_conf *heatr_conf, struct bme68x_dev *bme);

    int64_t pi3g_timestamp_ns();

    uint32_t pi3g_timestamp_us();

    uint32_t pi3g_timestamp_ms();

#ifdef BSEC
    bsec_library_return_t bsec_set_sample_rate(float sample_rate, uint8_t variant_id);

    bsec_library_return_t bsec_read_data(struct bme68x_data *data, uint8_t *data_len, int64_t time_stamp, bsec_input_t *inputs, uint8_t *n_bsec_inputs, int32_t bsec_process_data, uint8_t op_mode, struct bme68x_dev *bme);

    bsec_library_return_t bsec_process_data(bsec_input_t *bsec_inputs, uint8_t num_bsec_inputs);
#endif

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* INTERNAL_FUNCTIONS_H_ */
