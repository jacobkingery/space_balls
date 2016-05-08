import usb.core
import time
import RPi.GPIO as GPIO
import pygame
import random

class Speaker(object):
    def __init__(self):
        pygame.mixer.init(44100)
        self.pin = 10
        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(self.pin, GPIO.IN)
        self.sounds = {
            0: [pygame.mixer.Sound('/home/pi/space_balls/sounds/start.ogg')],
            1: [pygame.mixer.Sound('/home/pi/space_balls/sounds/shoot.ogg')],
            2: [pygame.mixer.Sound('/home/pi/space_balls/sounds/hit.ogg')],
            3: [pygame.mixer.Sound('/home/pi/space_balls/sounds/lose_evil.ogg'),
                pygame.mixer.Sound('/home/pi/space_balls/sounds/lose_cmon.ogg'),
                pygame.mixer.Sound('/home/pi/space_balls/sounds/lose_blimp.ogg'),
                pygame.mixer.Sound('/home/pi/space_balls/sounds/lose_friends.ogg'),
                pygame.mixer.Sound('/home/pi/space_balls/sounds/lose_goof.ogg'),
                pygame.mixer.Sound('/home/pi/space_balls/sounds/lose_gunner.ogg')]
        }
        self.SEND_SOUND = 0;
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('No USB device found matching idVendor and idProduct')
        self.dev.set_configuration()
        GPIO.add_event_detect(self.pin, GPIO.RISING, callback=self.receive_sound)
        if GPIO.input(self.pin):
            self.receive_sound(self.pin)

    def receive_sound(self, channel):
        sound = self.dev.ctrl_transfer(0xC0, self.SEND_SOUND, 0, 0, 1)[0]
        if sound in self.sounds:
            print 'Playing:', sound
            random.choice(self.sounds[sound]).play()
        else:
            print 'Sound not found'


def main():
    while True:
        try:
            speaker = Speaker()
            while True:
                time.sleep(1);
        except KeyboardInterrupt:
            exit(0)
        except:
            print 'Error, restarting'
            GPIO.cleanup()

if __name__ == '__main__':
    time.sleep(1);
    main()
