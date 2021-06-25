
import cv2
import numpy as np


from PIL import Image
import matplotlib.pyplot as plt



image = cv2.imdecode(np.fromfile('test1.jpg', dtype=np.uint8), -1)
image_gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

'''for n in range(1,8):
    image_canny = cv2.Canny(image_gray,140,210,n)
    cv2.imshow("test_%d.jpg"%n,image_canny)
    cv2.waitKey()

cv2.waitKey()'''
# test for threshold
#image_canny = cv2.Canny(image_gray, 110,150,3)
#image_canny = cv2.Canny(image_gray, 140, 210, 3)
#image_canny = cv2.Canny(image_gray, 180, 200, 3)

for n in range(30, 40):
    m = 10
    image_canny = cv2.Canny(image_gray, 390, 430, 3)
    img_blur = cv2.blur(image_canny, (17, 17))
    retval, dst = cv2.threshold(img_blur, 50, 255, cv2.THRESH_BINARY)
    cv2.imshow("test_%d.jpg"%n,dst)
    cv2.waitKey()

cv2.waitKey()

#img_blur = cv2.blur(image_canny, (15, 15))
#img_blur = cv2.blur(image_canny, (10, 10))
#img_blur = cv2.blur(image_canny, (18, 18))

#retval, dst = cv2.threshold(img_blur, 50, 255, cv2.THRESH_BINARY)

direction = 640  # center of the width
for n in range(1, 11):
    color = dst[721 - n * 72]
    center = 640
    white_count = np.sum(color == 255)
    white_index = np.where(color == 255)
    if white_count != 0:
        center = (white_index[0][white_count - 1] + white_index[0][0]) / 2
    direction = 0.6 * direction + 0.4 * center
    gap = direction - 640
    print(center, direction)

print('OK')

cv2.imwrite("test2.jpg", dst)