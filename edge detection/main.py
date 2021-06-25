import time
import picamera
import picamera.array
import cv2
import numpy as np
import RPi.GPIO as gpio

gpio.setmode(gpio.BOARD)

# 设置GPIO口为输出
gpio.setup(pin1, gpio.OUT)
gpio.setup(pin2, gpio.OUT)
gpio.setup(pin3, gpio.OUT)
gpio.setup(pin4, gpio.OUT)

# 设置PWM波,频率为500Hz
pwm1 = gpio.PWM(pin1, 500)
pwm2 = gpio.PWM(pin2, 500)
pwm3 = gpio.PWM(pin3, 500)
pwm4 = gpio.PWM(pin4, 500)

# pwm波控制初始化
pwm1.start(0)
pwm2.start(0)
pwm3.start(0)
pwm4.start(0)


# with picamera.PiCamera() as camera:

#     #camera.start_preview()
# 
#     #time.sleep(10)
# 
#     with picamera.array.PiRGBArray(camera) as stream:
# 
#         camera.capture(stream, format='rgb')#format类型：bgr\rgb\h264 # 此时就可以获取到bgr的数据流了
# 
#         image = stream.array


image = cv2.imdecode(np.fromfile('field_2.jpg', dtype=np.uint8), -1) 
image_gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# for n in range(1,10):
#     image_canny = cv2.Canny(image_gray,n*10,150,3)
#     cv2.imwrite("test_%d.jpg"%n, image_canny)
#test for threshold

image_canny = cv2.Canny(image_gray,110,150,3)
img_blur = cv2.blur(image_canny, (15,15))

retval, dst = cv2.threshold(img_blur, 50, 255, cv2.THRESH_BINARY)

direction = 640#center of the width
for n in range(1,11):
    color = dst[721-n*72]
    center = 640
    white_count = np.sum(color == 255)
    white_index = np.where(color == 255)
    if white_count != 0:
        center = (white_index[0][white_count - 1] + white_index[0][0]) / 2
    direction = 0.6*direction+0.4*center
    gap = direction - 640
    print(center,direction)
    
print('OK')


cv2.imwrite("test.jpg", dst)