RF-SNIPER by Antuino

Frequency scanner with graphical display in the range (0.1 ... 150) MHz inspired by the project "ANTUINO" by Ashhar Farhan - VU2ESE. This device is an RF bridge type radio frequency scanner.

https:\\www.qsl.net/yo6pir/sniper.html

![image](https://github.com/user-attachments/assets/d7e8e65a-c6d5-4c29-920f-9ebf5cbcd675)
Compared to the original project, I have made some changes, and I say, some improvements related to maneuverability, as follows:

I have eliminated the encoder as the main control and replaced it with a series of 4 "soft-touch" keys that mainly eliminate the "click" errors of mechanical encoders. The keys are more ergonomic and easier to select, in my opinion.
I have solved some BUGs in the program regarding the selection of various functions that were not clear enough when selecting them.
I have added to the main screen the indication regarding the voltage of the device's power battery
I have reconfigured the display of SWR values ​​on the display, limiting it to the value of 9.99 being sufficient for an accurate reading; values ​​above this threshold are no longer readable, therefore, not displayed.
In the "PWR" and "SNA" menus, the plotterr is scanned continuously, with the signal that changes in real time being visible. We have also reduced the plotter scan period from 50ms to 20ms.
If the cursor is moved on the resulting graph, after pressing the ENTER key this value is stored on the graph center and on the next scan it is taken as the centrally selected frequency. An advantage for multiple frequency scans in wide intervals.
