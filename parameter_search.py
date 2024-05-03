import subprocess

# Define parameter ranges
p_range = range(10, 61, 5)
r1_range = [i * 0.1 for i in range(1, 11)]
rlag_range = [i * 0.1 for i in range(1, 11)]
z_range = range(100, 3100, 100)

# Initialize variables to store best performance and parameters
best_percentage = 0
best_params = None

# Iterate over parameter combinations
for p in p_range:
    for r1 in r1_range:
        for rlag in rlag_range:
            for z in z_range:
                # Construct command for get_pitch
                cmd_get_pitch = f"get_pitch -p {p} --r1 {r1} --rlag {rlag} -z {z} prueba.wav prueba.f0"
                # Call get_pitch
                subprocess.run(cmd_get_pitch, shell=True)
                
                # Construct command for pitch_evaluate
                cmd_pitch_evaluate = "pitch_evaluate prueba.f0ref"
                # Call pitch_evaluate
                result = subprocess.run(cmd_pitch_evaluate, shell=True, capture_output=True, text=True)
                
                # Extract percentage from the output
                lines = result.stdout.splitlines()
                last_line = lines[-3]
                percentage = float(last_line.split(":")[-1].strip().split()[0])
                
                # Update best percentage and parameters if needed
                if percentage > best_percentage:
                    best_percentage = percentage
                    best_params = (p, r1, rlag, z)

# Print the best performance and parameters
print(f"Best performance: {best_percentage:.2f}%")
print(f"Parameters: p={best_params[0]}, r1={best_params[1]}, rlag={best_params[2]}, z={best_params[3]}")