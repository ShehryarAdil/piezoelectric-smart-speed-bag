1. Hardware Setup and Initialization:
• Voltage Measurement: The code uses analog pins 32, 33, and 35 to read voltage
from three separate sources. It also uses pin 34 to measure a DC voltage. These are
likely connected to voltage dividers to scale down higher voltages to a range
readable by the ESP32's ADC (0-3.3V).
• Bluetooth Communication: The ESP32's Bluetooth functionality is used to
communicate with a connected device (e.g., a smartphone or computer). The
device is named "PUNCH METER".
• Voltage Conversion: The code uses voltageConversionFactor and
voltageDividerFactor to convert the raw analog readings (0-1023) to actual voltage
values. This is crucial for accurate measurements.
• Data Storage: The VoltageData struct stores both the voltage reading and the
timestamp of the reading. Three arrays (voltageReadings1, voltageReadings2,
voltageReadings3) of these structs are used to store the data from each input.
2. Data Collection and Control:
• Start/Stop Commands: The code listens for "start" and "stop" commands over
Bluetooth. These commands control the data collection process.
• Data Acquisition: When data collection is active (isCollectingData == true), the
code reads the analog values from the three voltage inputs at each loop iteration.
The converted voltage values and timestamps are stored in the respective arrays.
• Circular Buffer: The arrayIndex variable and the modulo operator (arrayIndex++
and the subsequent if (arrayIndex >= arraySize) { arrayIndex = 0; })
implement a circular buffer. This allows the code to continuously collect data
without running out of memory, overwriting the oldest data when the buffer is full.
• Time Measurement: The startTime and stopTime variables, along with millis(),
are used to measure the duration of the data collection period.
3. Data Analysis and Calculations (After "stop" command):
• Peak Detection: The code identifies peaks in the highest voltage input using a
threshold (peakThreshold) and a debounce time (debounceTime). This prevents false
peak detections due to noise or small fluctuations.
• Highest Peak Identification: Determines which of the three inputs has the highest
peak voltage.
• Average Voltage: Calculates the average voltage across all inputs, considering only
non-zero readings (presumably to exclude periods when an input is inactive).
• Optimal Inputs: Counts the number of voltage readings across all inputs that are
above zero. This is referred to as "optimal inputs" in the code.
• Highest and Lowest Voltages: Finds the highest and lowest non-zero voltage
readings across all inputs.
• Average Rate of Optimal Inputs: Calculates the average time difference between
consecutive optimal inputs. This provides a measure of how frequently the inputs
are active.
• DC Voltage Measurement and Power Savings: Reads the DC voltage from pin 34.
If a previous DC voltage reading is available, it calculates and displays the
percentage of power saved (assuming a decrease in voltage corresponds to power
savings).
4. Bluetooth Output:
• Command Handling: After data collection stops, the code waits for further
Bluetooth commands.
• Output Formatting: The code formats the calculated data into human-readable
strings and sends them over Bluetooth.
• "all" Command: The "all" command provides a comprehensive summary of all the
calculated metrics.
Key Improvements and Considerations:
• Debouncing: The peak detection uses debouncing to prevent multiple peak
detections from a single event.
• Circular Buffer: The use of a circular buffer is efficient for continuous data
collection.
• Clearer Variable Names: The variable names are generally descriptive, making the
code easier to understand.
• Error Handling: The code includes basic error handling for Bluetooth initialization.
• Units: The code includes units (V, seconds, %) in the output, which is good
practice.
Potential Enhancements:
• Data Logging: The code could be enhanced to log the raw data to an SD card or
other storage for later analysis.
• Real-time Display: Instead of waiting for the "stop" command, some basic
statistics could be sent over Bluetooth in real-time.
• More Advanced Signal Processing: More sophisticated signal processing
techniques (e.g., filtering, FFT) could be implemented to extract more information
from the voltage readings.
• Configuration Options: The code could be made more configurable by allowing the
user to set parameters like the peakThreshold and debounceTime over Bluetooth.
