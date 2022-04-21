import sys
import os
from bme68x import BME68X
import bme68xConstants as BME68XCONST
import bsecConstants as BSECCONST
import json
from time import time, sleep
from datetime import datetime
from threading import Thread, Lock


def io_task(self):
    while True:
        try:
            inpt = input()
            self.increase_label_tag()
            print(f'INCREASED LABEL TAG TO {self.label_tag}')
        except KeyboardInterrupt:
            self.ai_data['rawDataBody']['dataBlock'] = self.data
            dateCreated = int(round(time()))
            dateCreated_ISO = datetime.now().isoformat()
            rDH = self.ai_data['rawDataHeader']
            print(rDH)
            self.ai_data['rawDataHeader']['dateCreated'] = dateCreated
            self.ai_data['rawDataHeader']['dateCreated_ISO'] = dateCreated_ISO
            date = list(dateCreated_ISO)
            fileDate = f"{date[0]}{date[1]}{date[2]}{date[3]}_{date[5]}{date[6]}_{date[8]}{date[9]}_{date[11]}{date[12]}_{date[14]}{date[15]}"
            fileName = f"{fileDate}_Board_{rDH['boardId']}_PowerOnOff_{rDH['counterPowerOnOff']}_{rDH['seedPowerOnOff']}_File_{rDH['counterFileLimit']}.bmerawdata"

            print('DATA')
            print(self.data)

            bmerawdata = {**self.ai_conf, **self.ai_data}

            dataString = json.dumps(bmerawdata, indent=3)

            with open(fileName, 'w') as f:
                f.write(dataString)
                f.close()

            print(f"\nSAVED DATA AS\n{fileName}")
            try:
                sys.exit(0)
            except SystemExit:
                os._exit(0)


class BME68X_AI(BME68X):
    index: int = 0
    ai_conf: dict = {}
    ai_data: dict = {}
    heatr_profile: dict = {}
    duty_cycle: dict = {}
    data: list = []
    label_tag: int = 0
    sensor_id = 0

    def __init__(self, i2c_addr: int, ai_conf: str, index=0):
        super(BME68X_AI, self).__init__(i2c_addr, 0)
        self.index = index
        self.set_ai_conf(ai_conf)
        with open('default.bmerawdata', 'r') as file:
            raw_data = json.loads(file.read())
            self.ai_data = {
                'rawDataHeader': raw_data['rawDataHeader'], 'rawDataBody': raw_data['rawDataBody']}
        self.label_tag = 0
        self.sensor_id = self.get_sensor_id()

    def set_ai_conf(self, ai_conf: str):
        try:
            with open(ai_conf, 'r') as conf_file:
                conf_str = conf_file.read()
            self.ai_conf = json.loads(conf_str)
            conf_body = self.ai_conf['configBody']

            for hp in conf_body['heaterProfiles']:
                if hp['id'] == conf_body['sensorConfigurations'][self.index]['heaterProfile']:
                    self.heatr_profile = hp

            temp_prof = []
            dur_prof = []
            for vector in self.heatr_profile['temperatureTimeVectors']:
                temp_prof.append(vector[0])
                dur_prof.append(vector[1])

            print('HEATR CONF IN SET AI CONF')
            print(dur_prof)
            self.set_heatr_conf(BME68XCONST.BME68X_ENABLE, temp_prof,
                                dur_prof, BME68XCONST.BME68X_PARALLEL_MODE)

            for dc in conf_body['dutyCycleProfiles']:
                if dc['id'] == conf_body['sensorConfigurations'][0]['dutyCycleProfile']:
                    self.duty_cycle = dc
        except Exception as e:
            print(e)

    def get_heatr_profile_dur(self):
        heatr_profile_dur = 0
        for vector in self.heatr_profile['temperatureTimeVectors']:
            heatr_profile_dur += vector[1]
        return heatr_profile_dur

    def increase_label_tag(self):
        self.label_tag += 1

    def record_data(self):
        self.label_tag = 0

        # io_thread = Thread(target=io_task(self))
        # io_thread.run()
        while(True):
            output_data = []
            try:
                heatr_profile_dur = self.get_heatr_profile_dur()
                print(f'profile dur {heatr_profile_dur * 140 / 1000}')
                for i in range(self.duty_cycle['numberScanningCycles']):
                    raw_data = self.get_data()
                    print(raw_data)
                    for hp_step in raw_data:
                        ts = hp_step['timestamp']
                        rt = hp_step['raw_temperature']
                        rp = hp_step['raw_pressure']
                        rh = hp_step['raw_humidity']
                        rg = hp_step['raw_gas'] * 1000
                        gi = hp_step['gas_index']

                        output_data.append(self.index)  # sensor index
                        output_data.append(self.sensor_id)  # sensor id
                        output_data.append(ts)  # time sp in ms
                        # unix time stamp
                        output_data.append(int(round(time())))
                        output_data.append(rt)  # temp
                        output_data.append(rp)  # pressure hPa
                        output_data.append(rh)  # rel hum
                        output_data.append(rg)  # gas res ohm
                        output_data.append(gi)  # hp step index
                        output_data.append(1)  # scanning enabled
                        output_data.append(self.label_tag)  # label tag
                        output_data.append(0)  # error code
                        print(output_data)
                        self.data.append(output_data)
                        output_data = []
                for i in range(self.duty_cycle['numberSleepingCycles']):
                    print('SLEEPING')
                    sleep(heatr_profile_dur * 140 / 1000)
            except KeyboardInterrupt:
                self.ai_data['rawDataBody']['dataBlock'] = self.data
                dateCreated = int(round(time()))
                dateCreated_ISO = datetime.now().isoformat()
                rDH = self.ai_data['rawDataHeader']
                print(rDH)
                self.ai_data['rawDataHeader']['dateCreated'] = dateCreated
                self.ai_data['rawDataHeader']['dateCreated_ISO'] = dateCreated_ISO
                date = list(dateCreated_ISO)
                fileDate = f"{date[0]}{date[1]}{date[2]}{date[3]}_{date[5]}{date[6]}_{date[8]}{date[9]}_{date[11]}{date[12]}_{date[14]}{date[15]}"
                fileName = f"{fileDate}_Board_{rDH['boardId']}_PowerOnOff_{rDH['counterPowerOnOff']}_{rDH['seedPowerOnOff']}_File_{rDH['counterFileLimit']}.bmerawdata"

                print('DATA')
                print(self.data)

                bmerawdata = {**self.ai_conf, **self.ai_data}

                dataString = json.dumps(bmerawdata, indent=3)

                with open(fileName, 'w') as f:
                    f.write(dataString)
                    f.close()

                print(f"\nSAVED DATA AS\n{fileName}")
                try:
                    sys.exit(0)
                except SystemExit:
                    os._exit(0)


if __name__ == '__main__':
    ai_conf = 'default.bmeconfig'
    bme68x_ai = BME68X_AI(BME68XCONST.BME68X_I2C_ADDR_HIGH, ai_conf)

    print("BSEC VERSION (FROM PYTHON)")
    print(bme68x_ai.get_bsec_version())

    print(bme68x_ai.get_sensor_id())

    print(bme68x_ai.get_data())

    bme68x_ai.record_data()
