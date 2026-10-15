# Swan-Ganz Interface Firmware
The firmware for the Swan-Ganz interface runs on an ARM-MSPM0+ and has three main states.
1. Start 
2. Data Measurement
3. Results 

In Addition to the three states there are three periodic threads
1. DAS
2. Input Polling
3. Heartbeat 

## Periodic Threads
All periodic threads are located in DAS.c. They're manually scheduled in the RTOS in Schedule.h
### DAS
The data acquisition system periodic thread is deployed at 200 Hz and samples six ADC channels across the two internal ADCs. 
ADC0 samples the low and high gain data corresponding to the thermistor while ADC1 handles the low and high gain to the two 
pressure channels. The ADC uses 32 sample hardware averaging to smooth out the measured data. Each sample is placed into a buffer, 
then from the buffer they're each played in a respective queue for their channel to be accessed by the foreground threads.
### Input Polling
The input polling periodic thread is deployed at 20 Hz and checks for if the touchscreen has been interacted with. If it detects 
the screen has been touched it fetches the position of the touched area through an SPI protocol and translates the position to
a correlating button. This button is sent in a queue as an enum to be read by a foreground thread.
### Heartbeat
The heartbeat periodic thread runs at 5 Hz and flashes an LED to indicate to the user that the device is functional

## Start State 
The start state is the initial state of the machine when it is powered on. It consists of a menu that displays the current 
readings of temperature and pressure, parameters for data measurement, and the connection status of the device to the desktop 
application. 
### InitReadings()
The InitReadings() foreground thread retrieves the initial ADC data from each queue. Each queue will uses a semaphore to wait for
data, so this thread will be blocked until all data is retreived. The temperature data is run through a digital low pass filter
so that when the measurement begins, the filter already has the context to begin accurate measurements. About once a second,
the data read will be displayed to the screen by signaling to another thread that the values are ready to be read.
### DisplayStartMenu()
The DisplayStartMenu() foreground thread draws the UI onto the screen for parameters such as injectate volume and temperature.
After the menu is drawn the thread is blocked until the Input Polling thread signals for it to wake up and handle an input.
If the start button is pressed, this thread tells all other foreground threads to terminate, and signal when finished. Once 
all threads have signaled then this thread adds the threads for the next states and terminates itself.
### DisplayCurrentReadings()
The DisplayCurrentReadings() draws foreground thread blocks until the InitReadings thread signals that new values are ready to 
be displayed on the screen. When unblocked it grabs access to the LCD to update the new values.
### DisplayConnected()
The DisplayConnected() foreground thread reads a register correlated to if the device is connected to the desktop application and
displays a red or green dot on the LCD to indicate the connection status to the user.

## Data measurement State
### TransferData()
The TransferData() foreground thread is similar to the InitReadings() thread in the way it is blocked until all threads have
received data from the DAS. This thread however stores the initial 150 values in an array to be used as the initial temperature value 
in the Stewart-Hamilton equation. Additionally, every time data is received, it is transmitted through UART to the desktop application.
### DisplayCurrentReadings()
Another identical version of the DisplayCurrentReadings() thread is deployed, however it is designated for this state only.
### DisplayMeasuring()
This thread shows two progress bars. One for the injectate to guide the user at the rate in which they should push the injectate syringe.
Another progress bar shows the progress of the data measurement state. Once this progress bar fills, this thread cleans up the state
and starts the next one.
## Results State
### DisplayResults()
The DisplayResults() foreground thread displays the results and blocks until input to reset the system