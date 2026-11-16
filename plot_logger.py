#!/usr/bin/env python3
"""Plot the Arduino logger CSV with interactive row and date selection. 
What could possibly go wrong?"""

import argparse
import csv
from datetime import datetime
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib.widgets import Button, RadioButtons, TextBox


def read_log(path):
    with path.open(newline="") as file:
        reader = csv.DictReader(file, skipinitialspace=True)
        expected = {"millis", "stamp", "date", "time", "light"}
        if reader.fieldnames is None or not expected.issubset(reader.fieldnames):
            raise ValueError("CSV must have millis, stamp, date, time and light columns")

        rows = []
        for line, row in enumerate(reader, start=2):
            try:
                rows.append({
                    "millis": float(row["millis"]),
                    "stamp": float(row["stamp"]),
                    "date/time": datetime.strptime(
                        f'{row["date"]} {row["time"]}', "%m/%d/%Y %H:%M:%S"
                    ),
                    "light": float(row["light"]),
                })
            except (TypeError, ValueError) as error:
                raise ValueError(f"Invalid data on CSV line {line}: {error}") from error
        if not rows:
            raise ValueError("CSV contains no data rows")
        return rows


def show_log(rows, title):
    fig, ax = plt.subplots(figsize=(12, 7))
    fig.subplots_adjust(left=0.25, bottom=0.31)
    ax_x = fig.add_axes([0.02, 0.55, 0.18, 0.18])
    ax_start = fig.add_axes([0.35, 0.17, 0.18, 0.05])
    ax_end = fig.add_axes([0.65, 0.17, 0.18, 0.05])
    ax_apply = fig.add_axes([0.86, 0.17, 0.12, 0.05])
    ax_date_start = fig.add_axes([0.35, 0.08, 0.18, 0.05])
    ax_date_end = fig.add_axes([0.65, 0.08, 0.18, 0.05])
    ax_apply_dates = fig.add_axes([0.86, 0.08, 0.12, 0.05])

    x_choices = RadioButtons(ax_x, ("date/time", "row"))
    start = TextBox(ax_start, "First row ", initial="1")
    end = TextBox(ax_end, "Last row ", initial=str(len(rows)))
    apply = Button(ax_apply, "Apply rows")
    date_format = "%Y-%m-%d %H:%M:%S"
    date_start = TextBox(
        ax_date_start, "From ", initial=min(row["date/time"] for row in rows).strftime(date_format)
    )
    date_end = TextBox(
        ax_date_end, "To ", initial=max(row["date/time"] for row in rows).strftime(date_format)
    )
    apply_dates = Button(ax_apply_dates, "Apply dates")
    ax_x.set_title("X axis")
    status = fig.text(0.25, 0.02, "", color="red")
    selected_rows = list(range(len(rows)))

    def redraw(_event=None):
        selection = [rows[index] for index in selected_rows]
        x_name = x_choices.value_selected
        x = (
            [index + 1 for index in selected_rows]
            if x_name == "row"
            else [row[x_name] for row in selection]
        )
        ax.clear()
        ax.plot(x, [row["light"] for row in selection], label="light")
        ax.set_title(title)
        ax.set_xlabel(x_name)
        ax.set_ylabel("Light (recorded value)")
        ax.grid(True)
        ax.legend()
        if x_name == "date/time":
            ax.tick_params(axis="x", labelrotation=25)
        fig.canvas.draw_idle()

    def apply_rows(_event):
        try:
            first = int(start.text)
            last = int(end.text)
            if not 1 <= first <= last <= len(rows):
                raise ValueError
        except ValueError:
            status.set_text(f"Enter a row range from 1 to {len(rows)}, inclusive.")
            fig.canvas.draw_idle()
            return
        selected_rows[:] = range(first - 1, last)
        status.set_text("")
        redraw()

    def apply_date_range(_event):
        try:
            first = datetime.strptime(date_start.text.strip(), date_format)
            last = datetime.strptime(date_end.text.strip(), date_format)
        except ValueError:
            status.set_text("Use date format YYYY-MM-DD HH:MM:SS.")
            fig.canvas.draw_idle()
            return
        if first > last:
            status.set_text("Start date/time must be before or equal to end date/time.")
            fig.canvas.draw_idle()
            return
        matches = [
            index for index, row in enumerate(rows)
            if first <= row["date/time"] <= last
        ]
        if not matches:
            status.set_text("No rows found in that date/time range.")
            fig.canvas.draw_idle()
            return
        selected_rows[:] = matches
        status.set_text("")
        redraw()

    x_choices.on_clicked(redraw)
    apply.on_clicked(apply_rows)
    apply_dates.on_clicked(apply_date_range)
    redraw()
    plt.show()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("file", nargs="?", type=Path, default=Path("LOGGER00.CSV"))
    args = parser.parse_args()
    try:
        rows = read_log(args.file)
    except (OSError, ValueError) as error:
        parser.exit(1, f"Cannot read {args.file}: {error}\n")
    show_log(rows, args.file.name)


if __name__ == "__main__":
    main()
