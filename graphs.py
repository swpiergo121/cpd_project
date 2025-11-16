#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt

# Read the data from a CSV file
filename = "outputs/results1.csv"  # Replace with your actual filename
df = pd.read_csv(filename)

# Drop rows where 'secs_total' is NaN
df.dropna(subset=["secs_total"], inplace=True)
df.sort_values("number_threads", inplace=True)  # Ensure order

# Plotting
# Time vs threads
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
# plt.savefig("outputs/nthreads_vs_time.png")
# plt.show()


# Plotting
# GFLOPS vs threads
plt.figure(figsize=(10, 6))
plt.plot(df["number_threads"], df["gflops"], marker="o", linestyle="-", color="g")
plt.xscale("log", base=2)  # Log scale for threads (base 2)
plt.xlabel("Number of Threads (log scale)")
plt.ylabel("Total GFLOPS")
plt.title("Total GFLOPS Time vs Number of Threads")
plt.grid(True, which="both", ls="--", linewidth=0.5)
plt.xticks(
    df["number_threads"], labels=[str(int(x)) for x in df["number_threads"]]
)  # Show exact thread counts
plt.tight_layout()

# Show or save the plot
# plt.savefig("outputs/nthreads_vs_gflops.png")
plt.show()

# Plotting
# Speedup
# Get the execution time for 1 thread
T1 = df[df["number_threads"] == 1]["secs_total"]
if len(T1) == 0:
    raise ValueError("Data for 1 thread not found. Cannot compute speedup.")
T1 = T1[0]

# Compute speedup
df["speedup"] = T1 / df["secs_total"]

# Plot 2: Speedup
plt.subplot(1, 2, 2)
plt.plot(
    df["number_threads"],
    df["speedup"],
    marker="s",
    color="orange",
    label="Achieved Speedup",
)
plt.axline((1, 1), slope=1, color="red", linestyle="--", label="Ideal Speedup")
plt.xscale("log", base=2)
plt.xlabel("Number of Threads (log scale)")
plt.ylabel("Speedup")
plt.title("Speedup vs Number of Threads")
plt.legend()
plt.grid(True, which="both", ls="--")

# Adjust layout and display
plt.tight_layout()
plt.show()
