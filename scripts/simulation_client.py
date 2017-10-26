import numpy
import zmq


MAZE_SIZE = 16
MAZE_TARGET = (10, 8)


distances = bytes([abs(x - MAZE_TARGET[0]) + abs(y - MAZE_TARGET[1])
                  for y in range(MAZE_SIZE) for x in range(MAZE_SIZE)])
walls = numpy.zeros((MAZE_SIZE, MAZE_SIZE)).astype('uint8')
walls[-1, :] += 2
walls[:, 0] += 4
walls[0, :] += 8
walls[:, -1] += 16
walls = walls.tobytes(order='C')

context = zmq.Context()
pusher = context.socket(zmq.PUSH)
pusher.connect('tcp://127.0.0.1:6574')

pusher.send(b'F' + distances + b'F' + walls)
