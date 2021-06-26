# BME68X and BSEC2.0 for Python by pi3g

This Python extension enables you to read measurement data by the BME68X sensor and the BSEC2.0 library from BOSCH

### How to install the extension without BSEC
```bash
pip3 install bme68x
```

### How to install the extension with BSEC
- unzip this folder to a location of your choice
- download the licensed BSEC2.0 library [from BOSCH](https://www.bosch-sensortec.com/software-tools/software/bme688-software/)
- unzip it into the AirQuality folder, next to this README
- open a new terminal window inside the AirQuality folder
```bash
sudo python3 setup.py install
```
- this will build and install the extension

### How to use the extension
- to import in Python
```python
import bme68x```
or
```python
from bme68x import BME68X
```
- see PythonDocumentation.md for reference
- to test the installation make sure you connected your BME68X sensor via I2C
- run the following code in a Python3 interpreter
```python
from bme68x import BME68X

# Replace I2C_ADDR with the I2C address of your sensor
# Either 0x76 (default for BME680) or 0x77 (default for BME688)
BME68X(, 0).get_data()
```

#### For documentation and examples please refer to our [GitHub](https://github.com/pi3g/bme68x-python-library)<br>
#### For feedback, bug reports or functionality requests send an E-mail to nathan@pi3g.com<br>


![pi3g logo](https://pi3g.com/wp-content/uploads/2015/06/pi3g-150px-only-transparent-e1622110450400.png)<br>
pi3g is an official approved Raspberry Pi reseller, and a Coral Machine Learning Platform (from Google) distribution partner.<br>
We take care of all your Raspberry Pi and Coral related hardware and software development, sourcing, accessory and consulting needs!<br>
Check out [our homepage](https://pi3g.com) and file your personal software or hardware request.<br>
Also visit [our shop](https://buyzero.de).<br>