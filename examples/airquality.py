# This example uses BSEC to indicate the IAQ (Air Quality)
# Requires Python module 'gpiozero' to be installed
# Run 'pip3 install gpiozero'
# Wire up a green LED to gpio pin 17
# Wire up a yellow LED to gpio pin 27
# Wire up a red LED to gpio pin 22
# If IAQ is below 100 the red LED will shine
# If IAQ is between 100 and 300 the yellow LED will shine
# If IAQ is over 300 the red LED will shine

from bme68x import BME68X
import bme68xConstants as cnst
import bsecConstants as bsec
import gpiozero as gpio
from time import sleep

GRN = gpio.LED(17)
YLW = gpio.LED(27)
RED = gpio.LED(22)


def control_LED(iaq):
    if(iaq < 100):
        GRN.on()
        YLW.off()
        RED.off()
        return 'GREEN'
    elif(100 <= iaq and iaq < 300):
        GRN.off()
        YLW.on()
        RED.off()
        return 'YELLOW'
    else:
        GRN.off()
        YLW.off()
        RED.on()
        return 'RED'


bme = BME68X(cnst.BME68X_I2C_ADDR_HIGH, 1)
bme.set_sample_rate(bsec.BSEC_SAMPLE_RATE_LP)


def get_data(sensor):
    data = {}
    try:
        data = sensor.get_bsec_data()
    except Exception as e:
        print(e)
        return None
    if data == None or data == {}:
        sleep(0.1)
        return None
    else:
        sleep(3)
        return data


colors = {
    'RED': '\033[91m',
    'YELLOW': '\033[93m',
    'BLUE': '\033[94m',
    'GREEN': '\033[92m',
}

while(True):
    bsec_data = get_data(bme)
    while bsec_data == None:
        bsec_data = get_data(bme)
    led_color = control_LED(bsec_data["iaq"])
    print(colors[led_color] +
          f'IAQ {bsec_data["iaq"]}' + ' ' + list(colors.values())[bsec_data["iaq_accuracy"]] + f'ACCURACY {bsec_data["iaq_accuracy"]}')
