import serial
import struct
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from mpl_toolkits.mplot3d import Axes3D


# serial configuration
serial_port = serial.Serial(port="/dev/ttyACM0", baudrate=115200,
                            bytesize=serial.EIGHTBITS,
                            timeout=2, stopbits=serial.STOPBITS_ONE)


# struct configuration
imu_struct = struct.Struct('<fcfcfcfc')
STRUCT_LEN_BYTES = 20


# matplotlib configuration
fig = plt.figure()
ax1 = fig.add_subplot(111, projection='3d')


# data
q0 = 0
q1 = 0
q2 = 0
q3 = 0


def align_serial():
    while(True):
        if(serial_port.in_waiting > 0):
            data = serial_port.read(1)
            if(data == '\n'.encode('utf-8')):
                break


def read_data():
    global q0, q1, q2, q3
    if(serial_port.in_waiting < STRUCT_LEN_BYTES):
        print("empty")
        return [q0, q1, q2, q3]

    data = serial_port.read(STRUCT_LEN_BYTES)
    data = imu_struct.unpack(data)
    
    if data[-1] != '\n'.encode('utf-8'):
        serial_port.reset_input_buffer()
        align_serial()
        print("aligning:", data)
        return [q0, q1, q2, q3]

    q0 = data[0]
    q1 = data[2]
    q2 = data[4]
    q3 = data[6]

    return [q0, q1, q2, q3]


def animate(i):
    a, b, c, d = read_data()

    # new vectors
    LIM = (-2*np.pi, 2*np.pi)
    ax1.clear()
    ax1.scatter(a, b, zs=c, color='b')
    #ax1.scatter(accel_vec.x, accel_vec.y, zs=accel_vec.z, color='m')
    #ax1.scatter(mag_vec.x, mag_vec.y, zs=mag_vec.z, color='g')

    # format the plot
    ax1.set_title("x={:.3f}, y={:.3f}, z={:.3f}".format(a, b, c))
    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.set_zlabel('Z')
    ax1.set_xlim(LIM)
    ax1.set_ylim(LIM)
    ax1.set_zlim(LIM)


def main():
    serial_port.reset_input_buffer()
    ani = animation.FuncAnimation(fig, animate, interval=15)
    plt.show()


if __name__ == "__main__":
    main()
