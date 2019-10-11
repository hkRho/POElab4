import serial

arduino = serial.Serial("COM8", 9600)
arduino.write(b'30')