import pandas as pd
import matplotlib.pyplot as plt


data = pd.read_csv("experiment_results.csv")

# Plot
plt.figure(figsize=(8, 5))
plt.plot(data['limit'], data['time_seconds'], marker='o', linestyle='-', color='blue')
plt.title("Execution Time of Multithreaded Sieve of Eratosthenes")
plt.xlabel("Prime Limit")
plt.ylabel("Time (seconds)")
plt.grid(True)
plt.xticks(data['limit'], rotation=45)
plt.tight_layout()
plt.savefig("sieve_experiment_plot.png")
plt.show()
