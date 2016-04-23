import usb.core
import time
import RPi.GPIO as GPIO
import pygame

class Speaker(object):
    def __init__(self):
        pygame.init()
        pygame.mixer.init()
        self.mixer = pygame.mixer.music
        self.pin = 10
        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(self.pin, GPIO.IN)
        self.sounds = {
            0: "sounds/start.ogg",
            1: "sounds/shoot.ogg",
            2: "sounds/hit.ogg",
            3: "sounds/lose.ogg"
        }
        self.SEND_AUDIO = 0;
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('No USB device found matching idVendor and idProduct')
        self.dev.set_configuration()
        GPIO.add_event_detect(self.pin, GPIO.RISING, callback=self.receive_sound)
        if GPIO.input(self.pin):
            self.receive_sound(self.pin)

    def receive_sound(self, channel):
        try:
            sound = self.dev.ctrl_transfer(0x40, self.SEND_AUDIO)
            if sound in self.sounds:
                print("Playing:", self.sounds[sound])
                mixer.load(self.sounds[sound])
            else:
                print("Sound not Found")
        except:
            print("receiving falure")


def main():
    try:
        speaker = Speaker()
        while True:
            time.sleep(1);
    except:
        GPIO.cleanup()

if __name__ == '__main__':
    main()
