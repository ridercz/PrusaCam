; This G-code will set GPIO pin 73 (available in J19 header on Einsy Rambo) to zero
; Note: Display message does not work on MK3 right now, see https://shop.prusa3d.com/forum/others-archive--f82/screen-message-t3304.html
M117 GPIO 73 low
M42 S0 P73
M84             ; disable motors