#!/usr/bin/env python3

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from rosbags.highlevel import AnyReader

BAG_PATH = Path("lucid2_data.bag")
IMAGE_TOPIC = "/lucid2/arena_camera_node/image_raw_synced"


def stamp_to_sec(stamp):
    return stamp.sec + stamp.nanosec * 1e-9


timestamps = []

with AnyReader([BAG_PATH]) as reader:
    connections = [c for c in reader.connections if c.topic == IMAGE_TOPIC]

    for connection, timestamp, rawdata in reader.messages(connections=connections):
        msg = reader.deserialize(rawdata, connection.msgtype)
        timestamps.append(stamp_to_sec(msg.header.stamp))

timestamps = np.array(timestamps)

if len(timestamps) < 2:
    raise RuntimeError("Need at least two images.")

dt = np.diff(timestamps)

print(f"Number of images : {len(timestamps)}")
print(f"Number of dt     : {len(dt)}")
print(f"Mean dt          : {np.mean(dt):.9f} s")
print(f"Std dt           : {np.std(dt):.9f} s")
print(f"Min dt           : {np.min(dt):.9f} s")
print(f"Max dt           : {np.max(dt):.9f} s")

# Finn de 10 største dt
largest_idx = np.argsort(dt)[-10:][::-1]

print("\n10 largest dt:")
for idx in largest_idx:
    print(
        f"Frame {idx} -> {idx+1}: "
        f"dt = {dt[idx]:.9f} s"
    )

dt_ms = dt * 1000

fig, ax = plt.subplots(figsize=(8, 5))
ax.hist(dt, bins=50)
ax.set_xlabel("Time between consecutive images [ms]")
ax.set_ylabel("Count")
ax.set_title("Histogram of image timestamp dt")
ax.ticklabel_format(useOffset=False)
ax.grid(True)

plt.show()