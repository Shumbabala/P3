#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdlib>

// Define parameter ranges
const std::vector<int> p_range = {21};
const std::vector<double> r1_range = {0.9};
const std::vector<double> rlag_range = {0.4};
const std::vector<int> z_range = {1500};
const std::vector<float> threshold_range = {0.01, 0.05, 0.1, 0.2, 0.5, 1, 2};

// Function to run get_pitch and pitch_evaluate commands
double runCommands(int p, double r1, double rlag, int z, float threshold)
{
    // Construct command for get_pitch
    std::stringstream cmd_get_pitch;
    cmd_get_pitch << "get_pitch -p " << p << " --r1 " << r1 << " --rlag " << rlag << " -z " << z << " --thr " << threshold << " prueba.wav prueba.f0";

    // Call get_pitch
    system(cmd_get_pitch.str().c_str());

    // Construct command for pitch_evaluate
    std::string cmd_pitch_evaluate = "pitch_evaluate prueba.f0ref";

    // Call pitch_evaluate
    FILE *pipe = popen(cmd_pitch_evaluate.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Error executing command: " << cmd_pitch_evaluate << std::endl;
        exit(EXIT_FAILURE);
    }

    // Extract percentage from the output
    double percentage = 0;
    char buffer[128];
    std::string previous_token = "";
    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != NULL)
        {
            std::string line(buffer);
            std::istringstream iss(line);
            std::string token;
            while (iss >> token)
            {
                // Check if the token is " %\n"
                if (token == "%")
                {
                    // If yes, return the previous token as it contains the percentage value
                    percentage = std::stod(previous_token);
                    break;
                }
                // Store the current token for the next iteration
                previous_token = token;
            }
        }
    }
    pclose(pipe);

    return percentage;
}

int main()
{
    // Initialize variables to store best performance and parameters
    double best_percentage = 0;
    int best_p;
    double best_r1;
    double best_rlag;
    int best_z;
    float best_threshold;

    // Calculate total iterations
    size_t total_iterations = p_range.size() * r1_range.size() * rlag_range.size() * z_range.size() * threshold_range.size();
    size_t current_iteration = 0;

    // Iterate over parameter combinations
    for (auto thr : threshold_range)
    {
        for (auto p : p_range)
        {
            for (auto r1 : r1_range)
            {
                for (auto rlag : rlag_range)
                {
                    for (auto z : z_range)
                    {
                        // Run commands and get performance
                        double percentage = runCommands(p, r1, rlag, z, thr);

                        // Update best percentage and parameters if needed
                        if (percentage > best_percentage)
                        {
                            best_percentage = percentage;
                            best_p = p;
                            best_r1 = r1;
                            best_rlag = rlag;
                            best_z = z;
                            best_threshold = thr;
                        }

                        // Update progress bar
                        ++current_iteration;
                        double progress = static_cast<double>(current_iteration) / total_iterations * 100.0;
                        std::cout << "\rProgress: " << std::fixed << std::setprecision(2) << progress << "%";
                        std::cout.flush();
                    }
                }
            }
        }
    }

    // Print the best performance and parameters
    std::cout << "\nBest performance: " << best_percentage << "%\n";
    std::cout << "Parameters: p=" << best_p << ", r1=" << best_r1 << ", rlag=" << best_rlag << ", z=" << best_z
              << ", thr=" << best_threshold << "\n";

    return 0;
}
