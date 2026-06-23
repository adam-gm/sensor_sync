

import rosbag
import numpy as np
import matplotlib.pyplot as plt

bag_path = "3_lidar_imu_sensorframe_outangle0.bag"
topic = "/synched/ouster/points"

timestamps = []

with rosbag.Bag(bag_path, "r") as bag:
    for _, msg, _ in bag.read_messages(topics=[topic]):
        timestamps.append(msg.header.stamp.to_sec())

timestamps = np.array(timestamps)

if len(timestamps) < 2:
    print("Not enough messages")
    exit()

dt = np.diff(timestamps)

print(f"Number of scans: {len(timestamps)}")
print(f"Mean dt: {np.mean(dt):.6f} s")
print(f"Std dt : {np.std(dt):.6f} s")
print(f"Min dt : {np.min(dt):.6f} s")
print(f"Max dt : {np.max(dt):.6f} s")

#bad_idx = np.where((dt < 0.05) | (dt > 0.15))[0]

#i = bad_idx[0]

#for k in range(i-3, i+5):
#    print(k, timestamps[k], "dt_prev=", timestamps[k] - timestamps[k-1] if k > 0 else None)
# Histogram
plt.figure(figsize=(10,5))
plt.hist(dt, bins=50)
plt.xlabel("Time between consecutive pointcloud timestamps [s]")
plt.ylabel("Count")
plt.title("Histogram of /synched/ouster/points timestamp differences")
plt.grid(True)

# dt over time
plt.figure(figsize=(10,5))
plt.plot(dt)
plt.axhline(np.mean(dt), linestyle="--")
plt.xlabel("Scan index")
plt.ylabel("Δt [s]")
plt.title("Consecutive pointcloud timestamp differences")
plt.grid(True)

plt.show()