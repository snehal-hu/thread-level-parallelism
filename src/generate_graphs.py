import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

# Actual Windows path of the project
BASE_DIR = Path(r"C:\msys64\home\91982\thread-level-parallelism")
RESULTS_DIR = BASE_DIR / "results"
GRAPHS_DIR = RESULTS_DIR / "graphs"

# Create graphs directory
GRAPHS_DIR.mkdir(parents=True, exist_ok=True)

# Read benchmark results
results = pd.read_csv(RESULTS_DIR / "results.csv")

# 1. Execution Time
plt.figure(figsize=(8, 5))
plt.plot(
    results["Threads"],
    results["AverageTime"],
    marker="o"
)
plt.xlabel("Number of Threads")
plt.ylabel("Average Execution Time (seconds)")
plt.title("Execution Time vs Number of Threads")
plt.grid(True)
plt.tight_layout()
plt.savefig(GRAPHS_DIR / "execution_time.png", dpi=300)
plt.close()

# 2. Speedup
plt.figure(figsize=(8, 5))
plt.plot(
    results["Threads"],
    results["Speedup"],
    marker="o"
)
plt.xlabel("Number of Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs Number of Threads")
plt.grid(True)
plt.tight_layout()
plt.savefig(GRAPHS_DIR / "speedup.png", dpi=300)
plt.close()

# 3. Parallel Efficiency
plt.figure(figsize=(8, 5))
plt.plot(
    results["Threads"],
    results["Efficiency"],
    marker="o"
)
plt.xlabel("Number of Threads")
plt.ylabel("Efficiency (%)")
plt.title("Parallel Efficiency vs Number of Threads")
plt.grid(True)
plt.tight_layout()
plt.savefig(GRAPHS_DIR / "efficiency.png", dpi=300)
plt.close()

# 4. Load-Balanced Execution Time
load_balanced = pd.read_csv(
    RESULTS_DIR / "load_balanced_results.csv"
)

plt.figure(figsize=(8, 5))
plt.plot(
    load_balanced["Threads"],
    load_balanced["ExecutionTime"],
    marker="o"
)
plt.xlabel("Number of Threads")
plt.ylabel("Execution Time (seconds)")
plt.title("Load-Balanced Execution Time vs Number of Threads")
plt.grid(True)
plt.tight_layout()
plt.savefig(
    GRAPHS_DIR / "load_balanced_execution_time.png",
    dpi=300
)
plt.close()

print("All graphs generated successfully!")
print("Graphs saved to:")
print(GRAPHS_DIR)
