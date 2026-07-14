#!/usr/bin/env python3
from pathlib import Path
import argparse

from rosbags.highlevel import AnyReader
from rosbags.rosbag1 import Writer
from rosbags.typesys import Stores, get_typestore


def shift_stamp(stamp, shift_sec: float):
    shift_ns = int(round(shift_sec * 1e9))
    total_ns = stamp.sec * 1_000_000_000 + stamp.nanosec + shift_ns

    if total_ns < 0:
        raise ValueError("Shift made timestamp negative")

    stamp.sec = total_ns // 1_000_000_000
    stamp.nanosec = total_ns % 1_000_000_000
    return stamp


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input_bag")
    parser.add_argument("output_bag")
    parser.add_argument(
        "--image-topic",
        default="/lucid2/arena_camera_node/image_raw_synced",
    )
    parser.add_argument(
        "--shift",
        type=float,
        required=True,
        help="Seconds to add to image header.stamp. Use negative to shift earlier.",
    )
    args = parser.parse_args()

    input_bag = Path(args.input_bag)
    output_bag = Path(args.output_bag)

    typestore = get_typestore(Stores.ROS1_NOETIC)

    shifted = 0
    copied = 0

    with AnyReader([input_bag], default_typestore=typestore) as reader, Writer(output_bag) as writer:
        conn_map = {}

        for conn in reader.connections:
            conn_map[conn.id] = writer.add_connection(
                conn.topic,
                conn.msgtype,
                typestore=typestore,
            )

        for conn, timestamp, rawdata in reader.messages():
            out_conn = conn_map[conn.id]

            if conn.topic == args.image_topic:
                msg = reader.deserialize(rawdata, conn.msgtype)
                shift_stamp(msg.header.stamp, args.shift)

                rawdata = typestore.serialize_ros1(msg, conn.msgtype)
                shifted += 1
            else:
                copied += 1

            writer.write(out_conn, timestamp, rawdata)

    print("Done.")
    print(f"Shifted image messages: {shifted}")
    print(f"Copied other messages: {copied}")
    print(f"Output: {output_bag}")


if __name__ == "__main__":
    main()