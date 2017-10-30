import os
import cmd
from pprint import pprint
from time import sleep
from traceback import print_exc

from bluetooth import (
    BluetoothSocket,
    RFCOMM,
)
from osbrain import (
    Agent,
    run_agent,
    run_nameserver,
)


class Proxy(Agent):
    def on_init(self):
        self.log = []
        self.buffer = b''

    def setup(self, address, port):
        self.rfcomm = BluetoothSocket(RFCOMM)
        self.rfcomm.connect((address, port))
        self.rfcomm.settimeout(0.01)
        self.each(0, 'receive')

    def process_received(self, received):
        self.buffer += received
        if b'\n' not in self.buffer:
            return
        splits = self.buffer.split(b'\n')
        for message in splits[:-1]:
            fields = message.split(b',')
            log = [x.decode('utf-8') for x in fields[:4]]
            log[0] = float(log[0])
            body = b','.join(fields[4:])
            if not body.startswith(b'RAW'):
                body = body.decode('utf-8')
            else:
                raise NotImplementedError()
            log = tuple(log + [body])
            if log[1] != 'INFO':
                print(log)
            self.log.append(log)
        self.buffer = splits[-1]

    def send(self, message):
        try:
            self.rfcomm.settimeout(1.)
            self.rfcomm.send(message)
        except Exception as error:
            print_exc()

    def receive(self):
        try:
            self.rfcomm.settimeout(0.01)
            received = self.rfcomm.recv(1024)
            self.process_received(received)
        except Exception:
            pass

    def tail(self, N):
        return self.log[-N:]

    def last_log_time(self):
        if not self.log:
            return 0
        return float(self.log[-1].split(b',')[0])


def complete_subcommands(text, subcommands):
    if not text:
        return subcommands
    return [c for c in subcommands if c.startswith(text)]


class Theseus(cmd.Cmd):
    prompt = '>>> '
    LOG_SUBCOMMANDS = ['all', 'clear']

    def cmdloop(self, intro=None):
        """Modified cmdloop() to handle keyboard interruptions."""
        while True:
            try:
                super().cmdloop(intro='')
                self.postloop()
                break
            except KeyboardInterrupt:
                print('^C')
                self.interrupted = True
                return False

    def emptyline(self):
        """Do nothing on empty line."""
        pass

    def preloop(self):
        self.interrupted = False
        self.ns = run_nameserver()
        self.proxy = run_agent('proxy', base=Proxy)
        self.proxy.after(0, 'setup', address='00:21:13:01:D1:59', port=1)

    def postloop(self):
        if not self.interrupted:
            self.ns.shutdown()

    def do_battery(self, *args):
        """Get battery voltage."""
        self.proxy.send('battery\0')

    def do_clear(self, *args):
        """Clear screen."""
        os.system('clear')

    def do_log(self, extra):
        """Get the full log."""
        if extra == 'all':
            pprint(self.proxy.get_attr('log'))
        elif extra == 'clear':
            self.proxy.set_attr(log=[])
        elif extra.isnumeric():
            pprint(self.proxy.tail(int(extra)))
        else:
            pprint(self.proxy.tail(10))

    def complete_log(self, text, line, begidx, endidx):
        return complete_subcommands(text, self.LOG_SUBCOMMANDS)

    def do_exit(self, *args):
        """Exit shell."""
        return True

    def do_EOF(self, line):
        """Exit shell."""
        return True


if __name__ == '__main__':
    Theseus().cmdloop()
