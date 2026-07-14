from rosbags.highlevel import AnyReader
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

bag_path = Path("nyestelidar_logikk_13juli_runde2.bag")
topic_name = "/synched/ouster/points"

timestamps = []

with AnyReader([bag_path]) as reader:
    conns = [c for c in reader.connections if c.topic == topic_name]

    if not conns:
        raise RuntimeError(f"Topic not found: {topic_name}")

    for conn, _, rawdata in reader.messages(connections=conns):
        msg = reader.deserialize(rawdata, conn.msgtype)
        stamp = msg.header.stamp.sec + msg.header.stamp.nanosec * 1e-9
        timestamps.append(stamp)

timestamps = np.array(timestamps)

if len(timestamps) < 2:
    raise RuntimeError("Not enough messages")

dt = np.diff(timestamps)

print(f"Number of scans: {len(timestamps)}")
print(f"Mean dt: {np.mean(dt):.6f} s")
print(f"Std dt : {np.std(dt):.6f} s")
print(f"Min dt : {np.min(dt):.6f} s")
print(f"Max dt : {np.max(dt):.6f} s")

bad_idx = np.where((dt < 0.05) | (dt > 0.15))[0]
print("Bad indices:", bad_idx)
print("Bad dt:", dt[bad_idx])

if len(bad_idx) > 0:
    i = bad_idx[0]
    for k in range(max(1, i - 3), min(len(timestamps), i + 5)):
        print(k, timestamps[k], "dt_prev=", timestamps[k] - timestamps[k - 1])

plt.figure(figsize=(10, 5))
plt.hist(dt, bins=50)
plt.xlabel("Time between consecutive pointcloud timestamps [s]")
plt.ylabel("Count")
plt.title(f"Histogram of {topic_name} timestamp differences")
plt.grid(True)

plt.figure(figsize=(10, 5))
plt.plot(dt)
plt.axhline(np.mean(dt), linestyle="--")
plt.xlabel("Scan index")
plt.ylabel("Δt [s]")
plt.title("Consecutive pointcloud timestamp differences")
plt.grid(True)

plt.show()
