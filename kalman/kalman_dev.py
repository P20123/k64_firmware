import serial
import struct
import collections
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import datetime as dt
from matplotlib import style
from mpl_toolkits.mplot3d import Axes3D

# serial configuration
serial_port = serial.Serial(port="/dev/ttyACM0", baudrate=115200,
                            bytesize=serial.EIGHTBITS,
                            timeout=2, stopbits=serial.STOPBITS_ONE)

def align_serial():
    while(True):
        if(serial_port.in_waiting > 0):
            data = serial_port.read(1)
            if(data == '\n'.encode('utf-8')):
                break

# struct configuration
imu_struct = struct.Struct('<hhhhhhchhhchc')

# create named tuple
Vector = collections.namedtuple('Vector', 'x y z')

# matplotlib configuration
fig = plt.figure()
ax1 = fig.add_subplot(111, projection='3d')

# positions
qpos = np.array([1.0, 0.0, 0.0, 0.0])
gyro_hp = Vector(0, 0, 0)
DELTA_T = 0.258

# 2G, 245DPS
ACC_INCR = 0.000061
GYRO_INCR = 0.00748
ACC_B = None
GYRO_B = None


def find_bias():
    global ACC_B
    global GYRO_B

    gyro_list = []
    accel_list = []

    while(True):
        if(serial_port.in_waiting < 23):
            continue

        data = serial_port.read(23)
        data = imu_struct.unpack(data)
        
        if data[-1] != '\n'.encode('utf-8'):
            serial_port.reset_input_buffer()
            align_serial()
            continue
    
        gyro_list.append(Vector(data[0]*GYRO_INCR, data[1]*GYRO_INCR, data[2]*GYRO_INCR))
        accel_list.append(Vector(data[3]*ACC_INCR, data[4]*ACC_INCR, data[5]*ACC_INCR))
        
        if len(gyro_list) == 100:
            break
    
    gyro_avg = [sum(ele) / len(gyro_list) for ele in zip(*gyro_list)]
    accel_avg = [sum(ele) / len(accel_list) for ele in zip(*accel_list)]
    
    GYRO_B = Vector(gyro_avg[0], gyro_avg[1], gyro_avg[2])
    ACC_B = Vector(accel_avg[0], accel_avg[1], accel_avg[2])

    print("Biases:", GYRO_B, ACC_B)


def quaternion_multiply(q, p):
    w = q[0]*p[0] - q[1]*p[1] - q[2]*p[2] - q[3]*p[3]
    x = q[0]*p[1] + q[1]*p[0] + q[2]*p[3] - q[3]*p[2]
    y = q[0]*p[2] - q[1]*p[3] + q[2]*p[0] + q[3]*p[1]
    z = q[0]*p[3] + q[1]*p[2] - q[2]*p[1] + q[3]*p[0]
    return np.array([w, x, y, z])


def quaternion_norm(q):
    return np.sqrt(q[0]**2 + q[1]**2 + q[2]**2 + q[3]**2)


def quaternion_conjugate(q):
    return np.array([q[0], -q[1], -q[2], -q[3]])


def animate(i):
    global qpos
    global gyro_hp

    gyro_vec = Vector(0, 0, 0)
    accel_vec = Vector(0, 0, 0)
    mag_vec = Vector(0, 0, 0)
    bar_vec = 0

    if(serial_port.in_waiting < 23):
        print("empty")
        return

    data = serial_port.read(23)
    data = imu_struct.unpack(data)
    
    if data[-1] != '\n'.encode('utf-8'):
        serial_port.reset_input_buffer()
        align_serial()
        print("aligning:", data)
        return

    # new vectors
    gyro_vec = Vector(data[0]*GYRO_INCR-GYRO_B.x, data[1]*GYRO_INCR-GYRO_B.y,
                      data[2]*GYRO_INCR-GYRO_B.z)
    accel_vec = Vector(data[3]*ACC_INCR-ACC_B.x, data[4]*ACC_INCR-ACC_B.y,
                       data[5]*ACC_INCR-ACC_B.z)
    mag_vec = Vector(data[7], data[8], data[9])
    bar_vec = data[11]

    PLOT_RAW_DATA = False
    # do the plotting
    ax1.clear()
    if PLOT_RAW_DATA:
        LIM = (-100, 100)
        ax1.scatter(gyro_vec.x, gyro_vec.y, zs=gyro_vec.z, color='b')
        ax1.scatter(accel_vec.x, accel_vec.y, zs=accel_vec.z, color='m')
        ax1.scatter(mag_vec.x, mag_vec.y, zs=mag_vec.z, color='g')
    else:
        LIM = (-1, 1)
        
        # high pass filtering
        gyro_hp = Vector((gyro_hp.x + gyro_vec.x) / 2, (gyro_hp.y + gyro_vec.y) / 2,
                         (gyro_hp.z + gyro_vec.z) / 2)
        gyro_vec = Vector(gyro_vec.x-gyro_hp.x, gyro_vec.y-gyro_hp.y,
                          gyro_vec.z-gyro_hp.z)

        # integrating the gyro data
        qgyro = np.array([0, gyro_vec.x * np.pi/180, gyro_vec.y * np.pi/180,
                          gyro_vec.z * np.pi/180])
        dqdt = .5 * quaternion_multiply(qpos, qgyro)
        qpos += DELTA_T * dqdt
        qpos /= quaternion_norm(qpos)

        temp = quaternion_multiply(qpos, np.array([1, 1, 0, 0]))
        temp = quaternion_multiply(temp, quaternion_conjugate(qpos))
        ax1.scatter(temp[1], temp[2], zs=temp[3], color='r')
        ax1.scatter(0, 0, zs=0, color='k')

    # format the plot
    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.set_zlabel('Z')
    ax1.set_xlim(LIM)
    ax1.set_ylim(LIM)
    ax1.set_zlim(LIM)


def main():
    serial_port.reset_input_buffer()
    align_serial()
    find_bias()
    ani = animation.FuncAnimation(fig, animate, interval=15)
    plt.show()


if __name__ == "__main__":
    main()
