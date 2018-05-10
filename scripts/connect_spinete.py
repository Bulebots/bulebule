import serial
import datetime
import zmq


def publish(log):
    body = log[-1]
    if not body.startswith('PUB'):
        return
    fields = body.split(',')
    if fields[1] != 'line':
        raise NotImplementedError()
    return fields


def process_received(received, buf):
    buf += received
    if b'\n' not in buf:
        return 0
    splits = buf.split(b'\n')
    for message in splits[:-1]:
        fields = message.split(b',')
        log = [x.decode('utf-8') for x in fields[:4]]
        try:
            log[0] = float(log[0])
        except ValueError:
            pass
        body = b','.join(fields[4:])
        if not body.startswith(b'RAW'):
            body = body.decode('utf-8')
        else:
            raise NotImplementedError()
        log = tuple(log + [body])
        if log[1] == 'ERROR':
            print(log)
    buf = splits[-1]
    prefilt = publish(log)
    return(prefilt, buf)

ser = serial.Serial('/dev/ttyUSB0', 921600)
last_time = datetime.datetime.utcnow().timestamp()
context = zmq.Context()
publisher = context.socket(zmq.PUB)
host = "127.0.0.1"
publisher.bind('tcp://{}:{}'.format(host, 5000))
buf = b''
log = []
integ_pc = 0
while True:
    try:
        i = ser.read(80)
        data, buf = process_received(i, buf)
        print(data)
        now = datetime.datetime.utcnow()
        if data[2] == 'gyro_raw':
            gyro_raw = float(data[3])
            publisher.send_pyobj(('gyro_raw', now.timestamp(), gyro_raw))
        elif data[2] == 'gyro_dps':
            gyro_dps = float(data[3])
            publisher.send_pyobj(('gyro_dps', now.timestamp(), gyro_dps))
        elif data[2] == 'gyro_degrees':
            gyro_degrees = float(data[3])
            publisher.send_pyobj(('gyro_degrees', now.timestamp(),
                                  gyro_degrees))
        else:
            pass
        last_time = now.timestamp()
    except KeyboardInterrupt:
        ser.close()
        break
