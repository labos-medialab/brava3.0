#!/usr/bin/python
import serial, subprocess, json
from datetime import datetime
from time import sleep
import simplejson as json

data = {}
arduino = serial.Serial("/dev/ttyATH0", 230400)

def loadData():
  global data
  file = open("/tmp/lock.data", "r")
  data =json.loads(file.read())
  file.close()

def saveData():
  global data
  rawData = json.dumps(data)
  file = open("/tmp/lock.data", "w")
  file.write(json.dumps(data, sort_keys=True, indent=2))
  file.close()

def tko(kako, podatak):
  for ime in data.keys():
    for option in data[ime].keys():
      for tip in data[ime][option][kako].keys():
        if data[ime][option][kako][tip] == podatak:
          return ime.encode('ascii','ignore') + " " + tip.encode('ascii','ignore')
  return "vandal!:unknown "

def arduinoSend(data):
  arduino.write(data)
  sleep(0.05)

def unlock():
  arduinoSend("0")

loadData()
#file = open("/tmp/lock.data", "r")
#data = json.loads(file.read())
#file.close() 

arduino.flushInput(); arduino.flushOutput()
subprocess.call(["reset-mcu"]); sleep(1)
arduinoSend("bravaBegin\n")

while(1):
  if arduino.inWaiting():
    string = arduino.readline()
    string = string.replace('\r\n','')
    print string
    if ':' in string:
      kod = string[0:string.index(':')]
      if "rfid" == kod:
        kako = kod
        podatak = string[string.index(':')+1:]
        string = tko(kako, podatak)
        string = string.split(' ')
        print string
        temp = string[0].split(':')
        string[0] = temp[0]
        while len(string[0]) < 20: string[0] += ' '
        string[0] += '\n'
        string[0] = '20' + string[0]
        arduinoSend(string[0])
        while len(temp[1]) < 20: temp[1] += ' '
        temp[1] += '\n'
        temp[1] = '21' + temp[1]
        arduinoSend(temp[1])
        while len(string[1]) < 20: string[1] += ' '
        string[1] += '\n'
        string[1] = '22' + string[1]
        arduinoSend(string[1])
        if "root" in temp[1]:
          unlock()
          sleep(1)
      elif "fromSerial" == kod:
        print string



  hour = datetime.now().strftime("%H")
  minute = datetime.now().strftime("%M")
  second = datetime.now().strftime("%S")
  string = hour + ':' + minute + ":" + second
  while len(string) < 20: string = ' ' + string
  string = '23' + string + '\n'
  arduinoSend(string)

#           ind = theKeyList.index(string);
#           while len(string) < 20:
#               string += ' '
#           string += '\n'
#           string = '20'+string;
#           arduinoSend(string)
#           thread.start_new_thread(unlock, ())
