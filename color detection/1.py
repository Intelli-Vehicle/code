import cv2
import numpy as np
import RPI.GPIO as GPIO

# 'red': {'Lower': np.array([0, 60, 60]), 'Upper': np.array([6, 255, 255])}

color_dist = {'1': {'Lower': np.array([134, 59, 175]), 'Upper': np.array([151, 255, 255])},
              '2': {'Lower': np.array([17, 10, 216]), 'Upper': np.array([66, 255, 255])},
              '3': {'Lower': np.array([100, 80, 46]), 'Upper': np.array([124, 255, 255])},
              }

cap = cv2.VideoCapture(0)
color_line = {'1': (0, 0, 255),
              '2': (0, 255, 0),
              '3': (255, 0, 0),
              }

color_PIN = {'1': 11,
             '2': 13,
             '3': 15,
             }
# frame = cv2.imdecode(np.fromfile('B.jpg', dtype=np.uint8), -1)
# cv2.namedWindow('camera', cv2.WINDOW_NORMAL)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(11, GPIO.OUT)
GPIO.setup(13, GPIO.OUT)
GPIO.setup(15, GPIO.OUT)

while cap.isOpened():
    GPIO.output(11, GPIO.LOW)
    GPIO.output(13, GPIO.LOW)
    GPIO.output(15, GPIO.LOW)
    state = ''
    ret, frame = cap.read()
    if ret:
        if frame is not None:
            gs_frame = cv2.GaussianBlur(frame, (5, 5), 0)  # 高斯模糊
            hsv = cv2.cvtColor(gs_frame, cv2.COLOR_BGR2HSV)  # 转化成HSV图像
            erode_hsv = cv2.erode(hsv, None, iterations=2)  # 腐蚀 粗的变细
            hStack = []
            for ball_color in color_dist.keys():
                inRange_hsv = cv2.inRange(erode_hsv, color_dist[ball_color]['Lower'], color_dist[ball_color]['Upper'])
                box = cv2.findContours(inRange_hsv.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
                if box:
                    '''c = max(cnts, key=cv2.contourArea)
                    rect = cv2.minAreaRect(c)
                    box = cv2.boxPoints(rect)
                    cv2.drawContours(frame, [np.int0(box)], -1, color_line[ball_color], 2)'''
                    GPIO.output(color_PIN(ball_color), GPIO.HIGH)

                '''if hStack == []:
                    hStack = inRange_hsv
                else:
                    hStack = np.hstack([hStack, inRange_hsv])
            cv2.imshow('camera', frame)
            cv2.imshow('masks', hStack)
            cv2.waitKey(1)'''
cap.release()

'''gs_frame = cv2.GaussianBlur(frame, (5, 5), 0)  # 高斯模糊
hsv = cv2.cvtColor(gs_frame, cv2.COLOR_BGR2HSV)  # 转化成HSV图像
erode_hsv = cv2.erode(hsv, None, iterations=2)  # 腐蚀 粗的变细
hStack = []
for ball_color in color_dist.keys():
    inRange_hsv = cv2.inRange(erode_hsv, color_dist[ball_color]['Lower'], color_dist[ball_color]['Upper'])
    cnts = cv2.findContours(inRange_hsv.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
    if cnts:
        c = max(cnts, key=cv2.contourArea)
        rect = cv2.minAreaRect(c)
        box = cv2.boxPoints(rect)
        cv2.drawContours(frame, [np.int0(box)], -1, (0, 255, 255), 2)
        cv2.imshow('camera', frame)
        print(ball_color)

    if hStack == []:
        hStack = inRange_hsv
    else:
        hStack = np.hstack([hStack, inRange_hsv])

cv2.imshow('masks', hStack)

cv2.waitKey(0)
cv2.destroyAllWindows()'''
