#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cstdio>

// Define parameter ranges
const std::vector<int> p_range = {30};
const std::vector<double> r1_range = {0.9};
const std::vector<double> rlag_range = {0.4, 0.5, 0.8};
const std::vector<int> z_range = {4000, 5000, 6000, 7000, 10000};
const std::vector<float> threshold_range = {0.01, 0.05};
const std::vector<float> offset_range = {0.001};

// Function to run get_pitch and extract percentage
double runGetPitch(int p, double r1, double rlag, int z, float threshold, double offset)
{
    // Construct command for run_get_pitch
    std::stringstream cmd_run_get_pitch;
    cmd_run_get_pitch << "run_get_pitch " << p << " " << r1 << " " << rlag << " " << z
                      << " " << threshold << " " << offset;

    // Call run_get_pitch and capture output
    FILE *pipe = popen(cmd_run_get_pitch.str().c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Error executing command: " << cmd_run_get_pitch.str() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Extract percentage from the output
    double percentage = 0;
    char buffer[128];
    while (fgets(buffer, 128, pipe) != NULL)
    {
        std::string line(buffer);
        if (line.find("TOTAL:") != std::string::npos)
        {
            sscanf(buffer, "===> TOTAL: %lf %%", &percentage);
        }
    }
    pclose(pipe);

    return percentage;
}

int main()
{
    // Initialize variables to store best performance and parameters
    double best_percentage = 0;
    int best_p = 0;
    double best_r1 = 0;
    double best_rlag = 0;
    int best_z = 0;
    float best_threshold = 0;
    double best_offset = 0;

    // Calculate total iterations
    size_t total_iterations = p_range.size() * r1_range.size() * rlag_range.size() *
                              z_range.size() * threshold_range.size() * offset_range.size();
    ;
    size_t current_iteration = 0;

    // Iterate over parameter combinations
    for (auto offset : offset_range)
    {
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
                            // Run run_get_pitch and get performance
                            double percentage = runGetPitch(p, r1, rlag, z, thr, offset);

                            // Update best percentage and parameters if needed
                            if (percentage > best_percentage)
                            {
                                best_percentage = percentage;
                                best_p = p;
                                best_r1 = r1;
                                best_rlag = rlag;
                                best_z = z;
                                best_threshold = thr;
                                best_offset = offset;
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
    }

    // Print the best performance and parameters
    std::cout << "\nBest performance: " << best_percentage << "%\n";
    std::cout << "Parameters: p=" << best_p << ", r1=" << best_r1 << ", rlag=" << best_rlag << ", z=" << best_z
              << ", thr=" << best_threshold << ", offset=" << std::fixed << std::setprecision(3) << best_offset << "\n";

    return 0;
}
