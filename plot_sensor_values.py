#!/usr/bin/env python3
import argparse
import csv
import sys
from datetime import datetime, timezone
import matplotlib.pyplot as plt


def read_sun_sensors_csv(path):
    rows = []
    with open(path, newline="") as f:
        r = csv.reader(f, delimiter=";")
        for row in r:
            if not row or row[0].strip().startswith("#"):
                continue
            try:
                t = int(row[0].strip())
                sun_vector_x = float(row[1].strip())
                sun_vector_y = float(row[2].strip())
                sun_vector_z = float(row[3].strip())
                rows.append((t, sun_vector_x, sun_vector_y, sun_vector_z))
            except (ValueError, IndexError):
                continue
    rows.sort(key=lambda x: x[0])
    return rows


def plot_sun_sensors(rows, out_png=None, show=False):
    if not rows:
        print("No valid rows to plot", file=sys.stderr)
        return 1
    ts = [t for t, _, _, _ in rows]
    sun_vector_x = [c for _, c, _, _ in rows]
    sun_vector_y = [c for _, _, c, _ in rows]
    sun_vector_z = [c for _, _, _, c in rows]

    plt.figure(figsize=(10, 5))
    plt.plot(ts, sun_vector_x, label="sun_vector_x")
    plt.plot(ts, sun_vector_y, label="sun_vector_y")
    plt.plot(ts, sun_vector_z, label="sun_vector_z")
    plt.xlabel("rtc_s (s)")
    plt.ylabel("coordinate value")
    plt.title("Sun Vector Value")
    plt.legend()
    plt.grid(True, linestyle="--", alpha=0.4)
    plt.tight_layout()

    if out_png:
        plt.savefig(out_png, dpi=120)
        print(f"Saved {out_png}")
    if show or not out_png:
        plt.show()
    return 0


def main():
    ap = argparse.ArgumentParser(description="Plot sun vector CSV: rtc_s;sun_vector_x;sun_vector_y;sun_vector_z")
    ap.add_argument("csv", help="input CSV path")
    ap.add_argument("-o", "--out", help="output PNG path (if omitted, shows the plot)")
    ap.add_argument("--show", action="store_true", help="show an interactive window")
    args = ap.parse_args()

    rows = read_sun_sensors_csv(args.csv)
    sys.exit(plot_sun_sensors(rows, args.out, args.show))


if __name__ == "__main__":
    main()