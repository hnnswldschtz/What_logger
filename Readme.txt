 __        ___   _    _  _____    ____ ___  _   _ _     ____
 \ \      / / | | |  / \|_   _|  / ___/ _ \| | | | |   |  _ \
  \ \ /\ / /| |_| | / _ \ | |   | |  | | | | | | | |   | | | |
   \ V  V / |  _  |/ ___ \| |   | |__| |_| | |_| | |___| |_| |
  __\_/\_/__|_|_|_/_/__ \_\_|___ \____\___/_\___/|_____|____/
 |  _ \ / _ \/ ___/ ___|_ _| __ )| | \ \ / /  / ___|/ _ \
 | |_) | | | \___ \___ \| ||  _ \| |  \ V /  | |  _| | | |
 |  __/| |_| |___) |__) | || |_) | |___| |   | |_| | |_| |
 |_|    \___/|____/____/___|____/|_____|_|    \____|\___/
 \ \      / /  _ \ / _ \| \ | |/ ___|__ \
  \ \ /\ / /| |_) | | | |  \| | |  _  / /
   \ V  V / |  _ <| |_| | |\  | |_| ||_|
    \_/\_/  |_| \_\\___/|_| \_|\____|(_)



A Data logger for environmental brightness - What could possibly go wrong?

Readme

Arduino dependencies:

- Arduino IDE and the Arduino AVR Boards package for the Uno
- Wire and SPI (included with the Arduino AVR Boards package)
- SD - by Arduino, SparkFun
- RTClib - by Adafruit
- BH1750 - by Christopher Laws

Download the Arduino IDE from https://www.arduino.cc/en/software/.
In the IDE, use Tools > Board > Boards Manager to install Arduino AVR Boards.
Open Library Manager, search for SD, RTClib and BH1750, and install each
library listed above. Accept any dependencies requested by Library Manager.

Connection:
Sensor | Arduino  
- GND -> GND
- SDA -> SDA
- SCL -> SCL
- VCC -> 5V


################################

Plotting logged data:


Python dependencies:

   - Python 3: download from https://www.python.org/downloads/
   - Matplotlib: install with pip as shown below

From the project directory, create and activate a virtual environment:

    macOS / Linux:
    python3 -m venv .venv
    source .venv/bin/activate

    Windows (PowerShell):
    py -3 -m venv .venv
    .\.venv\Scripts\Activate.ps1

Then install Matplotlib into the active environment:

    python -m pip install matplotlib

run the plotter:

    python plot_logger.py LOGGER00.CSV # your file!

Replace LOGGER00.CSV with the path to your CSV file. If no path is given,
the program looks for LOGGER00.CSV in the current directory.

Brightness is plotted on the Y axis; select date/time or row for the X axis.
To select rows, enter the first and last data row and click
"Apply rows". Alternatively, enter From and To timestamps in the format
YYYY-MM-DD HH:MM:SS and click "Apply dates"; both endpoints are inclusive.
Each Apply button replaces the previous selection. The timestamps use the
date and time recorded by the logger without timezone conversion.
Use the plot window's toolbar to pan, zoom and reset the view.
The light values are plotted as recorded: the sketch multiplies the sensor's
lux reading by 10; -1 means no reading was ready.
