import subprocess
import time
import matplotlib.pyplot as plt

def run_command(command):
    start_time = time.time()
    subprocess.run(command, shell=True)
    end_time = time.time()
    return end_time - start_time

# Configuration
amount_of_numbers = [5000000, 10000000, 100000000]
number_of_workers = [1, 2, 3, 6]
chunk_sizes = [10, 100, 1000, 10000]

# Collecting results
results = {}

for num in amount_of_numbers:
    results[num] = {}
    for workers in number_of_workers:
        results[num][workers] = []
        for chunk in chunk_sizes:
            command = f"./randomGenerator 10 {num} | time ./primeCounter {workers} {chunk}"
            elapsed_time = run_command(command)
            results[num][workers].append(elapsed_time)
            print(f"Executed {command} in {elapsed_time:.2f} seconds")

# Plotting the results
fig, axs = plt.subplots(1, 3, figsize=(18, 6), sharey=True)
fig.suptitle('Execution Time for Different Configurations')

for i, num in enumerate(amount_of_numbers):
    ax = axs[i]
    for workers in number_of_workers:
        ax.plot(chunk_sizes, results[num][workers], label=f'{workers} Workers')
    ax.set_title(f'{num} Numbers')
    ax.set_xlabel('Chunk Size')
    if i == 0:
        ax.set_ylabel('Time (seconds)')
    ax.legend()
    ax.set_xscale('log')

plt.tight_layout()
plt.show()
