import pandas as pd
import matplotlib.pyplot as plt

# Experiment 1 Results
def experiment1_plot():
    df = pd.read_csv("results_ex1.csv")  # or "results_ex1.csv"
    i_max_values = df['i_max'].unique()

    plt.figure(figsize=(10,6))
    for i_max in i_max_values:
        subset = df[df['i_max'] == i_max]
        plt.plot(subset['simulate_version'], subset['time_seconds'], marker='o', label=f"i_max={i_max}")

    plt.xticks([1, 2, 3], ['Sequential Simulation', 'Parallel Simulation, No Barrier', 'Parallel Simulation with Barriers'])
    plt.xlabel("Simulation Version")
    plt.ylabel("Time (Seconds)")
    plt.title("Wave Simulation Execution Time")
    plt.legend()
    plt.grid(True)


    plot_path = "./experiment_plots/experiment_1.png"
    plt.savefig(plot_path)
    plt.close()

def experiment2_plot():
    df = pd.read_csv("results_ex2.csv")

    i_values = df['i_max'].unique()
    t_values = df['t_max'].unique()

    i_values.sort()
    t_values.sort()

    for i_max in i_values:
        for t_max in t_values:

            subset = df[(df['i_max'] == i_max) & (df['t_max'] == t_max)]
            subset = subset.sort_values(by="num_threads")

            plt.figure(figsize=(8, 5))
            plt.plot(subset["num_threads"], subset["speedup"], marker='o', linewidth=2)

            plt.xlabel("Number of Threads")
            plt.ylabel("Speedup")
            plt.title(f"Speedup vs Threads\n(i_max={i_max}, t_max={t_max})")
            plt.grid(True)

            # Save the figure
            filename = f"./experiment_plots/ex2_i{i_max}_t{t_max}.png"
            plt.savefig(filename, dpi=200)
            plt.close()

            print(f"Saved: {filename}")


if __name__ == '__main__':
    experiment2_plot()