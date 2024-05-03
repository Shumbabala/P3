import matplotlib.pyplot as plt

# Read the extracted samples from the file
with open("audio_samples.txt", "r") as file:
    samples = [float(line.strip().split()[0]) for line in file]

# Calculate the time axis
#time_step = 0.3 / len(samples)  # Assuming the total duration is 0.3 seconds
#time = [i * time_step for i in range(len(samples))]
time = range(len(samples))

# Plot the samples
plt.figure(figsize=(10, 4))
plt.plot(time, samples, color='blue')
plt.title('Audio Signal')
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.grid(True)
plt.tight_layout()
plt.show()
