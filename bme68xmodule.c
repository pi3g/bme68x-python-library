#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include "internal_functions.h"

#define I2C_PORT_0 "/dev/i2c-0"
#define I2C_PORT_1 "/dev/i2c-1"

#define BME68X_VALID_DATA UINT8_C(0xB0)
#define BME68X_FLOAT_POINT_COMPENSATION
// #define BSEC

uint64_t time_stamp_interval_us = 0;
uint32_t n_samples = 0;

#ifdef BSEC
bsec_input_t bsec_inputs[BSEC_MAX_PHYSICAL_SENSOR];
uint8_t num_bsec_inputs = 0;
bsec_bme_settings_t sensor_settings;
uint8_t bsec_state[BSEC_MAX_STATE_BLOB_SIZE];
uint8_t work_buffer[BSEC_MAX_WORKBUFFER_SIZE];
uint32_t bsec_state_len = 0;
bsec_library_return_t bsec_status = BSEC_OK;
const char *bsec_conf_path = "BSEC_2.0.6.1_Generic_Release_04302021/config/bsec_sel_iaq_33v_4d/2021_04_29_02_51_bsec_h2s_nonh2s_2_0_6_1 .config";
FILE *bsec_conf;
#endif

static PyObject *
    bmeError;

typedef struct
{
    PyObject_HEAD
        uint8_t linux_device;
    int8_t temp_offset;
    struct bme68x_dev bme;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;
    struct bme68x_data *data;
    uint32_t del_period;
    uint32_t time_ms;
    uint8_t n_fields;
    int8_t rslt;
    uint8_t op_mode;
    uint16_t sample_count;
    uint8_t use_bsec;
} BMEObject;

static void
bme68x_dealloc(BMEObject *self)
{
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
bme68x_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    BMEObject *self;
    self = (BMEObject *)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        self->linux_device = 0;
        self->temp_offset = 0;

        self->bme.amb_temp = 0;
        self->bme.calib.par_gh1 = 0;
        self->bme.calib.par_gh2 = 0;
        self->bme.calib.par_gh3 = 0;
        self->bme.calib.par_h1 = 0;
        self->bme.calib.par_h2 = 0;
        self->bme.calib.par_h3 = 0;
        self->bme.calib.par_h4 = 0;
        self->bme.calib.par_h5 = 0;
        self->bme.calib.par_h6 = 0;
        self->bme.calib.par_h7 = 0;
        self->bme.calib.par_p10 = 0;
        self->bme.calib.par_p1 = 0;
        self->bme.calib.par_p2 = 0;
        self->bme.calib.par_p3 = 0;
        self->bme.calib.par_p4 = 0;
        self->bme.calib.par_p5 = 0;
        self->bme.calib.par_p6 = 0;
        self->bme.calib.par_p7 = 0;
        self->bme.calib.par_p8 = 0;
        self->bme.calib.par_p9 = 0;
        self->bme.calib.par_t1 = 0;
        self->bme.calib.par_t2 = 0;
        self->bme.calib.par_t3 = 0;
        self->bme.calib.range_sw_err = 0;
        self->bme.calib.res_heat_range = 0;
        self->bme.calib.res_heat_val = 0;
        self->bme.calib.t_fine = 0.0;
        self->bme.chip_id = 0;
        self->bme.delay_us = pi3g_delay_us;
        self->bme.info_msg = 0;
        self->bme.intf = BME68X_I2C_INTF;
        self->bme.intf_ptr = &(self->linux_device);
        self->bme.intf_rslt = 0;
        self->bme.mem_page = 0;
        self->bme.read = pi3g_read;
        self->bme.variant_id = 0;
        self->bme.write = pi3g_write;

        self->conf.os_hum = 0;
        self->conf.os_temp = 0;
        self->conf.os_pres = 0;
        self->conf.filter = 0;
        self->conf.odr = 0;

        self->heatr_conf.enable = 0;
        self->heatr_conf.heatr_dur = 0;
        uint16_t heatr_dur_prof[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        self->heatr_conf.heatr_dur_prof = heatr_dur_prof;
        self->heatr_conf.heatr_temp = 0;
        uint16_t heatr_temp_prof[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        self->heatr_conf.heatr_temp_prof = heatr_temp_prof;
        self->heatr_conf.profile_len = 0;
        self->heatr_conf.shared_heatr_dur = 0;

        self->data = malloc(sizeof(struct bme68x_data) * 3);

        self->del_period = 0;
        self->time_ms = 0;
        self->n_fields = 0;
        self->rslt = BME68X_OK;
        self->op_mode = BME68X_SLEEP_MODE;
        self->sample_count = 0;
        self->use_bsec = 0;
    }
    return (PyObject *)self;
}

static int
bme68x_init_type(BMEObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist1[] = {"i2c_addr", NULL};
    static char *kwlist2[] = {"i2c_addr", "use_bsec", NULL};

    self->linux_device = open(I2C_PORT_1, O_RDWR);
    Py_ssize_t size = PyTuple_Size(args);
    uint8_t i2c_addr;

    switch (size)
    {
    case 1:
        self->use_bsec = 0;
        PyArg_ParseTupleAndKeywords(args, kwds, "|b", kwlist1, &i2c_addr);
        if (ioctl(*((uint8_t *)self->bme.intf_ptr), I2C_SLAVE, i2c_addr) < 0)
        {
            perror("wrong I2C adress");
            PyErr_SetString(bmeError, "I2C device not found (check adress)");
            return -1;
        }
        break;
    case 2:
        PyArg_ParseTupleAndKeywords(args, kwds, "|bb", kwlist2, &i2c_addr, &(self->use_bsec));
        if (ioctl(*((uint8_t *)self->bme.intf_ptr), I2C_SLAVE, i2c_addr) < 0)
        {
            perror("wrong I2C adress");
            PyErr_SetString(bmeError, "I2C device not found (check adress)");
            return -1;
        }
        break;
    default:
        self->use_bsec = 0;
        if (ioctl(*((uint8_t *)self->bme.intf_ptr), I2C_SLAVE, BME68X_I2C_ADDR_LOW) < 0)
        {
            if (ioctl(*((uint8_t *)self->bme.intf_ptr), I2C_SLAVE, BME68X_I2C_ADDR_HIGH) < 0)
            {
                PyErr_SetString(bmeError, "Failed to open I2C device");
                return -1;
            }
        }
    }
    // Initialize BME68X sensor
    self->bme.intf = BME68X_I2C_INTF;
    self->bme.amb_temp = 25;
    self->bme.read = pi3g_read;
    self->bme.write = pi3g_write;
    self->bme.delay_us = pi3g_delay_us;

    self->rslt = BME68X_OK;
    self->rslt = bme68x_init(&(self->bme));
    if (self->rslt == BME68X_OK)
    {
        printf("INITIALIZED BME68X\n");
        if (self->bme.variant_id == BME68X_VARIANT_GAS_LOW)
        {
            printf("VARIANT = BME680\n");
        }
        else
        {
            printf("VARIANT = BME688\n");
        }
    }
    else
    {
        perror("initialize BME68X");
        PyErr_SetString(bmeError, "Could not initialize BME68X");
        return self->rslt;
    }
#ifdef BSEC
    if (self->use_bsec)
    {
        bsec_library_return_t bsec_rslt = BSEC_OK;
        bsec_rslt = bsec_init();
        if (bsec_rslt != BSEC_OK)
        {
            perror("bsec_init");
            PyErr_SetString(bmeError, "Failed to initialize BSEC");
            return self->rslt;
        }
        bsec_version_t version;
        bsec_get_version(&version);
        printf("INITIALIZED BSEC\nBSEC VERSION: %d.%d.%d.%d\n", version.major, version.minor, version.major_bugfix, version.minor_bugfix);

        uint8_t serialized_settings[BSEC_MAX_PROPERTY_BLOB_SIZE];
        uint32_t n_serialized_settings_max = BSEC_MAX_PROPERTY_BLOB_SIZE;
        uint8_t work_buffer[BSEC_MAX_PROPERTY_BLOB_SIZE];
        uint32_t n_work_buffer = BSEC_MAX_PROPERTY_BLOB_SIZE;

        bsec_conf = fopen(bsec_conf_path, "rb");
        if (!bsec_conf)
        {
            perror("open BSEC config");
            PyErr_SetString(bmeError, "Could not find BSEC config file");
            printf("USING DEFAULT BSEC CONFIG\n");
        }
        else
        {
            uint8_t conf_len;
            conf_len = fread(&serialized_settings[0], sizeof(unsigned char), BSEC_MAX_PROPERTY_BLOB_SIZE, bsec_conf);
            if (conf_len == 0)
            {
                perror("read BSEC config");
                PyErr_SetString(bmeError, "Could not read from BSEC config file");
            }

            // Apply the configuration
            bsec_rslt = bsec_set_configuration(serialized_settings, n_serialized_settings_max, work_buffer, n_work_buffer);
            if (bsec_rslt != BSEC_OK)
            {
                perror("set BSEC config");
                PyErr_SetString(bmeError, "Could not set BSEC config");
            }
            else
            {
                printf("SET BSEC CONFIG (SEL IAQ 3.3v 4d)\n");
            }
        }

        bsec_rslt = bsec_set_sample_rate(BSEC_SAMPLE_RATE_LP, self->bme.variant_id);
        if (bsec_rslt != BSEC_OK)
        {
            printf("bsec_rslt = %d\n", bsec_rslt);
            perror("bsec_set_sample_rate");
            return bsec_rslt;
        }
    }
#endif
    self->op_mode = BME68X_FORCED_MODE;
    self->rslt = pi3g_set_conf(BME68X_OS_16X, BME68X_OS_1X, BME68X_OS_2X, BME68X_FILTER_OFF, BME68X_ODR_NONE, &(self->conf), &(self->bme));
    if (self->rslt != BME68X_OK)
    {
        PyErr_SetString(bmeError, "Failed to set configure sensor");
        return self->rslt;
    }
    self->rslt = pi3g_set_heater_conf_fm(BME68X_ENABLE, 300, 100, &(self->heatr_conf), &(self->bme));
    if (self->rslt != BME68X_OK)
    {
        PyErr_SetString(bmeError, "Failed to configure heater");
        return self->rslt;
    }

    return self->rslt;
}

static PyMemberDef bme68x_members[] = {
    {"linux_device_handle", T_UBYTE, offsetof(BMEObject, linux_device), 0, "Linux address of the sensor device"},
    {"temp_offset", T_BYTE, offsetof(BMEObject, temp_offset), 0, "temperature offset to be subtracted from 25 degC"},
    {"bme68x", T_OBJECT_EX, offsetof(BMEObject, bme), 0, "BME68X sensor struct"},
    {"conf", T_OBJECT_EX, offsetof(BMEObject, conf), 0, "BME68X config struct"},
    {"heatr_conf", T_OBJECT_EX, offsetof(BMEObject, heatr_conf), 0, "BME68X heater config struct"},
    {"data", T_OBJECT_EX, offsetof(BMEObject, data), 0, "BME68X measurement data struct"},
    {"del_period", T_ULONG, offsetof(BMEObject, del_period), 0, "delay period"},
    {"time_ms", T_ULONG, offsetof(BMEObject, time_ms), 0, "millisecond precision time stamp"},
    {"n_fields", T_UBYTE, offsetof(BMEObject, n_fields), 0, "number of data fields"},
    {"rslt", T_BYTE, offsetof(BMEObject, rslt), 0, "function execution result"},
    {"op_mode", T_UBYTE, offsetof(BMEObject, op_mode), 0, "BME68X operation mode"},
    {"sample_count", T_UINT, offsetof(BMEObject, sample_count), 0, "number of data samples"},
    {"use_bsec", T_UBYTE, offsetof(BMEObject, use_bsec), 0, "enable/disable BSEC"},
};

static PyObject *bme_set_temp_offset(BMEObject *self, PyObject *args)
{
    int t_offs;
    if (!PyArg_ParseTuple(args, "i", &t_offs))
    {
        PyErr_SetString(bmeError, "Invalid arguments in set_temp_offset(double t_offs)");
        return NULL;
    }

    self->temp_offset = t_offs;
    self->bme.amb_temp = 25 - self->temp_offset;
    printf("SET TEMP OFFSET\n");

    return Py_BuildValue("i", 0);
}

static PyObject *bme_close_i2c(BMEObject *self)
{
    return Py_BuildValue("i", close(*((uint8_t *)self->bme.intf_ptr)));
}

static PyObject *bme_open_i2c(BMEObject *self, PyObject *args)
{
    close(*((uint8_t *)self->bme.intf_ptr));
    self->linux_device = open(I2C_PORT_1, O_RDWR);
    self->bme.intf_ptr = &(self->linux_device);
    Py_ssize_t size = PyTuple_Size(args);
    uint8_t i2c_addr;

    if ((uint8_t)size == 1)
    {
        if (!PyArg_ParseTuple(args, "b", &i2c_addr))
        {
            PyErr_SetString(bmeError, "Failed to parse I2C address");
            return NULL;
        }
        else if (ioctl(*((uint8_t *)self->bme.intf_ptr), I2C_SLAVE, i2c_addr) < 0)
        {
            PyErr_SetString(bmeError, "Failed to open I2C address");
            return NULL;
        }
    }
    else
    {
        PyErr_SetString(bmeError, "Argument must be i2c_addr: int");
        close(*((uint8_t *)self->bme.intf_ptr));
        return NULL;
    }

    return Py_BuildValue("i", 0);
}

static PyObject *bme_get_variant(BMEObject *self)
{
    char *variant = "";
    if (self->bme.variant_id == BME68X_VARIANT_GAS_LOW)
    {
        variant = "BME680";
    }
    else if (self->bme.variant_id == BME68X_VARIANT_GAS_HIGH)
    {
        variant = "BME688";
    }
    else
    {
        variant = "UNKNOWN";
    }
    return Py_BuildValue("s", variant);
}

static PyObject *bme_set_conf(BMEObject *self, PyObject *args)
{
    Py_ssize_t size = PyTuple_Size(args);
    uint8_t c[size];

    switch (size)
    {
    case 1:
        PyArg_ParseTuple(args, "b", &c[0]);
        self->rslt = pi3g_set_conf(c[0], self->conf.os_pres, self->conf.os_temp, self->conf.filter, self->conf.odr, &(self->conf), &(self->bme));
        break;
    case 2:
        PyArg_ParseTuple(args, "bb", &c[0], &c[1]);
        self->rslt = pi3g_set_conf(c[0], c[1], self->conf.os_temp, self->conf.filter, self->conf.odr, &(self->conf), &(self->bme));
        break;
    case 3:
        PyArg_ParseTuple(args, "bbb", &c[0], &c[1], &c[2]);
        self->rslt = pi3g_set_conf(c[0], c[1], c[2], self->conf.filter, self->conf.odr, &(self->conf), &(self->bme));
        break;
    case 4:
        PyArg_ParseTuple(args, "bbbb", &c[0], &c[1], &c[2], &c[3]);
        self->rslt = pi3g_set_conf(c[0], c[1], c[2], c[3], self->conf.odr, &(self->conf), &(self->bme));
        break;
    case 5:
        PyArg_ParseTuple(args, "bbbbb", &c[0], &c[1], &c[2], &c[3], &c[4]);
        self->rslt = pi3g_set_conf(c[0], c[1], c[2], c[3], c[4], &(self->conf), &(self->bme));
        break;
    default:
        self->rslt = pi3g_set_conf(BME68X_OS_16X, BME68X_OS_1X, BME68X_OS_2X, BME68X_FILTER_OFF, BME68X_ODR_NONE, &(self->conf), &(self->bme));
        break;
    }

    return Py_BuildValue("b", self->rslt);
}

static PyObject *bme_set_heatr_conf(BMEObject *self, PyObject *args)
{
    uint8_t enable;
    PyObject *temp_prof_obj;
    PyObject *dur_prof_obj;
    if (!PyArg_ParseTuple(args, "bOOb", &enable, &temp_prof_obj, &dur_prof_obj, &(self->op_mode)))
    {
        PyErr_SetString(bmeError, "Function takes 4 arguments: enable, temp_prof, dur_prof, op_mode");
        return NULL;
    }
    if (self->op_mode == BME68X_FORCED_MODE)
    {
#ifdef BSEC
        if (self->use_bsec == 1)
        {
            bsec_sensor_configuration_t requested_virtual_sensors[1];
            bsec_sensor_configuration_t required_sensor_settings[BSEC_MAX_PHYSICAL_SENSOR];
            uint8_t n_required_sensor_settings = BSEC_MAX_PHYSICAL_SENSOR;

            requested_virtual_sensors[0].sensor_id = BSEC_OUTPUT_RAW_GAS_INDEX;
            requested_virtual_sensors[0].sample_rate = BSEC_SAMPLE_RATE_DISABLED;

            bsec_update_subscription(requested_virtual_sensors, 1, required_sensor_settings, &n_required_sensor_settings);
        }
#endif

        uint16_t heatr_temp, heatr_dur;
        PyArg_Parse(temp_prof_obj, "H", &heatr_temp);
        PyArg_Parse(dur_prof_obj, "H", &heatr_dur);
        if (heatr_temp == 0 || heatr_dur == 0)
        {
            PyErr_SetString(bmeError, "heatr_temp and heatr_dur need to be of type uint16_t (unsigned short)");
            return NULL;
        }
        self->rslt = pi3g_set_heater_conf_fm(enable, heatr_temp, heatr_dur, &(self->heatr_conf), &(self->bme));
    }
    else
    {
        if (self->bme.variant_id == BME68X_VARIANT_GAS_LOW)
        {
            printf("ONLY FORCED MODE IS AVAILABLE FOR BME680 SENSOR\n");
            return Py_BuildValue("i", -1);
        }

        if (!PyList_Check(temp_prof_obj) || !PyList_Check(dur_prof_obj))
        {
            PyErr_SetString(bmeError, "temp_prof and dur_prof must be of type list\n");
            return NULL;
        }

        int temp_size = PyList_Size(temp_prof_obj);
        int dur_size = PyList_Size(dur_prof_obj);
        if (temp_size != dur_size)
        {
            PyErr_SetString(bmeError, "temp_prof and dur_prof must have the same size");
            return NULL;
        }
        if (temp_size > 10)
        {
            PyErr_SetString(bmeError, "length of heater profile must not exceed 10");
            return NULL;
        }

        uint16_t temp_prof[temp_size], dur_prof[temp_size];
        PyObject *val;

        for (int i = 0; i < temp_size; i++)
        {
            val = PyList_GetItem(temp_prof_obj, i);
            temp_prof[i] = (uint16_t)PyLong_AsLong(val);
            val = PyList_GetItem(dur_prof_obj, i);
            dur_prof[i] = (uint16_t)PyLong_AsLong(val);
        }

        for (int i = 0; i < temp_size; i++)
        {
            printf("%d ", temp_prof[i]);
        }
        printf("\n");
        for (int i = 0; i < temp_size; i++)
        {
            printf("%d ", dur_prof[i]);
        }
        printf("\n");

#ifdef BSEC
        if (self->use_bsec == 1)
        {
            bsec_sensor_configuration_t requested_virtual_sensors[1];
            bsec_sensor_configuration_t required_sensor_settings[BSEC_MAX_PHYSICAL_SENSOR];
            uint8_t n_required_sensor_settings = BSEC_MAX_PHYSICAL_SENSOR;

            uint8_t PROFILE_DUR = 0;
            for (uint8_t i = 0; i < dur_size; i++)
            {
                PROFILE_DUR += dur_prof[i];
            }

            float_t HTR = 1 / PROFILE_DUR;
            requested_virtual_sensors[0].sensor_id = BSEC_OUTPUT_RAW_GAS_INDEX;
            requested_virtual_sensors[0].sample_rate = HTR; // sample_rate = HTR = 1/heater step duration

            bsec_update_subscription(requested_virtual_sensors, 1, required_sensor_settings, &n_required_sensor_settings);
        }
#endif

        if (self->op_mode == BME68X_PARALLEL_MODE)
        {
            self->rslt = pi3g_set_heater_conf_pm(enable, temp_prof, dur_prof, (uint8_t)temp_size, &(self->conf), &(self->heatr_conf), &(self->bme));
        }
        else if (self->op_mode == BME68X_SEQUENTIAL_MODE)
        {
            self->rslt = pi3g_set_heater_conf_sm(enable, temp_prof, dur_prof, (uint8_t)temp_size, &(self->heatr_conf), &(self->bme));
        }
        else
        {
            perror("set_heatr_conf");
        }
    }

    return Py_BuildValue("i", self->rslt);
}

#ifdef BSEC
static PyObject *bme_get_bsec_data(BMEObject *self)
{
    bsec_library_return_t bsec_rslt;
    int64_t time_stamp = pi3g_timestamp_ns();
    bsec_sensor_control(time_stamp, &sensor_settings);

    uint16_t meas_dur;
    self->rslt = BME68X_OK;

    /* Check if a forced-mode measurement should be triggered now */
    if (sensor_settings.trigger_measurement)
    {
        /* Set sensor configuration */
        self->conf.os_hum = sensor_settings.humidity_oversampling;
        self->conf.os_pres = sensor_settings.pressure_oversampling;
        self->conf.os_temp = sensor_settings.temperature_oversampling;
        self->heatr_conf.enable = sensor_settings.run_gas;
        self->heatr_conf.heatr_temp = sensor_settings.heater_temperature; /* degree Celsius */
        self->heatr_conf.heatr_dur = sensor_settings.heater_duration;     /* milliseconds */

        self->op_mode = sensor_settings.op_mode;
        self->rslt = bme68x_set_conf(&(self->conf), &(self->bme));
        if (self->rslt < 0)
        {
            PyErr_SetString(bmeError, "FAILED TO SET CONFIG");
            return NULL;
        }
        self->rslt = bme68x_set_heatr_conf(sensor_settings.op_mode, &(self->heatr_conf), &(self->bme));
        if (self->rslt < 0)
        {
            PyErr_SetString(bmeError, "FAILED TO SET HEATER CONFIG");
            return NULL;
        }
        /* Select the power mode */
        /* Must be set before writing the sensor configuration */
        self->rslt = bme68x_set_op_mode(self->op_mode, &(self->bme));

        /* Get the total measurement duration so as to sleep or wait till the measurement is complete */
        meas_dur = bme68x_get_meas_dur(self->op_mode, &(self->conf), &(self->bme));

        /* Delay till the measurement is ready. Timestamp resolution in ms */
        usleep((uint32_t)meas_dur);

        /* Call the API to get current operation mode of the sensor */
        self->rslt = bme68x_get_op_mode(&(self->op_mode), &(self->bme));
        /* When the measurement is completed and data is ready for reading, the sensor must be in BME680_SLEEP_MODE.
         * Read operation mode to check whether measurement is completely done and wait until the sensor is no more
         * in BME680_FORCED_MODE. */
        while (self->op_mode != BME68X_SLEEP_MODE)
        {
            /* sleep for 5 ms */
            usleep(5000);
            self->rslt = bme68x_get_op_mode(&(self->op_mode), &(self->bme));
        }
        num_bsec_inputs = 0;
        uint8_t data_length = 0;
        bsec_rslt = bsec_read_data(self->data, &data_length, time_stamp, bsec_inputs, &num_bsec_inputs, sensor_settings.process_data, sensor_settings.op_mode, &(self->bme));
        if (bsec_rslt != BSEC_OK)
        {
            perror("read BSEC data");
            PyErr_SetString(bmeError, "FAILED TO READ BSEC DATA");
            return NULL;
        }
        // bsec_process_data(bsec_inputs, num_bsec_inputs);

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
            if (bsec_status != BSEC_OK)
            {
                perror("bsec_do_steps");
                PyErr_SetString(bmeError, "Failed to process data");
                return NULL;
            }

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

            n_samples++;

            time_stamp_interval_us = (sensor_settings.next_call - pi3g_timestamp_ns()) / 1000;
            if (time_stamp_interval_us > 0)
            {
                usleep((uint32_t)time_stamp_interval_us);
            }
        }

        PyObject *bsec_data = PyDict_New();
        PyDict_SetItemString(bsec_data, "sample_nr", Py_BuildValue("i", n_samples));
        PyDict_SetItemString(bsec_data, "timestamp", Py_BuildValue("L", timestamp));
        PyDict_SetItemString(bsec_data, "iaq", Py_BuildValue("d", iaq));
        PyDict_SetItemString(bsec_data, "iaq_accuracy", Py_BuildValue("i", iaq_accuracy));
        PyDict_SetItemString(bsec_data, "temperature", Py_BuildValue("d", temp * 100));
        PyDict_SetItemString(bsec_data, "raw_temperature", Py_BuildValue("d", raw_temp * 100));
        PyDict_SetItemString(bsec_data, "raw_pressure", Py_BuildValue("d", raw_pressure / 100));
        PyDict_SetItemString(bsec_data, "humidity", Py_BuildValue("d", humidity * 1000));
        PyDict_SetItemString(bsec_data, "raw_humidity", Py_BuildValue("d", raw_humidity * 1000));
        PyDict_SetItemString(bsec_data, "raw_gas", Py_BuildValue("d", raw_gas / 1000));
        PyDict_SetItemString(bsec_data, "static_iaq", Py_BuildValue("d", static_iaq));
        PyDict_SetItemString(bsec_data, "static_iaq_accuracy", Py_BuildValue("i", static_iaq_accuracy));
        PyDict_SetItemString(bsec_data, "co2_equivalent", Py_BuildValue("d", co2_equivalent));
        PyDict_SetItemString(bsec_data, "co2_accuracy", Py_BuildValue("i", co2_accuracy));
        PyDict_SetItemString(bsec_data, "breath_voc_equivalent", Py_BuildValue("d", breath_voc_equivalent));
        PyDict_SetItemString(bsec_data, "breath_voc_accuracy", Py_BuildValue("i", breath_voc_accuracy));
        PyDict_SetItemString(bsec_data, "comp_gas_value", Py_BuildValue("d", comp_gas_value));
        PyDict_SetItemString(bsec_data, "comp_gas_accuracy", Py_BuildValue("i", comp_gas_accuracy));
        PyDict_SetItemString(bsec_data, "gas_percentage", Py_BuildValue("d", gas_percentage));
        PyDict_SetItemString(bsec_data, "gas_percentage_accuracy", Py_BuildValue("i", gas_percentage_accuracy));

        return bsec_data;
    }
    PyErr_SetString(bmeError, "Failed to read measurement Data");
    return NULL;
}
#endif

static PyObject *bme_get_data(BMEObject *self)
{
    self->rslt = bme68x_set_op_mode(self->op_mode, &(self->bme));

    if (self->rslt != BME68X_OK)
    {
        perror("set_op_mode");
    }

    if (self->op_mode == BME68X_FORCED_MODE)
    {
        self->del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &(self->conf), &(self->bme)) + (self->heatr_conf.heatr_dur * 1000);
        self->bme.delay_us(self->del_period, self->bme.intf_ptr);
        self->time_ms = pi3g_timestamp_ms();

        self->rslt = bme68x_get_data(self->op_mode, self->data, &(self->n_fields), &(self->bme));
        if (self->rslt == BME68X_OK && self->n_fields > 0)
        {
            self->sample_count++;
            self->bme.amb_temp = self->data[0].temperature - self->temp_offset;

            PyObject *pydata = PyDict_New();
            PyDict_SetItemString(pydata, "sample_nr", Py_BuildValue("i", self->sample_count));
            PyDict_SetItemString(pydata, "timestamp", Py_BuildValue("i", self->time_ms));
            PyDict_SetItemString(pydata, "temperature", Py_BuildValue("d", self->data[0].temperature));
            PyDict_SetItemString(pydata, "pressure", Py_BuildValue("d", self->data[0].pressure / 100));
            PyDict_SetItemString(pydata, "humidity", Py_BuildValue("d", self->data[0].humidity));
            PyDict_SetItemString(pydata, "gas_resistance", Py_BuildValue("d", self->data[0].gas_resistance / 1000));
            PyDict_SetItemString(pydata, "status", Py_BuildValue("i", self->data[0].status));
            return pydata;
        }
    }
    else
    {
        PyObject *pydata = PyList_New(self->heatr_conf.profile_len);
        PyObject *field;
        uint8_t counter = 0;
        while (counter < self->heatr_conf.profile_len)
        {
            if (self->op_mode == BME68X_PARALLEL_MODE)
            {
                self->del_period = bme68x_get_meas_dur(BME68X_PARALLEL_MODE, &(self->conf), &(self->bme)) + (self->heatr_conf.shared_heatr_dur * 1000);
            }
            else if (self->op_mode == BME68X_SEQUENTIAL_MODE)
            {
                self->del_period = bme68x_get_meas_dur(BME68X_SEQUENTIAL_MODE, &(self->conf), &(self->bme)) + (self->heatr_conf.heatr_dur_prof[0] * 1000);
            }
            else
            {
                PyErr_SetString(bmeError, "Failed to receive data");
                return NULL;
            }
            self->bme.delay_us(self->del_period, self->bme.intf_ptr);
            self->time_ms = pi3g_timestamp_ms();

            self->rslt = bme68x_get_data(self->op_mode, self->data, &(self->n_fields), &(self->bme));
            if (self->rslt < 0)
            {
                perror("bme68x_get_data");
            }

            /* Check if rslt == BME68X_OK, report or handle if otherwise */
            for (uint8_t i = 0; i < self->n_fields; i++)
            {
                if (self->data[i].status == BME68X_VALID_DATA)
                {
                    field = PyDict_New();
                    PyDict_SetItemString(field, "sample_nr", Py_BuildValue("i", self->sample_count));
                    PyDict_SetItemString(field, "timestamp", Py_BuildValue("i", self->time_ms));
                    PyDict_SetItemString(field, "temperature", Py_BuildValue("d", self->data[i].temperature));
                    PyDict_SetItemString(field, "pressure", Py_BuildValue("d", self->data[i].pressure / 100));
                    PyDict_SetItemString(field, "humidity", Py_BuildValue("d", self->data[i].humidity));
                    PyDict_SetItemString(field, "gas_resistance", Py_BuildValue("d", self->data[i].gas_resistance / 1000));
                    PyDict_SetItemString(field, "gas_index", Py_BuildValue("i", self->data[i].gas_index));
                    PyDict_SetItemString(field, "status", Py_BuildValue("i", self->data[i].status));
                    PyList_SetItem(pydata, self->data[i].gas_index, field);
                    self->sample_count++;
                    counter++;
                }
            }
        }
        self->bme.amb_temp = self->data[0].temperature - self->temp_offset;
        return pydata;
    }
    return Py_BuildValue("s", "Failed to get data");
}

static PyMethodDef bme68x_methods[] = {
    {"set_temp_offset", (PyCFunction)bme_set_temp_offset, METH_VARARGS, "Set temperature offset"},
    {"close_i2c", (PyCFunction)bme_close_i2c, METH_NOARGS, "Close the I2C bus"},
    {"open_i2c", (PyCFunction)bme_open_i2c, METH_VARARGS, "Open the I2C bus and connect to I2C address"},
    {"get_variant", (PyCFunction)bme_get_variant, METH_NOARGS, "Return string representing variant (BME680 or BME688)"},
    {"set_conf", (PyCFunction)bme_set_conf, METH_VARARGS, "Configure the BME68X sensor"},
    {"set_heatr_conf", (PyCFunction)bme_set_heatr_conf, METH_VARARGS, "Configure the BME68X heater"},
    {"get_data", (PyCFunction)bme_get_data, METH_NOARGS, "Measure and read data from the BME68X sensor w/o BSEC"},
#ifdef BSEC
    {"get_bsec_data", (PyCFunction)bme_get_bsec_data, METH_NOARGS, "Measure and read data from the BME68x sensor with BSEC"},
#endif
    {NULL, NULL, 0, NULL} // Sentinel
};

static PyTypeObject BMEType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "bme68x.BME68X",
    .tp_doc = "BME68X sensor object",
    .tp_basicsize = sizeof(BMEObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = bme68x_new,
    .tp_init = (initproc)bme68x_init_type,
    .tp_dealloc = (destructor)bme68x_dealloc,
    .tp_members = bme68x_members,
    .tp_methods = bme68x_methods,
};

static PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "bme68x",
    .m_doc = "Example module that creates an extension type.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_bme68x(void)
{
    PyObject *m;
    if (PyType_Ready(&BMEType) < 0)
        return NULL;

    m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&BMEType);
    if (PyModule_AddObject(m, "BME68X", (PyObject *)&BMEType) < 0)
    {
        Py_DECREF(&BMEType);
        Py_DECREF(m);
        return NULL;
    }

    PyModule_AddIntConstant(m, "BME68X_I2C_ADDR_LOW", 0x76);
    PyModule_AddIntConstant(m, "BME68X_I2C_ADDR_HIGH", 0x77);
    PyModule_AddIntConstant(m, "BME68X_CHIP_ID", 0x61);
    PyModule_AddIntConstant(m, "BME68X_OK", 0);

    return m;
}
