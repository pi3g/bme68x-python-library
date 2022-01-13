I bmerawdata
---------------

This tool allows you to record and save data with your [BME688 Breakout Board](https://buyzero.de/products/luftqualitatssensor-bosch-bme688-breakout-board?_pos=2&_sid=985505100&_ss=r), so that it can be imported by [Boschs AI Studio](https://www.bosch-sensortec.com/software-tools/software/bme688-software/).
To use the smell detection feature of the BME688 you need to train an algorithm in AI Studio.
The algorithm requires lots of training and testing data, which you can record using this tool.
**[Read this guide](https://picockpit.com/raspberry-pi/teach-bme688-how-to-smell/)** for detailed instructions.

How to use the script

- Make sure your sensor is connected and working properly (check out the parallel_mode.py example in the examples folder to confirm)
- Download the bmerawdata folder
- Open a new project in AI Studio (at least version 1.6.0 since we need to export configurations for BSEC 2.0.6.1)
- (Optional) If you want to use your own configuration
  - Press Configure BME Board and save the Configuration file
  - Its advisable to use Heater Profile Exploration Mode and the RDC-1-0-Continuous Duty Cycle 
  - Copy the newly created **.bmeconfig** file into the bmerawdata folder
  - Edit the bmerawdata.py and in line 174 change the value of ai_conf to the name of your config file
- Execute the bmerawdata.py script to start recording data
- Press **Ctrl+C** to terminate the script
- The captured data will be saved as a **.bmerawdata** file that starts with the timestamp of creation
- Import that **.bmerawdata** file into AI Studio and label your specimen (You can record multiple specimens in one session, but you have to manually label them in AI Studio)
