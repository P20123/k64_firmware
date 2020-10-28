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


# trail
gx = []
gy = []
gz = []

ax = []
ay = []
az = []


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


def sphereFit(spX,spY,spZ):
    #   Assemble the A matrix
    spX = np.array(spX)
    spY = np.array(spY)
    spZ = np.array(spZ)
    A = np.zeros((len(spX),4))
    A[:,0] = spX*2
    A[:,1] = spY*2
    A[:,2] = spZ*2
    A[:,3] = 1

    #   Assemble the f matrix
    f = np.zeros((len(spX),1))
    f[:,0] = (spX*spX) + (spY*spY) + (spZ*spZ)
    C, residules, rank, singval = np.linalg.lstsq(A,f)

    #   solve for the radius
    t = (C[0]*C[0])+(C[1]*C[1])+(C[2]*C[2])+C[3]
    radius = np.sqrt(t)

    return radius, C[0], C[1], C[2]


def animate_gyro(i):
    global gx, gy, gz
    a, b, c, d = read_data()
    gx.append(a)
    gy.append(b)
    gz.append(c)
    gx = gx[-500:]
    gy = gy[-500:]
    gz = gz[-500:]

    r, x0, y0, z0 = sphereFit(gx, gy, gz)
    u, v = np.mgrid[0:2*np.pi:20j, 0:np.pi:10j]
    x = np.cos(u)*np.sin(v)*r
    y = np.sin(u)*np.sin(v)*r
    z = np.cos(v)*r
    x += x0
    y += y0
    z += z0

    # new vectors
    LIM = (-.2, .2)
    ax1.clear()
    ax1.scatter(gx, gy, zs=gz, color='b')
    ax1.scatter(x0, y0, zs=z0, color='r')
    ax1.plot_wireframe(x, y, z, color='r', alpha=.3)
    ax1.scatter(0, 0, zs=0, color='k')

    # format the plot
    ax1.set_title("x={:.3f}, y={:.3f}, z={:.3f}".format(x0[0], y0[0], z0[0]))
    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.set_zlabel('Z')
    ax1.set_xlim(LIM)
    ax1.set_ylim(LIM)
    ax1.set_zlim(LIM)


def animate_accel(i):
    global ax, ay, az
    a, b, c, d = read_data()

    ax.append(a)
    ay.append(b)
    az.append(c)
    ax = ax[-500:]
    ay = ay[-500:]
    az = az[-500:]

    avgx = np.mean(ax)
    avgy = np.mean(ay)
    avgz = np.mean(az)

    # new vectors
    LIM = (-1, 1)
    ax1.clear()
    ax1.scatter(ax, ay, zs=az, color='b')
    ax1.scatter(0, 0, zs=0, color='k')

    # format the plot
    ax1.set_title("x={:.3f}, y={:.3f}, z={:.3f}".format(avgx, avgy, avgz))
    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.set_zlabel('Z')
    ax1.set_xlim(LIM)
    ax1.set_ylim(LIM)
    ax1.set_zlim(LIM)


def main():
    serial_port.reset_input_buffer()
    ani = animation.FuncAnimation(fig, animate_accel, interval=15)
    plt.show()


if __name__ == "__main__":
    main()
