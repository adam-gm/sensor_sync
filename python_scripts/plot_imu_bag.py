from rosbags.highlevel import AnyReader
from pathlib import Path
import matplotlib.pyplot as plt
import math

bagpath = Path("3_lidar_imu_lidarframe_outangle0.bag")
imu_topic = "/senti/senti/imu"   # endre til riktig topic

t = []
ax, ay, az = [], [], []
gx, gy, gz = [], [], []

with AnyReader([bagpath]) as reader:
    conns = [c for c in reader.connections if c.topic == imu_topic]

    for conn, timestamp, rawdata in reader.messages(connections=conns):
        msg = reader.deserialize(rawdata, conn.msgtype)

        # Bruk bag timestamp som tid
        time_sec = timestamp * 1e-9

        t.append(time_sec)

        ax.append(msg.linear_acceleration.x)
        ay.append(msg.linear_acceleration.y)
        az.append(msg.linear_acceleration.z)

        gx.append(msg.angular_velocity.x)
        gy.append(msg.angular_velocity.y)
        gz.append(msg.angular_velocity.z)

# gjør tid relativ til første sample
t0 = t[0]
t = [ti - t0 for ti in t]

acc_norm = [math.sqrt(x*x + y*y + z*z) for x, y, z in zip(ax, ay, az)]

plt.figure()
plt.plot(t, ax, label="acc x")
plt.plot(t, ay, label="acc y")
plt.plot(t, az, label="acc z")
#plt.plot(t, acc_norm, label="acc norm", linestyle="--")
plt.xlabel("time [s]")
plt.ylabel("acceleration [m/s^2]")
plt.legend()
plt.grid()
plt.show()

plt.figure()
plt.plot(t, gx, label="gyro x")
plt.plot(t, gy, label="gyro y")
plt.plot(t, gz, label="gyro z")
plt.xlabel("time [s]")
plt.ylabel("angular velocity [rad/s]")
plt.legend()
plt.grid()
plt.show()