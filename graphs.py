#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt

# Read the data from a CSV file
filename = "outputs/results2.csv"  # Replace with your actual filename
df = pd.read_csv(filename)

# Drop rows where 'secs_total' is NaN
df.dropna(subset=["secs_total"], inplace=True)

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(df["number_threads"], df["secs_total"], marker="o", linestyle="-", color="b")
plt.xscale("log", base=2)  # Log scale for threads (base 2)
plt.xlabel("Number of Threads (log scale)")
plt.ylabel("Total Time (seconds)")
plt.title("Total Execution Time vs Number of Threads")
plt.grid(True, which="both", ls="--", linewidth=0.5)
plt.xticks(
    df["number_threads"], labels=[str(int(x)) for x in df["number_threads"]]
)  # Show exact thread counts
plt.tight_layout()

# Show or save the plot
plt.savefig("outputs/out.png")
# plt.show()
