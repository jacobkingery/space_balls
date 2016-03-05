import usb.core
import time
import csv
import os.path
import sys
import matplotlib.pyplot as plt

class IRSensor:
    def __init__(self, fname=None, prompt_overwrite=True):
        self.GET_DISTANCE   = 1

        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

        self.field_names = ['Time', 'Distance']

        self.colors = ['b']
        plt.ion()

        self.fname = fname
        if self.fname:
            if prompt_overwrite and os.path.isfile(self.fname):
                raw_input('{} already exists, press Ctrl-C now to quit or Enter to overwrite.')
            with open(self.fname, 'w') as f:
                csv.DictWriter(f, fieldnames=self.field_names).writeheader()

        self.inital_time = time.time()

    def close(self):
        self.dev = None

    def toWord(self, byteArray):
        val = 0
        for i,byte in enumerate(byteArray):
            val += int(byte) * 2**(8*i)
        return val

    def get_distance(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.GET_DISTANCE, 0, 0, 2)
        except usb.core.USBError:
            print "Could not send GET_DISTANCE vendor request."
        else:
            return ret

    def get_readings(self):
        now = time.time() - self.inital_time
        distance = self.toWord(self.get_distance())

        readings = [now, distance]
        return dict(zip(self.field_names, readings))

    def write_readings(self, readings):
        if self.fname:
            with open(self.fname, 'a') as f:
                csv.DictWriter(f, fieldnames=self.field_names).writerows(readings)

    def plot_readings(self, readings):
        for i,key in enumerate(self.field_names[1:]):
            plt.scatter(readings['Time'], readings[key] * 0.5 if key == 'Motor_velocity' else readings[key], color=self.colors[i])
        plt.legend(self.field_names[1:], loc='lower center')
        plt.xlabel('Time (s)')
        plt.ylabel('Value')
        plt.ylim((0, 70000))
        plt.pause(0.01)

    def calibrate_sensor(self):
        fieldnames = ['Distance_(in)', 'Reading']
        readings = []
        with open('sensor_calibration.csv', 'w') as f:
            writer = csv.writer(f)
            writer.writerow(fieldnames)
            for i in range(4, 44, 4):
                raw_input('Place object {}" in front of sensor, then press Enter'.format(i))
                for j in range(20):
                    readings.append([i, self.toWord(self.get_distance())])
                    writer.writerow(readings[-1])
        print 'Wrote readings to sensor_calibration.csv'
        return readings

    def plot_calibration(self, readings):
        plt.ioff()
        for r in readings:
            plt.scatter(r[0], r[1], color='b')
        plt.xlim(0, 44)
        plt.ylim(0, 70000)
        plt.title('Reading vs Distance at Vcc = 3.3V')
        plt.xlabel('Distance (inches)')
        plt.ylabel('Reading')
        plt.show()

try:
    fname = sys.argv[1]
except IndexError:
    fname = None

ir = IRSensor(fname)

# readings = ir.calibrate_sensor()
# ir.plot_calibration(readings)

i = 0
readings = []
while True:
    i += 1
    r = ir.get_readings()

#     # Plotting the readings slows things down significantly,
#     # but can be helpful to look at when not capturing data.
    # ir.plot_readings(r)

    readings.append(r)
    if i > 50:
        ir.write_readings(readings)
        i = 0
        readings = []
