import subprocess
import time
import argparse

def measure_time(command):
    start_time = time.time()
    process = subprocess.Popen(command, shell=True)
    process.wait()
    end_time = time.time()
    return end_time - start_time

def main(num1, num2):
    command1 = f"./randomGenerator {num1} {num2} | ./naive"
    command2 = f"./randomGenerator {num1} {num2} | ./primeCounter"

    print("Measuring time for command:", command1)
    time1 = measure_time(command1)
    print(f"Execution time: {time1:.4f} seconds")

    print("Measuring time for command:", command2)
    time2 = measure_time(command2)
    print(f"Execution time: {time2:.4f} seconds")

    print(f"\nComparison:\n'./naive' took {time1:.4f} seconds\n'./primeCounter' took {time2:.4f} seconds")
    if time1 < time2:
        print(f"naive is faster {time2/time1:.2f} times more")
    elif time2 < time1:
        print(f"primeCounter is faster {time1/time2:.0f} times more")
    else:
        print("Both commands took the same time")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Compare execution times of two commands.')
    parser.add_argument('num1', type=int, help='First number for randomGenerator')
    parser.add_argument('num2', type=int, help='Second number for randomGenerator')
    
    args = parser.parse_args()
    main(args.num1, args.num2)
