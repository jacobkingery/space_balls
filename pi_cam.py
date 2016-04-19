import io
import usb.core
import time
import picamera
import picamera.array
import numpy as np
import RPi.GPIO as GPIO
from collections import deque

class Camera(object):
    def __init__(self):
        self.pin = 8
        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(self.pin, GPIO.OUT)

    def is_yellow(self, rgb):
        # without bool cast, it returns a numpy boolean which isn't recognized by RPi.GPIO
        return bool(rgb[0] > 25.0 and rgb[1] > 25.0 and rgb[2] < 28.0)

    def process(self, img):
        cropped = img[40:80,60:100,:]
        avg = cropped.reshape((1600, 1, 3)).mean(axis=0).flatten()
        has_ball = self.is_yellow(avg)
        GPIO.output(self.pin, has_ball)
        return has_ball

    def sense(self):
        with picamera.PiCamera() as camera:
            camera.resolution = (160, 120)
            camera.shutter_speed = 30000
            camera.exposure_mode = 'off'
            camera.awb_mode = 'off'
            camera.awb_gains = (1.1, 1.8)
            camera.iso = 200

            with picamera.array.PiRGBArray(camera) as stream:
                times = deque(maxlen=20)
                start = time.time()
                for _ in camera.capture_continuous(stream, format='rgb', use_video_port=True):
                    img = stream.array
                    times.append(time.time() - start)
                    self.process(img)
                    # print 1.0 / np.mean(times)
                    # reset the stream before the next capture
                    stream.seek(0)
                    stream.truncate()
                    start = time.time()

class Speaker(object):
    def __init__(self):
        self.pin = 9
        GPIO.setup(self.pin, GPIO.IN)
    def receive_sound(self):
        sel

def main():
    try:
        camera = Camera()
        camera.sense()
    except:
        GPIO.cleanup()

if __name__ == '__main__':
    main()
