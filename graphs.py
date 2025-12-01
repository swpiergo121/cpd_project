#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt

# Read the data from a CSV file
fn_simple = f"outputs/results_simple.csv"  # Replace with your actual filename
fn_collapse = f"outputs/results_collapse.csv"  # Replace with your actual filename
out_graph = f"outputs/both/"
df_simple = pd.read_csv(fn_simple)

# Drop rows where 'secs_total' is NaN
df_simple.dropna(subset=["secs_total"], inplace=True)
df_simple.sort_values("number_threads", inplace=True)  # Ensure order

df_collapse = pd.read_csv(fn_collapse)

# Drop rows where 'secs_total' is NaN
df_collapse.dropna(subset=["secs_total"], inplace=True)
df_collapse.sort_values("number_threads", inplace=True)  # Ensure order


# Plotting
# Time vs threads
def time(df, model):
    plt.figure(figsize=(10, 6))
    plt.plot(
        df["number_threads"], df["secs_total"], marker="o", linestyle="-", color="b"
    )
    plt.xscale("log", base=2)  # Log scale for threads (base 2)
    plt.xlabel("Number of Threads (log scale)")
    plt.ylabel("Total Time (seconds)")
    plt.title(f"Total Execution Time vs Number of Threads for {model} model")
    plt.grid(True, which="both", ls="--", linewidth=0.5)
    plt.xticks(
        df["number_threads"], labels=[str(int(x)) for x in df["number_threads"]]
    )  # Show exact thread counts
    plt.tight_layout()

    # Show or save the plot
    plt.savefig(out_graph + "nthreads_vs_time.png")
    # plt.show()


# Plotting
# GFLOPS vs threads
def gflop(df, model):
    plt.figure(figsize=(10, 6))
    plt.plot(df["number_threads"], df["gflops"], marker="o", linestyle="-", color="g")
    plt.xscale("log", base=2)  # Log scale for threads (base 2)
    plt.xlabel("Number of Threads (log scale)")
    plt.ylabel("Total GFLOPS")
    plt.title(f"Total GFLOPS Time vs Number of Threads for {model} model")
    plt.grid(True, which="both", ls="--", linewidth=0.5)
    plt.xticks(
        df["number_threads"], labels=[str(int(x)) for x in df["number_threads"]]
    )  # Show exact thread counts
    plt.tight_layout()

    # Show or save the plot
    plt.savefig(out_graph + "nthreads_vs_gflops.png")
    # plt.show()


def gflop_2(df_simple, df_collapse, model):
    plt.figure(figsize=(10, 6))

    # Plot GFLOPS for Simple version
    plt.plot(
        df_simple["number_threads"],
        df_simple["gflops"],
        marker="o",
        linestyle="-",
        color="green",
        label="Simple Model",
    )

    # Plot GFLOPS for Collapse version
    plt.plot(
        df_collapse["number_threads"],
        df_collapse["gflops"],
        marker="s",
        linestyle="-",
        color="blue",
        label="Collapse Model",
    )

    plt.xscale("log", base=2)  # Log scale for threads (base 2)
    plt.xlabel("Number of Threads (log scale)")
    plt.ylabel("Total GFLOPS")
    plt.title(f"Total GFLOPS vs Number of Threads for {model} model")
    plt.legend()
    plt.grid(True, which="both", ls="--", linewidth=0.5)

    plt.tight_layout()

    # Save the plot
    # plt.savefig(out_graph + "nthreads_vs_gflops.png")
    plt.show()


# Plotting
# Speedup
# Get the execution time for 1 thread
def speedup(df, model):

    T1 = df[df["number_threads"] == 1]["secs_total"]
    if len(T1) == 0:
        raise ValueError("Data for 1 thread not found. Cannot compute speedup.")
    T1 = T1[0]

    # Compute speedup
    df["speedup"] = T1 / df["secs_total"]

    # Parameters
    n = 125_000_000
    M = 373_810
    p = df["number_threads"]  # Number of threads (varies per row)

    # Compute theoretical ideal speedup using given formula
    n_cubed = n**3
    speedup_ideal = n_cubed / (n_cubed / p + M)

    # Plot 2: Speedup
    plt.figure(figsize=(10, 6))
    plt.plot(
        df["number_threads"],
        df["speedup"],
        marker="s",
        color="orange",
        label="Achieved Speedup",
    )
    plt.plot(
        df["number_threads"],
        speedup_ideal,
        color="red",
        linestyle="--",
        label="Theoretical Ideal Speedup",
    )
    plt.xscale("log", base=2)
    plt.xlabel("Number of Threads (log scale)")
    plt.ylabel("Speedup")
    plt.title(f"Speedup vs Number of Threads for {model} model")
    plt.legend()
    plt.grid(True, which="both", ls="--")

    # Adjust layout and display
    plt.tight_layout()
    plt.savefig(out_graph + "nthreads_vs_speedup.png")
    # plt.show()


# Plotting
# Speedup
# Get the execution time for 1 thread
def speedup_2(df_simple, df_collapse, model):
    # Extract serial time from df_simple (assumes 1-thread run exists in df_simple)
    T1_simple = df_simple[df_simple["number_threads"] == 1]["secs_total"]
    T1_collapse = df_collapse[df_collapse["number_threads"] == 1]["secs_total"]
    if len(T1_simple) == 0:
        raise ValueError(
            "Data for 1 thread not found in df_simple. Cannot compute speedup."
        )
    T1_simple = T1_simple.iloc[0]  # Use .iloc[0] for safety
    T1_collapse = T1_collapse.iloc[0]  # Use .iloc[0] for safety

    # Compute achieved speedup for both dataframes
    df_simple = df_simple.copy()
    df_collapse = df_collapse.copy()

    df_simple["speedup"] = T1_simple / df_simple["secs_total"]
    df_collapse["speedup"] = T1_collapse / df_collapse["secs_total"]

    # Parameters for theoretical ideal speedup
    n = 125_000_000
    M = 373_810
    p_simple = df_simple["number_threads"]

    # Compute theoretical ideal speedup
    n_cubed = n**3
    speedup_ideal_simple = n_cubed / (n_cubed / p_simple + M)

    # Plot: Speedup vs Number of Threads
    plt.figure(figsize=(10, 6))

    # Plot both achieved speedups
    plt.plot(
        df_simple["number_threads"],
        df_simple["speedup"],
        marker="s",
        color="orange",
        label="Achieved Speedup (Simple)",
    )
    plt.plot(
        df_collapse["number_threads"],
        df_collapse["speedup"],
        marker="o",
        color="blue",
        label="Achieved Speedup (Collapse)",
    )

    # Plot theoretical ideal speedup (use p_simple for x-axis or combine both)
    # Using simple's thread counts for ideal curve (assuming same p values)
    plt.plot(
        df_simple["number_threads"],
        speedup_ideal_simple,
        color="red",
        linestyle="--",
        label="Theoretical Ideal Speedup",
    )

    plt.xscale("log", base=2)
    plt.xlabel("Number of Threads (log scale)")
    plt.ylabel("Speedup")
    plt.title(f"Speedup vs Number of Threads for {model} models")
    plt.legend()
    plt.grid(True, which="both", ls="--")

    # Adjust layout and save
    plt.tight_layout()
    plt.savefig(out_graph + "nthreads_vs_speedup.png")
    # plt.show()


gflop_2(df_simple, df_collapse, "both")
speedup_2(df_simple, df_collapse, "both")
