# This example records measurements and outputs them in a
# '.bmerawdata' file which can be imported into AI Studio from BOSCH
# Requires Python module 'pynput' to be installed
# Run 'pip3 install pynput'
# Press Ctrl+c or Esc to stop measurement loop and output '.bmerawdata' file
# Press l or any number from 0 to 9 to change specimen label accordingly

from bme68x import BME68X
import json
import time
from pynput import keyboard
from datetime import datetime
import gpiozero as gpio

GPIO_LED = 20
GPIO_BTN = 16


def writeFile():

    dateCreated = '2021-03-31T15:16:37.274Z'
    appVersion = '1.5.4'
    boardType = 'board_8'  # option: 'board_8'
    # options: 'burn_in' 'heater_profile_exploration' 'duty_cycle_profile_exploration'
    boardMode = 'burn_in'
    boardLayout = 'grouped'  # options: 'grouped' 'shuffled' 'random'

    configHeader = {
        'dateCreated': dateCreated,
        'appVersion': appVersion,
        'boardType': boardType,
        'boardMode': boardMode,
        'boardLayout': boardLayout
    }

    heaterProfiles = [{
        "id": "heater_354",
        "timeBase": 140,
        "temperatureTimeVectors": [
            [
              320,
                5
            ],
            [
                100,
                2
            ],
            [
                100,
                10
            ],
            [
                100,
                30
            ],
            [
                200,
                5
            ],
            [
                200,
                5
            ],
            [
                200,
                5
            ],
            [
                320,
                5
            ],
            [
                320,
                5
            ],
            [
                320,
                5
            ]
        ]
    }]  # TODO
    dutyCycleProfiles = [{
        "id": "duty_5_10",
        "numberScanningCycles": 5,
        "numberSleepingCycles": 10
    }]  # TODO
    sensorConfigurations = [{
        "sensorIndex": 0,
        "heaterProfile": "heater_354",
        "dutyCycleProfile": "duty_5_10"
    }]  # TODO

    configBody = {
        'heaterProfiles': heaterProfiles,
        'dutyCycleProfiles': dutyCycleProfiles,
        'sensorConfigurations': sensorConfigurations
    }

    counterPowerOnOff = 1
    seedPowerOnOff = '63lia0cvtdr8jz18'  # TODO
    counterFileLimit = 0
    dateCreated = int(round(time.time()))
    dateCreated_ISO = datetime.now().isoformat()  # TODO
    firmwareVersion = '1.2.0'
    boardID = '84CCA811FA3C'
    hashFunction = 'HMAC-SHA256'
    hmacValue = 'E11A8ECCF92D3D07174D1B2D9F2D86C657B4D91EC6ABFB55EF9C97D4BD2B4882'  # TODO

    rawDataHeader = {
        'counterPowerOnOff': counterPowerOnOff,
        'seedPowerOnOff': seedPowerOnOff,
        'counterFileLimit': counterFileLimit,
        'dateCreated': dateCreated,
        'dateCreated_ISO': dateCreated_ISO,
        'firmwareVersion': firmwareVersion,
        'boardID': boardID,
        'hashFunction': hashFunction,
        'hmacValue': hmacValue
    }

    sensor_index = dict(name='Sensor Index', unit='',
                        format='integer', key='sensor_index')
    sensor_id = dict(name='Sensor ID', unit='',
                     format='integer', key='sensor_id')
    timestamp_since_poweron = dict(
        name='Time Since PowerOn', unit='Milliseconds', format='integer', key='timestamp_since_poweron')
    real_time_clock = dict(name='Real time clock', unit='Unix Timestamp: seconds since Jan 01 1970. (UTC); 0 = missing',
                           format='integer', key='real_time_clock')
    temparature = dict(name='Temperature', unit='DegreesCelcius',
                       format='float', key='temperature')
    pressure = dict(name='Pressure', unit='Hectopscals',
                    format='float', key='pressure')
    relative_humidity = dict(
        name='Relative Humidity', unit='Percent', format='float', key='relative_humidity')
    resistance_gassensor = dict(name='Resistance Gassensor',
                                unit='Ohms', format='float', key='resistance_gassensor')
    heater_profile_step_index = dict(
        name='Heater Profle Step Index', unit='', format='integer', key='heater_profile_step_index')
    scanning_enabled = dict(name='Scanning enabled',
                            unit='', format='integer', key='scanning_enabled')
    label_tag = dict(name='Label Tag', unit='',
                     format='integer', key='label_tag')
    error_code = dict(name='Error Code', unit='',
                      format='integer', key='error_code')

    dataColumns = [
        sensor_index,
        sensor_id,
        timestamp_since_poweron,
        real_time_clock,
        temparature,
        pressure,
        relative_humidity,
        resistance_gassensor,
        heater_profile_step_index,
        scanning_enabled,
        label_tag,
        error_code
    ]

    rawDataBody = {
        'dataColumns': dataColumns,
        'dataBlock': dataBlock
    }

    bmerawdata = {
        'configHeader': configHeader,
        'configBody': configBody,
        'rawDataHeader': rawDataHeader,
        'rawDataBody': rawDataBody
    }

    date = list(dateCreated_ISO)
    fileDate = f"{date[0]}{date[1]}{date[2]}{date[3]}_{date[5]}{date[6]}_{date[8]}{date[9]}_{date[11]}{date[12]}_{date[14]}{date[15]}"
    fileName = f"{fileDate}_Board_{boardID}_PowerOnOff_{counterPowerOnOff}_{seedPowerOnOff}_File_{counterFileLimit}.bmerawdata"

    dataString = json.dumps(bmerawdata, indent=3)

    with open(fileName, 'w') as f:
        f.write(dataString)
        f.close()


def on_press(kbkey):
    if (kbkey == keyboard.Key.esc):
        global terminate
        terminate = True
        return False
    try:
        k = kbkey.char
    except:
        k = kbkey.name
    if k in ['l']:
        print("NEW SPECIMEN")
        global lt
        lt = not lt
    if k in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']:
        print(f"NEW SPECIMEN {int(k)}")
        global lbl_tag
        lbl_tag = int(k)


def on_press_btn():
    print("NEW SPECIMEN")
    global lt
    lt = not lt


# MAIN
start_time = int(round(time.time()*1000))
print('RECORDING DATA')
print('Press Ctrl+c or Esc to terminate')
print('Press l to mark new specimen or 0 to 9 to mark new specimen with specific number')

led = gpio.LED(GPIO_LED)
btn = gpio.Button(GPIO_BTN)
btn.when_pressed = on_press_btn

bme = BME68X(0x77, 1)
temp_profile = [320, 100, 100, 100, 200, 200, 200, 320, 320, 320]
dur_profile = [5, 2, 10, 30, 5, 5, 5, 5, 5, 5]
bme.set_heatr_conf(1, temp_profile, dur_profile, 2)
dataBlock = []

terminate = False
lt = False
lbl_tag = 0
listener = keyboard.Listener(on_press=on_press)
listener.start()

print('\nSTARTING MEASUREMENT\n')

while(True):
    try:
        led.on()
        sens_indx = 0
        sens_id = 1643613715
        ts = int(round(time.time()*1000))
        unix_ts = int(round(ts/1000))
        time_s_p = ts - start_time
        bme68xdata = bme.get_data()
        scnn_en = 1

        if(lt):
            lbl_tag += 1
            lt = not lt

        err_code = 0

        for ind in range(0, len(temp_profile)):
            data = [sens_indx, sens_id, time_s_p, unix_ts, bme68xdata[ind]['temperature'], bme68xdata[ind]['pressure'],
                    bme68xdata[ind]['humidity'], bme68xdata[ind]['gas_resistance'], bme68xdata[ind]['gas_index'], scnn_en, lbl_tag, err_code]
            print(data)
            dataBlock.append(data)
        print()

        if(terminate):
            break

        led.off()
        time.sleep(3)
    except:
        writeFile()
        pass
        break

writeFile()
