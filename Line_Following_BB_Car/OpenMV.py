import pyb, sensor, image, time, math

THRESHOLD = (0, 100)
BINARY_VISIBLE = True

enable_lens_corr = False # turn on for straighter lines...
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE) # grayscale is faster
sensor.set_framesize(sensor.QQQVGA)
sensor.set_vflip(True)
sensor.set_hmirror(True)
sensor.skip_frames(time = 2000)
clock = time.clock()

# All lines also have `x1()`, `y1()`, `x2()`, and `y2()` methods to get their end-points
# and a `line()` method to get all the above as one 4 value tuple for `draw_line()`.

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)

while(True):
   clock.tick()
   img = sensor.snapshot().binary([THRESHOLD]) if BINARY_VISIBLE else sensor.snapshot()

   line = img.get_regression([(255, 255) if BINARY_VISIBLE else THRESHOLD], robust=True)

   if (line):
      img.draw_line(line.line(), color=127)
      print_args = (line.x1(), line.y1(), line.x2(), line.y2(), line.length(), line.theta(), line.rho())
      print(line)
      uart.write(("x1: %d, y1: %d, x2: %d, y2: %d, length: %d, theta: %d, rho: %d\n" % print_args).encode())
   #uart.write(("Tx: %f, Ty %f % (line)).encode())


   #print("FPS %f" % clock.fps())
