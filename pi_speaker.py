import usb.core
import RPi.GPIO as GPIO
import pygame.mixer.music as mixer

class Speaker(object):
    def __init__(self):
        self.pin = 9
        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(self.pin, GPIO.IN)
        self.sounds = {
            0: "start.ogg",
            1: "shoot.ogg",
            2: "hit.ogg",
            3: "lose.ogg"
        }
        self.SEND_AUDIO = 0;
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('No USB device found matching idVendor and idProduct')
        self.dev.set_configuration()
        GPIO.add_event_detect(self.pin, GPIO.RISING, callback=self.receive_sound)

    def receive_sound(self):
        try:
            sound = self.dev.ctrl_transfer(0x40, self.SEND_AUDIO);
            if sound in self.sounds:
                mixer.queue(self.sounds[sound]);
        except:
            print("receiving falure")


def main():
    try:
        speaker = Speaker()
    except:
        GPIO.cleanup()

if __name__ == '__main__':
    main()
