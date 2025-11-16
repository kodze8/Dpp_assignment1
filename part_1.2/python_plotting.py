import pandas as pd
import matplotlib.pyplot as plt
import os

os.makedirs("./experiment_plots", exist_ok=True)

# ----------------- Experiment 1 -----------------
def experiment1_plot():
    df = pd.read_csv("results_ex1.csv")
    plt.figure(figsize=(10,6))

    plt.plot(df['i_max'], df['time_seconds'], marker='o', linestyle='-')

    plt.xscale('log')
    plt.xlabel("i_max (Number of Data Points)")
    plt.ylabel("Execution Time (seconds)")
    plt.title("Experiment 1: Execution Time for Fixed Threads (num_threads=4)")
    plt.grid(True)

    plt.savefig("./experiment_plots/experiment_1.png", dpi=200)
    plt.close()
    print("Experiment 1 plot saved")

# ----------------- Experiment 2 -----------------
def experiment2_plot():
    df = pd.read_csv("results_ex2.csv")
    plt.figure(figsize=(10,6))

    for sched in df['schedule'].unique():
        subset_sched = df[df['schedule'] == sched]
        plt.plot(subset_sched['i_max'], subset_sched['time_seconds'], marker='o', linestyle='-', label=f"Schedule={sched}")

    plt.xscale('log')  # log scale for i_max
    plt.xlabel("i_max (Number of Data Points)")
    plt.ylabel("Execution Time (seconds)")
    plt.title("Experiment 2: Scheduler Comparison (num_threads=4)")
    plt.legend()
    plt.grid(True)
    plt.savefig("./experiment_plots/experiment_2.png", dpi=200)
    plt.close()
    print("Experiment 2 plot saved")

# ----------------- Experiment 3 -----------------
def experiment3_plot():
    df = pd.read_csv("results_ex3.csv")
    plt.figure(figsize=(10,6))

    for sched in df['scheduler'].unique():
        subset_sched = df[df['scheduler'] == sched]
        plt.plot(subset_sched['num_threads'], subset_sched['time_seconds'],
                 marker='o', linestyle='-', label=f"Scheduler={sched}")

    plt.xlabel("Number of Threads")
    plt.ylabel("Execution Time (seconds)")
    plt.title("Experiment 3: Execution Time vs Number of Threads for Different Schedulers")
    plt.xticks(sorted(df['num_threads'].unique()))
    plt.grid(True)
    plt.legend()
    plt.savefig("./experiment_plots/experiment_3.png", dpi=200)
    plt.close()
    print("Experiment 3 plot saved")

# ----------------- Run all -----------------
if __name__ == "__main__":
    # experiment1_plot()
    experiment3_plot()
    # experiment3_plot()
