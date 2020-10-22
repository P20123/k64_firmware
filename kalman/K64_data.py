import serial
import struct
import collections
import numpy as np

# serial configuration
serial_port = serial.Serial(port="/dev/ttyACM0", baudrate=115200,
                            bytesize=serial.EIGHTBITS,
                            timeout=2, stopbits=serial.STOPBITS_ONE)

# struct configuration
imu_struct = struct.Struct('<hhhhhhchhhchc')

# create named tuple
Vector = collections.namedtuple('Vector', 'x y z')

# 2G, 245DPS
DELTA_T = 0.258
ACC_INCR = 0.000061
GYRO_INCR = 0.00748
ACC_B = None
GYRO_B = None
MYCALC = True

if MYCALC:
    quat = np.array([1, 0, 0, 0])
else:
    quat = np.array([0, 0, 0])

# filtering!
gyro_hp = [Vector(0,0,0) * 10]

def align_serial():
    while(True):
        if(serial_port.in_waiting > 0):
            data = serial_port.read(1)
            if(data == '\n'.encode('utf-8')):
                break


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


def get_data(cur_attitude):
    global quat

    if(serial_port.in_waiting < 23):
#        print("empty")
        return quat

    data = serial_port.read(23)
    data = imu_struct.unpack(data)
    
    if data[-1] != '\n'.encode('utf-8'):
        serial_port.reset_input_buffer()
        align_serial()
#        print("aligning:", data)
        return quat

    # gyro vector! and NORMALIZE!!!!!!
    gyro_vec = Vector(data[0]*GYRO_INCR-GYRO_B.x, data[1]*GYRO_INCR-GYRO_B.y,
                      data[2]*GYRO_INCR-GYRO_B.z)
    gyro_norm = vector_norm(gyro_vec)
    gyro_vec = Vector(gyro_vec.x/gyro_norm, gyro_vec.y/gyro_norm, gyro_vec.z/gyro_norm)

    # accel vector!
    accel_vec = Vector(data[3]*ACC_INCR-ACC_B.x, data[4]*ACC_INCR-ACC_B.y,
                       data[5]*ACC_INCR-ACC_B.z)
    mag_vec = Vector(data[7], data[8], data[9])
    bar_vec = data[11]

    quat = calculate_rotation(gyro_vec, quat)

    return quat


def calculate_rotation(gyro_vec, cur_att):
    global gyro_hp

    gyro_hp.append(gyro_vec)
    gyro_hp = gyro_hp[-10:]
    gyro_hp_avg = [sum(ele) / len(gyro_hp) for ele in zip(*gyro_hp)]

    # high pass filtering
    gyro_vec = Vector(gyro_vec.x-gyro_hp_avg[0], gyro_vec.y-gyro_hp_avg[1],
                      gyro_vec.z-gyro_hp_avg[2])

    # integrating the gyro data
    if MYCALC:
        qgyro = np.array([0, gyro_vec.x * np.pi/180, gyro_vec.y * np.pi/180,
                      gyro_vec.z * np.pi/180])
        qgyro = DELTA_T * .5 * quaternion_multiply(cur_att, qgyro) 
        qgyro /= quaternion_norm(qgyro)
    else:
        qgyro = np.array([gyro_vec.x * np.pi/180, gyro_vec.y * np.pi/180,
                      gyro_vec.z * np.pi/180])

    return qgyro


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


def vector_norm(v):
    return np.sqrt(v.x**2 + v.y**2 + v.z**2)


def init_transfer():
    serial_port.reset_input_buffer()
    align_serial()
    find_bias()
