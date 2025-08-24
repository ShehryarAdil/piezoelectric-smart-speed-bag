# piezoelectric-smart-speed-bag
My University level Project

Energy Generation and Performance Metrics from Piezoelectric Smart Speed Bag

Abstract
Traditional speed bag training offers limited feedback and lacks sustainability. This project addresses these limitations by developing a "smart speed bag" equipped with piezoelectric sensors. These sensors not only harvest kinetic energy from impacts but also provide real-time performance metrics, such as hit count and impact force. A microcontroller processes the sensor data and transmits it wirelessly via Bluetooth to a mobile application. The project successfully demonstrated the feasibility of energy harvesting and accurate performance tracking using piezoelectric technology. This innovation has the potential to enhance the training experience for boxers and martial artists, providing valuable data for performance analysis and sustainable training solutions.

Key Findings & Potential Enhancements

Key Findings

Feasibility of Energy Harvesting: The project successfully demonstrated that piezoelectric technology can be used to harvest kinetic energy from impacts, generating a measurable amount of power.

Accurate Performance Tracking: The system accurately tracks performance metrics like hit count and impact force, proving its potential for practical use in training.

Key Improvements and Considerations

Debouncing: Peak detection uses a debouncing mechanism to prevent false positives.

Circular Buffer: A circular buffer is used for continuous and efficient data collection.

Clear Variable Names: The code uses descriptive variable names for improved readability.

Potential Enhancements

Data Logging: The system could be enhanced to log raw data to an SD card for later analysis.

Real-time Display: Basic statistics could be sent in real-time instead of waiting for a stop command.

More Advanced Signal Processing: More sophisticated techniques like filtering or FFT could be implemented to extract more information from the voltage readings.

Configuration Options: Parameters like the peak threshold could be configurable over Bluetooth, making the device more adaptable.
