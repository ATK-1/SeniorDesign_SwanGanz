# Swan-Ganz Interface Hardware 
The Swan-Ganz Interface Hardware had the following requirements:
- One temperature channel for cardiac output calculation
- Two pressure transducer channels
- Discrete processor
- Intuitive user interface
- Uart to USB converter

The Swan-Ganz Interface has gone through two reveisions in development. The first of these circuit boards aimed to have baseline functionality for testing, utalizing a launchpad processor for simplicity. The second revision aimed to refine the original with a descirete processor, resistive touch screen, audio output and integrated UART to USB converter. This document will cover the hardware relavent to the second revision circuit board.

## Analog Interface Circuits
The heart of the hardware of this project are the analog interfaces for temperature and pressure. These interfaces are intended to interface the thermistor within the Swan-Ganz catheter and an external pressure transducer. These channels have very similar amplifier architectures however, the interfaces to the transducers themselves have small differences. Each of the analog input stages were sampled by the analog to digital converters on the MSPM0+ microcontroller.

### Pressure Analog Interface
The Swan-Ganz Catheter does not natively have any pressure transducers. Instead, it has two ports that connect to the fluid in the catheter to external pressure transducers to measure pressure. Thus, to satisfy both pressure ports, two pressure channels must be included to service two independent transducers.

The independent pressure transducers are implemented with a strain gauge. This means that pressure is exerted on wires within the gauge, stretching and compressing them, changing their ressitance. The a model the transducer's structure shown below. The model consists of four variable resistors. Two of the resistors’ resistance is directly proportional to pressure, and two resistors are indirectly proportional. This results in a structure resembling a voltage divider, where a voltage is applied across the transducer; one output lead increases its output voltage with pressure, while the other decreases. 

<img width="377" height="302" alt="StrainGaugeModel" src="https://github.com/user-attachments/assets/0640db28-dbbb-4a0b-b3a2-ada5ad708306" />


With this architecture in mind when a known voltage is applied across the gauge, an output voltage coresponding with pressure is produced across the V+ and V- nodes of the gauge. This signal is supplied to the instrumentation amplifier in the amplifier circuit at a very high gain to produce a clean pressure signal.

### Thermistor Analog Interface
The temperature channel hardware was created to interface with the thermistor embedded at the end of the Swan-Ganz catheter. This thermistor is intended to assist in the calculation of volumetric flow by analyzing its temperature over time; thus, it must translate temperature into a digital value. 

Within the thermistor, temperature and resistance are non-linear and inversely proportional. The thermistor has low resistance when the temperature is high and high resistance when the temperature is low. This relationship is shown by the graph below.

<img width="624" height="359" alt="ResistanceTemperatureGraph" src="https://github.com/user-attachments/assets/31837caf-89d2-45dd-bc34-61649f44dd8e" />


Thus, the thermistor can be placed in to a resistor divider with a constant resistance value producing an output voltage that corelates temperature to output voltage. Despite the relationship between resistance and temperature being non-linear, putting the thermsistor a resistor divider linearizes the output voltage as the relationship between resistance and voltage is non-linear in the divder itself. A second resistor dividor with constant resistances is used to set a minimum resistance, or a maximum temperature cutoff. The voltage across the variable and constant dividers is inserted into the instrumentation amplifier of the amplfier circuit to finish the temperature to voltage conversion. A diagram of the basic temperature circuit is shown below.

<img width="494" height="396" alt="ThermistorDiagram" src="https://github.com/user-attachments/assets/731de016-37df-4228-8535-8e47abda18f6" />


### Amplifier Stage
After converting the physical measurement to an analog voltage, the signal stil requires analog filtering and amplification prior to being digitally sampled by the microcontroller. To do this, we must know some information about the characteristics of our signals and digital sampling for the design of our ampilfier stage.

For our project, we decided to sample both the temperature and pressure signals at 200Hz. The typical resting heart rate in adults ranges from 60 to 100 beats per minute (BPM), which corresponds with a frequency of 1 to 1.67 Hz. During exercise, heart rate can increase up to 170 BPM, which is equivalent to 2.83 Hz. This means that the base frequency we would be investigating for pressure would be around 2 Hz. Despite this, we wanted to be able to view any additional high frequncy components of the heartbeat, as well oversamping our signal to increase the frequncy before we started having aliasing into our digtal signal. For the temperature component of our interface, we expected to receive even lower frequency signals, having frequencies of at most 10Hz, thus the 200Hz sample rate was kept for consistancy.

Because of the digital sample rate of 200Hz, any noise introduced to the system above 100Hz, cannot be distinguished from our desired signal, and cannot be
digitally filtered. Thus, to ensure that our resulting digital signals do not have any high-frequency noise, we created a 2-pole low-pass Butterworth filter set with a cut-off frequency of 75Hz. This allows us to pass the 20Hz signal frequencies while attenuating any high-frequency noise that is introduced from our environment. 

Amplification in this case is important as it determines the range and resolution of our
measurement. Our microcontroller, the MSPM0+, has 12-bit ADCs with an input voltage range of 0-3.3V. Therefore, to maximize the resolution of our signals, we must amplify the analog signals to make the largest desired analog signal equal to 3.3V. To do this, we have the amplification of the instrumentation amplifiers, tuned by a gain resistor, added by the 2-pole Butterworth filter, and an additional non-inverting amplifier for any additional required gain. To allow for both high resolution and high range, we sample with our ADCs both before and after the additional amplifier, allowing for a low-gain measurement that optimizes for range and a high-gain measurement for resolution. Below are pictures of the full analog input stages for the pressure and tempearture interfaces.

*Add gain stage filter picture*

## User Interface
The user interface for the Swan-Ganz Interface revolves around the 5” TFT display. This display is controlled by a discrete driver board, the Adafruit 1590. This driver board has SPI communication with the MSPM0+ processor. The driver board is required to boost the input voltage of to the screen to 24V, as well as continously refresh the TFT display. Drivers to communicate with the 1590 were proivided by Adafruit and translated to C and to be compatable with the MSPM0+ by Nathan DeLaRosa.

*Add picture of this circuit*

The TFT screen has resistive touchscreen. This is provided by four screen outputs that interface with a discrete resistive touch screen controller IC, the TSC2046IPWR. This IC interfaces with the MSPM0+ though SPI and, upon its request, applies a voltage across the screen in the X and Y direction and samples its ADCs to determine the position of a touch to the screen. This information is then transmitted back to the MSPM0+ over another SPI channel to be interpreted. The drivers to communicate with the TSC2046IPWR were written by Ashton Knecht.

*Add picture of this circuit*

In the case that issues arised with the resistive touch screen, five discrete hardware buttons are on the PCB as a backup user interface. The TFT is to provide a high-resolution, dynamic user interface able to display both measurement information and user instructions. This allows medical professionals to activly analyze measurement data while maintaining correct procedure by following directions.

Since MSPM0+ does not natively support USB communication, a discrete UART to USB
converter is included to allow the microcontroller to communicate with a host computer running the desktop application. This IC is the FT232RNL, and supports a 3MHz baud rate, and has transmit and receive debug LEDs. Additionally, the manufactuer, FTDI provides free drivers for Windows, Linux, and MacOs.  

*Add picture of this circuit*

Lastly, support for an audio amplifier circuit was made to add an additional layer of user feedback, however development on the core funcationality and testing were prioritized and so firmware functionality was not created. 

*Add picture of this circuit*


## Additional Elements of the PCB

### Reverse Polarity Protection



