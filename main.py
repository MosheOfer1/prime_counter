import subprocess
import time
import matplotlib.pyplot as plt

# Define the ranges for MAX_THREADS
max_threads_range = [1, 2, 3, 6, 10]
k = 3  # Number of iterations to average over
numbers = 10000000

results = []

for max_threads in max_threads_range:
    total_time = 0
    
    for _ in range(k):
        print(f"Running {numbers} with {max_threads} threads")

        # Start measuring time using 'time -v' command
        process_random = subprocess.Popen(["./randomGenerator", "10", str(numbers)], stdout=subprocess.PIPE)
        process_prime = subprocess.Popen(["time","-v","./primeCounter", str(max_threads)], stdin=process_random.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        # Wait for the prime counter process to finish and get the output
        _, stderr = process_prime.communicate()
        

        # Parse the output to extract timing information
        time_info = stderr.decode("utf-8")  # Using stderr for time -v output
        time_lines = time_info.splitlines()
        for line in time_lines:
            if line.startswith("\tElapsed (wall clock) time"):
                elapsed_time = float(line.split(":")[-1].strip().split()[0])  # Extract the time in seconds
                break

        total_time += elapsed_time
    
    average_time = total_time / k
    results.append((max_threads, average_time))
    print(f"Average time for {max_threads} threads: {average_time} seconds")

# Extract max_threads and average_time from results
max_threads_values, average_times = zip(*results)

# Plot the results
plt.plot(max_threads_values, average_times, marker='o')
plt.title('Average Execution Time vs. Maximum Threads')
plt.xlabel('Maximum Threads')
plt.ylabel('Average Execution Time (seconds)')
plt.xticks(max_threads_range)
plt.grid(True)
plt.show()
