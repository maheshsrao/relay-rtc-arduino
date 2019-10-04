# relay-rtc-arduino
Relay control in real time using arduino and RTC. 
Used in Fishtank lighting, Plant irrigation.

Time values specified in structure ON - OFF sequence. Also manually can override ON/OFF sequence using Bluetooth. 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Hardware: 
    1. 5V - 2 Channel Relay module on Normally Open configuration.
    2. DS3231 RTC Module
    3. HC05 Bluetooth
    4. Arduino Micro
    5. Jumper cables, prototype board, 230v Sockets and USB cable for powering arduino. 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Time Schedule Eg: 

// Total number of schedules
#define NUMBER_OF_SCHEDULES  9

// Schedule timings: {ON hour, ON Minute, OFF Hour, OFF Minute}
struct schedules stime[NUMBER_OF_SCHEDULES] = {
  {{7, 0}, {7, 45}},   //From morning 7AM to 7:45AM. 0.45 Total minutes
  {{8, 0}, {9, 15}},   //1.15
  {{11, 0}, {12, 0}},  //1.00
  {{13, 0}, {14, 0}},  //1.00 (1PM - 2PM)
  {{15, 45}, {16, 5}}, //0.20 (3:45PM - 4:05PM)
  {{16, 45}, {17, 0}}, //0.15 (4:45PM - 5PM)
  {{18, 0}, {18, 45}}, //0.45 (6PM - 6:45PM)
  {{19, 45}, {20, 0}}, //0.15 (7:45PM - 8PM)
  {{21, 0}, {22, 5}}   //1.30 (9PM - 10:05PM)
};
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
files: *.ino - Arduino sketch.
       *.stl - 3d printable Board for accomadating Arduino, 2Channel Relay, RTC Module, Bluetooth & 2 Nos Socket. 
       Wiring Diagram - To Be Uploaded.
