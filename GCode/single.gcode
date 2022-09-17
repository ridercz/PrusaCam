; This G-code will alternate GPIO pin 73 (available in J19 header on Einsy Rambo)
; between low and high every second - used for testing
; Note: Display message does not work on MK3 right now, see https://shop.prusa3d.com/forum/others-archive--f82/screen-message-t3304.html
M117 GPIO 73 low
M42 S0 P73
G4 S1
M117 GPIO 73 high
M42 S255 P73
G4 S1
M117 GPIO 73 low
M42 S0 P73
G4 S1
M84             ; disable motors