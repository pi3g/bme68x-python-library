from setuptools import setup, Extension, find_packages
from pathlib import Path

BSEC = True

if BSEC:
    ext_comp_args = ['-D BSEC']
    libs = ['pthread', 'm', 'rt', 'algobsec']
    lib_dirs = ['/usr/local/lib',
                'BSEC_2.0.6.1_Generic_Release_04302021/algo/normal_version/bin/RaspberryPi/PiThree_ArmV6']
else:
    ext_comp_args = []
    libs = ['pthread', 'm', 'rt']
    lib_dirs = ['/usr/local/lib']

LIBDIR = Path(__file__).parent

README = (LIBDIR / "README.md").read_text()

bme68x = Extension('bme68x',
                   extra_compile_args=ext_comp_args,
                   include_dirs=['/usr/local/include'],
                   libraries=libs,
                   library_dirs=lib_dirs,
                   depends=['BME68x-Sensor-API/bme68x.h', 'BME68x-Sensor-API/bme68x.c',
                            'BME68x-Sensor-API/bme68x_defs.h', 'internal_functions.h', 'internal_functions.c'],
                   sources=['bme68xmodule.c', 'BME68x-Sensor-API/bme68x.h', 'BME68x-Sensor-API/bme68x.c', 'BME68x-Sensor-API/bme68x_defs.h', 'internal_functions.h', 'internal_functions.c'])

setup(name='bme68x',
      version='1.0.4',
      description='Python interface for BME68X sensor and BSEC',
      long_description=README,
      long_description_content_type='text/markdown',
      url='https://github.com/pi3g/bme68x-python-library',
      author='Nathan',
      author_email='nathan@pi3g.com',
      license='MIT',
      classifiers=[
           'Development Status :: 3 - Alpha',
           'Intended Audience :: Developers',
           'License :: OSI Approved :: MIT License',
          'Natural Language :: English',
          'Operating System :: POSIX :: Linux',
          'Programming Language :: Python :: Implementation :: CPython',
          'Topic :: Scientific/Engineering :: Atmospheric Science',
      ],
      keywords='bme68x bme680 bme688 BME68X BME680 BME688 bsec BSEC sensor environment temperature pressure humidity air pollution',
      packages=find_packages(),
      py_modules=['bme68xConstants', 'bsecConstants'],
      package_data={
          'bme68x': [
               'BSEC_2.0.6.1_Generic_Release_04302021/config/bsec_sel_iaq_33v_4d/2021_04_29_02_51_bsec_h2s_nonh2s_2_0_6_1 .config',
          ]
      },
      headers=['BME68x-Sensor-API/bme68x.h',
               'BME68x-Sensor-API/bme68x_defs.h', 'internal_functions.h'],
      ext_modules=[bme68x])
