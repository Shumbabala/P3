/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <filesystem>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN 0.030   /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;
namespace fs = std::__fs::filesystem;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project
    -p REAL  maximum power above which voiced is considered [default: 45]
    --r1 REAL  maximum value of r1 norm (r[1] / r[0]) above which voiced is considered [default: 0.9]
    --rlag REAL  maximum value of r_max norm (r[lag] / r[0]) above which voiced is considered [default: 0.5]
    -z REAL  maximum value of zero crossings above which voiced is NOT considered [default: 2e3]
    --thr REAL  center clipping threshold [default: 0.0]
    --offset REAL  center clipping offset [default: 0.0]

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[])
{
  /// \TODO
  ///  Modify the program syntax and the call to **docopt()** in order to
  ///  add options and arguments to the program.
  /// \DONE added 4 additional arguments
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
                                                             {argv + 1, argv + argc}, // array of arguments, without the program name
                                                             true,                    // show help if requested
                                                             "2.0");                  // version string

  std::string input_wav = args["<input-wav>"].asString();
  std::string output_txt = args["<output-txt>"].asString();

  // additional params extraction
  float max_power = std::stof(args["-p"].asString());
  float r1norm_max = std::stof(args["--r1"].asString());
  float rmaxnorm_max = std::stof(args["--rlag"].asString());
  float zcr_max = std::stof(args["-z"].asString());
  float center_clipping_threshold = std::stof(args["--thr"].asString());
  float center_clipping_offset = std::stof(args["--offset"].asString());

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0)
  {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  // copy audio data to output file
  ofstream outFile("audio_samples.txt", ios::app);
  if (outFile.is_open())
  {
    // Iterate over the current frame and write each value to the file
    for (auto it = x.begin(); it < x.end(); ++it)
    {
      outFile << *it << "\n"; // Write value to file followed by newline
    }
    outFile.close(); // Close the file
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, max_power, r1norm_max, rmaxnorm_max, zcr_max, PitchAnalyzer::RECT, 50, 500);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift)
  {
    // test center clipping
    if (center_clipping_offset == 0.0f && center_clipping_threshold != 0.0f)
    {
      // create float vector
      vector<float> x(n_len); // local copy of input frame, size N
      copy(iX, iX + n_len, x.begin());

      // apply clipping
      vector<float> center_clipped = analyzer.center_clipping_NoOffset(x, center_clipping_threshold);
      float f = analyzer(center_clipped);
      f0.push_back(f);
    }
    else if (center_clipping_offset != 0.0f && center_clipping_threshold != 0.0f)
    {
      // applying center clipping with offset

      //  create float vector
      vector<float> x(n_len); // local copy of input frame, size N
      copy(iX, iX + n_len, x.begin());

      // apply clipping
      vector<float> center_clipped = analyzer.center_clipping_YesOffset(x, center_clipping_threshold,
                                                                        center_clipping_offset);
      float f = analyzer(center_clipped);
      f0.push_back(f);
    }
    else
    {
      float f = analyzer(iX, iX + n_len);
      f0.push_back(f);
    }

    // addition for exporting to txt file to later graph using matplotlib (remove after graphs are done)
    //  Check if it's the second iteration
    // if (iteration == 192)
    // {
    //   // Open the file in append mode
    //   vector<float> aux(n_len);
    //   ofstream outFile("extracted_samples.txt", ios::app);
    //   if (outFile.is_open())
    //   {
    //     // Iterate over the current frame and write each value to the file
    //     int i = 0;
    //     for (auto it = iX; it < iX + n_len; ++it)
    //     {
    //       aux[i] = *it;
    //       outFile << aux[i] << "\n"; // Write value to file followed by newline
    //       i++;
    //     }
    //     outFile.close(); // Close the file
    //   }

    // }
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  // Check if the file exists and delete it if it does
  if (fs::exists(output_txt))
  {
    fs::remove(output_txt);
  }

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good())
  {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; // pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX)
    os << *iX << '\n';
  os << 0 << '\n'; // pitch at t=Dur

  return 0;
}
