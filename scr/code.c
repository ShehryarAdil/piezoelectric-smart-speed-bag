#include "BluetoothSerial.h"
String device_name = "PUNCH METER";
const int voltagePin1 = 32;
const int voltagePin2 = 33;
const int voltagePin3 = 35;
const int dcVoltagePin = 34; // New pin for DC voltage
const float referenceVoltage = 3.3;
const int arraySize = 100;
const float R1 = 4700.0;
const float R2 = 1000.0;
const float voltageDividerFactor = (R1 + R2) / R2;
const float voltageConversionFactor = referenceVoltage / 1023.0;
const float peakThreshold = 0.5;
const unsigned long debounceTime = 200;
struct VoltageData {
float voltage;
unsigned long timestamp;
};
VoltageData voltageReadings1[arraySize];
VoltageData voltageReadings2[arraySize];
VoltageData voltageReadings3[arraySize];
int arrayIndex = 0;
bool isCollectingData = false;
unsigned long startTime = 0;
unsigned long stopTime = 0;
int numPeaks = 0;
float previousDCVoltage = 0; // Store previous DC voltage for comparison
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled!
#endif
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available.
#endif
BluetoothSerial SerialBT;
void resetCalculations() {
arrayIndex = 0;
numPeaks = 0;
}
void setup() {
Serial.begin(115200);
SerialBT.begin(device_name);
Serial.printf("Device \"%s\" started.\nPair it with Bluetooth!\n",
device_name.c_str());
SerialBT.println("NOTE: Available commands:");
SerialBT.println(" - start: Start data collection.");
SerialBT.println(" - stop: Stop data collection.");
SerialBT.println(" - dc: Display DC Voltage and Power Saved.");
SerialBT.println(" - avg: Display Average Voltage (All Inputs).");
SerialBT.println(" - opt: Display Optimal Inputs (All Inputs).");
SerialBT.println(" - high: Display Highest Voltage (All Inputs).");
SerialBT.println(" - low: Display Lowest Voltage (All Inputs).");
SerialBT.println(" - rate: Display Average rate of optimal inputs (All
Inputs).");
SerialBT.println(" - time: Display Total Time.");
SerialBT.println(" - peaks: Display Number of Peaks (Highest Input).");
SerialBT.println(" - all: Display all data.");
SerialBT.println("Note: Send commands one at a time after data collection has
stopped.");
SerialBT.println(); // Add an extra newline for better readability
}
void loop()
{
float totalTimeInSeconds = 0;
String receivedString = "";
while (SerialBT.available()) {
char c = SerialBT.read();
if (c == '\n') {
break;
}
receivedString += c;
}
if (receivedString.length() > 0) {
if (receivedString == "start") {
isCollectingData = true;
resetCalculations();
startTime = millis();
SerialBT.println("Data collection started.");
} else if (receivedString == "stop") {
isCollectingData = false;
stopTime = millis();
SerialBT.println("Data collection stopped.");
}
}
if (isCollectingData) {
unsigned long currentTime = millis();
int rawVoltage1 = analogRead(voltagePin1);
float vout1 = rawVoltage1 * voltageConversionFactor;
float vin1 = vout1 * voltageDividerFactor;
int rawVoltage2 = analogRead(voltagePin2);
float vout2 = rawVoltage2 * voltageConversionFactor;
float vin2 = vout2 * voltageDividerFactor;
int rawVoltage3 = analogRead(voltagePin3);
float vout3 = rawVoltage3 * voltageConversionFactor;
float vin3 = vout3 * voltageDividerFactor;
voltageReadings1[arrayIndex].voltage = vin1;
voltageReadings1[arrayIndex].timestamp = currentTime;
voltageReadings2[arrayIndex].voltage = vin2;
voltageReadings2[arrayIndex].timestamp = currentTime;
voltageReadings3[arrayIndex].voltage = vin3;
voltageReadings3[arrayIndex].timestamp = currentTime;
arrayIndex++;
if (arrayIndex >= arraySize) {
arrayIndex = 0;
}
} else if (!isCollectingData && arrayIndex > 0)
{ // This is now correctly outside the isCollectingData block
totalTimeInSeconds = (stopTime - startTime) / 1000.0;
float highestPeak = 0;
int highestPeakInput = 0;
for (int i = 0; i < arrayIndex; i++) {
if (voltageReadings1[i].voltage > highestPeak) {
highestPeak = voltageReadings1[i].voltage;
highestPeakInput = 1;
}
if (voltageReadings2[i].voltage > highestPeak) {
highestPeak = voltageReadings2[i].voltage;
highestPeakInput = 2;
}
if (voltageReadings3[i].voltage > highestPeak) {
highestPeak = voltageReadings3[i].voltage;
highestPeakInput = 3;
}
}
numPeaks = 0;
unsigned long lastPeakTimeLocal = 0;
VoltageData *highestVoltageReadings;
if (highestPeakInput == 1) {
highestVoltageReadings = voltageReadings1;
} else if (highestPeakInput == 2) {
highestVoltageReadings = voltageReadings2;
} else {
highestVoltageReadings = voltageReadings3;
}
for (int i = 1; i < arrayIndex; i++) {
unsigned long currentTime = highestVoltageReadings[i].timestamp;
if (highestVoltageReadings[i].voltage >= peakThreshold &&
highestVoltageReadings[i - 1].voltage < peakThreshold && (currentTime -
lastPeakTimeLocal > debounceTime)) {
numPeaks++;
lastPeakTimeLocal = currentTime;
}
}
float totalVoltage = 0;
int numOptimalInputs = 0;
float highestVoltageAll = 0;
float lowestNonZeroVoltageAll = referenceVoltage;
unsigned long lastOptimalInputTimestampAll = 0;
float totalTimeDiffAll = 0;
int numTimeDiffPairsAll = 0;
for (int i = 0; i < arrayIndex; i++) {
float voltages[] = {voltageReadings1[i].voltage,
voltageReadings2[i].voltage, voltageReadings3[i].voltage};
for (float voltage : voltages) {
totalVoltage += voltage;
if (voltage > 0) {
numOptimalInputs++;
if (lastOptimalInputTimestampAll != 0) {
totalTimeDiffAll += voltageReadings1[i].timestamp -
lastOptimalInputTimestampAll;
numTimeDiffPairsAll++;
}
lastOptimalInputTimestampAll = voltageReadings1[i].timestamp;
}
if (voltage > highestVoltageAll) {
highestVoltageAll = voltage;
}
if (voltage > 0 && voltage < lowestNonZeroVoltageAll) {
lowestNonZeroVoltageAll = voltage;
}
}
}
float averageVoltage = numOptimalInputs > 0 ? totalVoltage /
numOptimalInputs : 0;
if (numOptimalInputs == 0) lowestNonZeroVoltageAll = 0;
int rawDCVoltage = analogRead(dcVoltagePin);
float voutDC = rawDCVoltage * voltageConversionFactor;
float vinDC = voutDC * voltageDividerFactor;
receivedString = ""; // Clear for output command
while (SerialBT.available()) {
char c = SerialBT.read();
if (c == '\n') {
break;
}
receivedString += c;
}
if (receivedString.length() > 0) {
if (receivedString == "dc") {
SerialBT.print("DC Voltage: ");
SerialBT.print(vinDC, 2);
SerialBT.println(" V");
if (previousDCVoltage > 0) {
float powerSaved = (previousDCVoltage - vinDC) / previousDCVoltage *
100;
SerialBT.print("Power Saved: ");
SerialBT.print(powerSaved, 2);
SerialBT.println(" %");
} else {
SerialBT.println("No previous DC voltage to compare.");
}
previousDCVoltage = vinDC;
} else if (receivedString == "avg") {
SerialBT.print("Average Voltage (All Inputs): ");
SerialBT.print(averageVoltage, 2);
SerialBT.println(" V");
} else if (receivedString == "opt") {
SerialBT.print("Optimal Inputs (All Inputs): ");
SerialBT.println(numOptimalInputs);
} else if (receivedString == "high") {
SerialBT.print("Highest Voltage (All Inputs): ");
SerialBT.print(highestVoltageAll, 2);
SerialBT.println(" V");
} else if (receivedString == "low") {
SerialBT.print("Lowest Voltage (All Inputs): ");
SerialBT.print(lowestNonZeroVoltageAll, 2);
SerialBT.println(" V");
} else if (receivedString == "rate") {
if (numTimeDiffPairsAll > 0) {
float averageTimeDiffInSeconds = totalTimeDiffAll / numTimeDiffPairsAll
/ 1000.0;
SerialBT.print("Average rate of optimal inputs (All Inputs): ");
SerialBT.println(averageTimeDiffInSeconds, 2);
} else {
SerialBT.println("No consecutive optimal inputs found. (All Inputs)");
}
} else if (receivedString == "time") {
SerialBT.print("Total Time: ");
SerialBT.print(totalTimeInSeconds, 2);
SerialBT.println(" seconds");
} else if (receivedString == "peaks") {
SerialBT.print("Number of Peaks (Highest Input): ");
SerialBT.println(numPeaks);
} else if (receivedString == "all") {
SerialBT.print("DC Voltage: ");
SerialBT.print(vinDC, 2);
SerialBT.println(" V");
if (previousDCVoltage > 0) {
float powerSaved = (previousDCVoltage - vinDC) / previousDCVoltage *
100;
SerialBT.print("Power Saved: ");
SerialBT.print(powerSaved, 2);
SerialBT.println(" %");
} else {
SerialBT.println("No previous DC voltage to compare.");
}
previousDCVoltage = vinDC;
SerialBT.print("Average Voltage (All Inputs): ");
SerialBT.print(averageVoltage, 2);
SerialBT.println(" V");
SerialBT.print("Optimal Inputs (All Inputs): ");
SerialBT.println(numOptimalInputs);
SerialBT.print("Highest Voltage (All Inputs): ");
SerialBT.print(highestVoltageAll, 2);
SerialBT.println(" V");
SerialBT.print("Lowest Voltage (All Inputs): ");
SerialBT.print(lowestNonZeroVoltageAll, 2);
SerialBT.println(" V");
if (numTimeDiffPairsAll > 0) {
float averageTimeDiffInSeconds = totalTimeDiffAll /
numTimeDiffPairsAll / 1000.0;
SerialBT.print("Average rate of optimal inputs (All Inputs): ");
SerialBT.println(averageTimeDiffInSeconds, 2);
} else {
SerialBT.println("No consecutive optimal inputs found. (All Inputs)");
}
SerialBT.print("Total Time: ");
SerialBT.print(totalTimeInSeconds, 2);
SerialBT.println(" seconds");
SerialBT.print("Number of Peaks (Highest Input): ");
SerialBT.println(numPeaks); // Use the renamed variable here
}
}
arrayIndex = 0;
}
delay(100);
}
