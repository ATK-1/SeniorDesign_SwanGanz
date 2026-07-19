<img width="377" height="302" alt="StrainGaugeModel" src="https://github.com/user-attachments/assets/fdbc0333-1494-4f08-90f8-66556a6850d4" />
# Swan-Ganz Interface Hardware 
The Swan-Ganz Interface Hardware had the following requirements:
- One temperature channel for cardiac output calculation
- Two pressure transducer channels
- Discrete processor
- Intuitive user interface
- Uart to USB converter

The Swan-Ganz Interface has gone through two reveisions in development. The first of these circuit boards aimed to have baseline functionality for testing, utalizing a launchpad processor for simplicity. The second revision aimed to refine the original with a descirete processor, resistive touch screen, audio output and integrated UART to USB converter. This document will cover the hardware relavent to the second revision circuit board.

## Analog Interface Circuits
The heart of the hardware of this project are the analog interfaces for temperature and pressure. These interfaces are intended to interface the thermistor within the Swan-Ganz catheter and an external pressure transducer. These channels have very similar amplifier architectures however, the interfaces to the transducers themselves have small differences.

### Pressure Analog Interface
The Swan-Ganz Catheter does not natively have any pressure transducers. Instead, it has two ports that connect to the fluid in the catheter to external pressure transducers to measure pressure. Thus, to satisfy both pressure ports, two pressure channels must be included to service two independent transducers.

The independent pressure transducers are implemented with a strain gauge. This means that pressure is exerted on wires within the gauge, stretching and compressing them, changing their ressitance. The a model the transducer's structure shown below. The model consists of four variable resistors. Two of the resistors’ resistance is directly proportional to pressure, and two resistors are indirectly proportional. This results in a structure resembling a voltage divider, where a voltage is applied across the transducer; one output lead increases its output voltage with pressure, while the other decreases. <img width="494" height="396" alt="ThermistorDiagram" src="https://github.com/user-attachments/assets/3ac712ec-04de-4ae6-ba56-ff43c52f99c3" />


<img width="377" height="302" alt="StrainGaugeModel" src="https://github.com/user-attachments/assets/0640db28-dbbb-4a0b-b3a2-ada5ad708306" />


With this architecture in mind when a known voltage is applied across the gauge, an output voltage coresponding with pressure is produced across the V+ and V- nodes of the gauge. This signal is supplied to the instrumentation amplifier in the amplifier circuit at a very high gain to produce a clean pressure signal.

### Thermistor Analog Interface
The temperature channel hardware was created to interface with the thermistor embedded at the end of the Swan-Ganz catheter. This thermistor is intended to assist in the calculation of volumetric flow by analyzing its temperature over time; thus, it must translate temperature into a digital value. 

Within the thermistor, temperature and resistance are non-linear and inversely proportional. The thermistor has low resistance when the temperature is high and high resistance when the temperature is low. This relationship is shown by the graph below.

<img width="624" height="359" alt="ResistanceTemperatureGraph" src="https://github.com/user-attachments/assets/31837caf-89d2-45dd-bc34-61649f44dd8e" />


Thus, the thermistor can be placed in to a resistor divider with a constant resistance value producing an output voltage that corelates temperature to output voltage. Despite the relationship between resistance and temperature being non-linear, putting the thermsistor a resistor divider linearizes the output voltage as the relationship between resistance and voltage is non-linear in the divder itself. A second resistor dividor with constant resistances is used to set a minimum resistance, or a maximum temperature cutoff. The voltage across the variable and constant dividers is inserted into the instrumentation amplifier of the amplfier circuit to finish the temperature to voltage conversion. A diagram of the basic temperature circuit is shown below.

<img width="494" height="396" alt="ThermistorDiagram" src="https://github.com/user-attachments/assets/731de016-37df-4228-8535-8e47abda18f6" />


