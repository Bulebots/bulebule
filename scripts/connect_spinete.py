import serial
import datetime
import socket
import time
import zmq

ser = serial.Serial('/dev/ttyUSB0', 921600)
last_time = datetime.datetime.utcnow().timestamp()
context = zmq.Context()
publisher = context.socket(zmq.PUB)
host = "127.0.0.1"
publisher.bind('tcp://{}:{}'.format(host, 5000))
integ_pc = 0
while True:
    try:
        data = ser.readline()
        if data:
            data = data.decode("utf-8").strip('\n').split(',')
            print(data)
            if data[1] == 'INFO':
                now = datetime.datetime.utcnow()
                vo = int(data[4])*3.3/4096
                vref = int(data[5])*3.3/4096
                vdiff = vo - vref
                #0.67 mv/dps * 10 (op. amp)
                if abs(vdiff) < 0.06:
                    degrees = 0
                else:
                    dps = (vdiff)/(0.00067*2)
                    degrees = dps*(now.timestamp() - last_time)
                integ_pc = integ_pc - degrees
                publisher.send_pyobj(('D', now.timestamp(),
                (0, 0, integ_pc)))
                publisher.send_pyobj(('Vo', now.timestamp(),vo))
                publisher.send_pyobj(('Vdiff', now.timestamp(),vdiff))
                last_time = now.timestamp()
    except KeyboardInterrupt:
        ser.close()
        break
