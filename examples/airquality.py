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
    elif(100 <= iaq and iaq < 300):
        GRN.off()
        YLW.on()
        RED.off()
    else:
        GRN.off()
        YLW.off()
        RED.on()


bme = BME68X(cnst.BME68X_I2C_ADDR_HIGH, bsec.BSEC_ENABLE)

while(True):
    data = bme.get_bsec_data()
    print(data)
    iaq = data["iaq"]
    control_LED(iaq)
    print(iaq)
