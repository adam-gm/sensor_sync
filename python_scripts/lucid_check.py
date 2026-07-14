#!/usr/bin/env python3

from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np
from rosbags.highlevel import AnyReader

BAG_PATH = Path("small_lucid_test_from_start_3.bag")

LUCID1_TOPIC = "/lucid1/arena_camera_node/image_raw_synced"
LUCID2_TOPIC = "/lucid2/arena_camera_node/image_raw_synced"


def stamp_to_sec(stamp):
    return stamp.sec + stamp.nanosec * 1e-9


def print_dt_stats(name, times):
    times = np.array(times)

    if len(times) < 2:
        print(f"{name}: not enough images")
        return None

    dt = np.diff(times)

    print(f"\n{name}")
    print(f"Number of images : {len(times)}")
    print(f"Mean dt          : {np.mean(dt):.9f} s")
    print(f"Std dt           : {np.std(dt):.9f} s")
    print(f"Min dt           : {np.min(dt):.9f} s")
    print(f"Max dt           : {np.max(dt):.9f} s")

    small_idx = np.where(dt < 0.09)[0]
    print("Small dt count:", len(small_idx))

    for i in small_idx[:20]:
        print(f"Frame {i}->{i+1}: dt={dt[i]:.9f}")
        print(f"  t[i]   = {times[i]:.9f}")
        print(f"  t[i+1] = {times[i+1]:.9f}")

    return dt


lucid1_times = []
lucid2_times = []

with AnyReader([BAG_PATH]) as reader:
    topics = {LUCID1_TOPIC, LUCID2_TOPIC}
    connections = [c for c in reader.connections if c.topic in topics]

    for connection, timestamp, rawdata in reader.messages(connections=connections):
        msg = reader.deserialize(rawdata, connection.msgtype)
        t = stamp_to_sec(msg.header.stamp)

        if connection.topic == LUCID1_TOPIC:
            lucid1_times.append(t)
        elif connection.topic == LUCID2_TOPIC:
            lucid2_times.append(t)


dt1 = print_dt_stats("Lucid1", lucid1_times)
dt2 = print_dt_stats("Lucid2", lucid2_times)

n = min(len(lucid1_times), len(lucid2_times))
lucid1_arr = np.array(lucid1_times[:n])
lucid2_arr = np.array(lucid2_times[:n])

cam_dt = lucid2_arr - lucid1_arr

print("\nLucid2 - Lucid1 timestamp difference")
print(f"Compared pairs : {n}")
print(f"Mean           : {np.mean(cam_dt):.9f} s")
print(f"Std            : {np.std(cam_dt):.9f} s")
print(f"Min            : {np.min(cam_dt):.9f} s")
print(f"Max            : {np.max(cam_dt):.9f} s")

print("\nFirst 20 pairs:")
for i in range(min(20, n)):
    print(
        f"{i}: "
        f"lucid1={lucid1_arr[i]:.9f}, "
        f"lucid2={lucid2_arr[i]:.9f}, "
        f"diff={cam_dt[i]:.9f}"
    )

bad = np.where(np.abs(cam_dt) > 0.01)[0]
print(f"\nPairs with |lucid2-lucid1| > 10 ms: {len(bad)}")
for i in bad[:20]:
    print(
        f"{i}: lucid1={lucid1_arr[i]:.9f}, "
        f"lucid2={lucid2_arr[i]:.9f}, "
        f"diff={cam_dt[i]:.9f}"
    )

plt.figure(figsize=(8, 5))
plt.hist(cam_dt * 1000, bins=50)
plt.xlabel("Lucid2 - Lucid1 timestamp difference [ms]")
plt.ylabel("Count")
plt.title("Stereo timestamp difference")
plt.grid(True)
plt.show()

from bisect import bisect_left

l1 = np.array(lucid1_times)
l2 = np.array(lucid2_times)

diffs = []

for t1 in l1:
    idx = bisect_left(l2, t1)

    candidates = []
    if idx > 0:
        candidates.append(l2[idx - 1])
    if idx < len(l2):
        candidates.append(l2[idx])

    if candidates:
        nearest = min(candidates, key=lambda t2: abs(t2 - t1))
        diffs.append(nearest - t1)

diffs = np.array(diffs)

good = diffs[np.abs(diffs) < 0.01]

print("\nGood stereo matches only, |diff| < 10 ms")
print(f"Good count      : {len(good)}")
print(f"Mean            : {np.mean(good):.9f} s")
print(f"Std             : {np.std(good):.9f} s")
print(f"Max abs         : {np.max(np.abs(good)):.9f} s")

print("\nFirst 10 timestamps:")
for i in range(min(100, len(lucid1_times), len(lucid2_times))):
    print(
        f"{i}: "
        f"lucid1={lucid1_times[i]:.9f}, "
        f"lucid2={lucid2_times[i]:.9f}, "
        f"diff={lucid2_times[i] - lucid1_times[i]:.9f}"
    )

print("\nLast 10 Lucid1 timestamps:")
for i in range(max(0, len(lucid1_times) - 10), len(lucid1_times)):
    print(f"{i}: lucid1={lucid1_times[i]:.9f}")

print("\nLast 10 Lucid2 timestamps:")
for i in range(max(0, len(lucid2_times) - 10), len(lucid2_times)):
    print(f"{i}: lucid2={lucid2_times[i]:.9f}")

print("\nLast 10 nearest matches:")
for i in range(max(0, len(lucid1_times) - 10), len(lucid1_times)):
    t1 = lucid1_times[i]
    idx = np.searchsorted(lucid2_times, t1)

    candidates = []
    if idx > 0:
        candidates.append(lucid2_times[idx - 1])
    if idx < len(lucid2_times):
        candidates.append(lucid2_times[idx])

    nearest = min(candidates, key=lambda t2: abs(t2 - t1))
    print(
        f"{i}: lucid1={t1:.9f}, "
        f"nearest_lucid2={nearest:.9f}, "
        f"diff={nearest - t1:.9f}"
    )

print("\nNearest Lucid2 - Lucid1 timestamp difference")
print(f"Compared pairs : {len(diffs)}")
print(f"Mean           : {np.mean(diffs):.9f} s")
print(f"Std            : {np.std(diffs):.9f} s")
print(f"Min            : {np.min(diffs):.9f} s")
print(f"Max            : {np.max(diffs):.9f} s")
print(f"Bad > 10 ms    : {np.sum(np.abs(diffs) > 0.01)}")

bad = np.where(np.abs(diffs) > 0.01)[0]
for i in bad:
    print(i, lucid1_times[i], diffs[i])
