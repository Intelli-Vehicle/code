#THRESHOLD = (0,100) # Grayscale threshold for dark things...
THRESHOLD = (200,255)
qq = (100,200)
import sensor, image, time
from pyb import LED
import car
from pid import PID
import time
from pyb import UART
rho_pid = PID(p=0.1, i=0)
theta_pid = PID(p=0.1, i=0)

#LED(1).on()
#LED(2).on()
#LED(3).on()
#LED(4).on()

sensor.reset()
#sensor.set_vflip(True)
#sensor.set_hmirror(True)
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QQVGA) # 80x60 (4,800 pixels) - O(N^2) max = 2,3040,000.
#sensor.set_windowing([0,20,80,40])
sensor.skip_frames(time = 2000)     # WARNING: If you use QQVGA it may take seconds
clock = time.clock()                # to process a frame sometimes.

output=0


while(True):
    clock.tick()
line = img.get_regression([THRESHOLD],invert=False, robust = False)

if (line):
    rho_err = abs(line.rho())-img.width()/2
    if line.theta()>90:
        theta_err = line.theta()-180
    else:
        theta_err = line.theta()
    img.draw_line(line.line(), color = 127)
    if line.magnitude()>1:
        #if -40<b_err<40 and -30<t_err<30:
        rho_output = rho_pid.get_pid(rho_err,5) #rho_err>0, theta_err>0: turn right
        theta_output = theta_pid.get_pid(theta_err,2)
        output = rho_output + theta_output
        car.run(30+output, 30-output)
        print(line.magnitude(),rho_err,theta_err,output)
    else:
        if output>0:
            car.run(-30,30)
        else:
            car.run(30,-30)
else:
    if output>0:
        car.run(-30,30)
    else:
        car.run(30,-30)
pass
print(clock.fps())

theta_output=theta_pid.get_pid(theta_err,3)
car.run(30+theta_output,30-theta_output)



