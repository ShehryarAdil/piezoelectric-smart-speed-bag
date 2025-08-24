Source Code (src)
This folder contains the core microcontroller and mobile application code for the Smart Speed Bag project.

The code is designed to perform the following functions:

Continuous Data Collection: Reads voltage from piezoelectric sensors.

Peak Detection: Identifies voltage peaks to register a hit, using a threshold and a debouncing mechanism.

Data Processing: Calculates the average voltage and total hit count.

Bluetooth Communication: Transmits processed data to the mobile application upon a stop command.

Output Formatting: Formats the calculated data into a human-readable string.

The code includes basic error handling for Bluetooth initialization and uses a circular buffer for efficient, continuous data collection.
