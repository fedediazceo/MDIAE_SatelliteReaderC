#!/usr/bin/env python3
import argparse
import csv
import sys
from datetime import datetime, timezone
import matplotlib.pyplot as plt


def read_thermal_csv(path):
    rows = []
    with open(path, newline="") as f:
        r = csv.reader(f, delimiter=";")
        for row in r:
            if not row or row[0].strip().startswith("#"):
                continue
            try:
                t = int(row[0].strip())
                cpu = float(row[1].strip())
                mir = float(row[2].strip())
                rows.append((t, cpu, mir))
            except (ValueError, IndexError):
                continue
    rows.sort(key=lambda x: x[0])
    return rows


def plot_thermal(rows, out_png=None, show=False):
    if not rows:
        print("No valid rows to plot", file=sys.stderr)
        return 1
    ts = [t for t, _, _ in rows]
    cpu = [c for _, c, _ in rows]
    mir = [m for _, _, m in rows]

    plt.figure(figsize=(10, 5))
    plt.plot(ts, cpu, label="CPU_C")
    plt.plot(ts, mir, label="mirror_cell_C")
    plt.xlabel("rtc_s (s)")
    plt.ylabel("Temperature (C)")
    plt.title("Thermal telemetry")
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
    ap = argparse.ArgumentParser(description="Plot thermal CSV: rtc_s;CPU_C;mirror_cell_C")
    ap.add_argument("csv", help="input CSV path")
    ap.add_argument("-o", "--out", help="output PNG path (if omitted, shows the plot)")
    ap.add_argument("--show", action="store_true", help="show an interactive window")
    args = ap.parse_args()

    rows = read_thermal_csv(args.csv)
    sys.exit(plot_thermal(rows, args.out, args.show))


if __name__ == "__main__":
    main()