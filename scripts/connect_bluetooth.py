from traceback import print_exc

from bluetooth import (
    BluetoothSocket,
    btcommon,
    discover_devices,
    RFCOMM,
)
from osbrain import (
    Agent,
    run_agent,
    run_nameserver,
)


class Theseus(Agent):
    def setup(self, address, port):
        self.rfcomm = BluetoothSocket(RFCOMM)
        self.rfcomm.connect((address, port))
        self.rfcomm.settimeout(0.01)
        self.log = []
        self.each(0, 'receive')

    def send(self, message):
        try:
            self.rfcomm.send(message)
        except Exception as error:
            print_exc()

    def receive(self):
        try:
            received = self.rfcomm.recv(1024)
            print(received)
        except Exception:
            pass


if __name__ == '__main__':
    ns = run_nameserver()
    theseus = run_agent('theseus', base=Theseus)
    theseus.setup(address='00:21:13:01:D1:59', port=1)
    while True:
        text = input()
        if text == "exit":
            break
        else:
            theseus.send(text.encode('utf-8') + b'\0')
    ns.shutdown()
