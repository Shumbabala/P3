import matplotlib.pyplot as plt
import numpy as np

# Function to read samples from a text file
def read_samples(file_path):
    with open(file_path, "r") as file:
        lines = file.readlines()
        power = []
        corr_distance_1 = []
        corr_distance_lag = []
        zero_crossings = []
        for line in lines:
            values = line.strip().split()
            power.append(float(values[0]))
            corr_distance_1.append(float(values[1]))
            corr_distance_lag.append(float(values[2]))
            zero_crossings.append(float(values[3]))
        return power, corr_distance_1, corr_distance_lag, zero_crossings

# Read samples from "plot_samples.txt"
power, corr_distance_1, corr_distance_lag, zero_crossings = read_samples("plot_samples.txt")

# Create time axis
time = np.arange(len(power))  # Assuming each line corresponds to a time step
time = time/len(time) * 0.3

# Create subplots
fig, axs = plt.subplots(4, figsize=(10, 12))

# Plot each set of samples
axs[0].plot(time, power, color='blue')
axs[0].set_title('Power')
axs[0].set_xlabel('Time')
axs[0].set_ylabel('Power')
axs[0].grid(True)

axs[1].plot(time, corr_distance_1, color='red')
axs[1].set_title('Correlation Distance 1')
axs[1].set_xlabel('Time')
axs[1].set_ylabel('Distance')
axs[1].grid(True)

axs[2].plot(time, corr_distance_lag, color='green')
axs[2].set_title('Correlation Distance Lag')
axs[2].set_xlabel('Time')
axs[2].set_ylabel('Distance')
axs[2].grid(True)

axs[3].plot(time, zero_crossings, color='purple')
axs[3].set_title('Zero Crossings')
axs[3].set_xlabel('Time')
axs[3].set_ylabel('Count')
axs[3].grid(True)

# Adjust layout and display
plt.tight_layout()
plt.show()