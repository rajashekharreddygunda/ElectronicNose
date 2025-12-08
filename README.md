# Electronic Nose with LoRa-Based Remote Gas Monitoring

An Electronic Nose (E-Nose) system capable of detecting and quantifying multiple gases using MQ-series sensors.
This improved version includes LoRa long-range wireless communication and a solar-powered transmitter node, making it suitable for remote environmental monitoring.

*Project Overview*

This project uses four MQ gas sensors:
MQ-3 – Ethanol
MQ-135 – Ammonia
MQ-7 – Carbon Monoxide (CO)
MQ-8 – Hydrogen (H₂)

The sensors are interfaced with an ATmega328P (Arduino UNO) at the transmitter end, which computes PPM values using equations derived from datasheet sensitivity curves.
These values are sent wirelessly using the LoRa SX1278 433MHz module to a receiver node, where the data is displayed on an LCD and the serial monitor.

The transmitter node is designed to run entirely on solar power, making the system ideal for long-term outdoor deployment.
