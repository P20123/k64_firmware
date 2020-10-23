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

def MahonyAHRSupdateIMU(gyro_vec, accel_vec):
	float recipNorm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;        

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = q1 * q3 - q0 * q2;
		halfvy = q0 * q1 + q2 * q3;
		halfvz = q0 * q0 - 0.5f + q3 * q3;
	
		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled
		if(twoKi > 0.0f) {
			integralFBx += twoKi * halfex * (1.0f / sampleFreq);	// integral error scaled by Ki
			integralFBy += twoKi * halfey * (1.0f / sampleFreq);
			integralFBz += twoKi * halfez * (1.0f / sampleFreq);
			gx += integralFBx;	// apply integral feedback
			gy += integralFBy;
			gz += integralFBz;
		}
		else {
			integralFBx = 0.0f;	// prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += twoKp * halfex;
		gy += twoKp * halfey;
		gz += twoKp * halfez;
	}
	
	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / sampleFreq));		// pre-multiply common factors
	gy *= (0.5f * (1.0f / sampleFreq));
	gz *= (0.5f * (1.0f / sampleFreq));
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx); 
	
	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}

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
