/// @file

#include <iostream>
#include <fstream>
#include <math.h>
#include <cmath>
#include "pitch_analyzer.h"

using namespace std;

/// Name space of UPC
namespace upc
{
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const
  {

    for (unsigned int l = 0; l < r.size(); ++l)
    {
      /// \TODO Compute the autocorrelation r[l] <-- DONE IN LAB
      /// \DONE Autocorrelation COMPLETE
      r[l] = 0;
      for (unsigned int n = l; n < x.size(); ++n)
      {
        r[l] += x[n] * x[n - l];
      }
      r[l] /= x.size();
    }

    if (r[0] == 0.0F) // to avoid log() and divide zero
      r[0] = 1e-10;
  }

  void PitchAnalyzer::set_window(Window win_type)
  {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type)
    {
    case HAMMING:
      /// \TODO Implement the Hamming window
      break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0)
  {
    npitch_min = (unsigned int)samplingFreq / max_F0;
    if (npitch_min < 2)
      npitch_min = 2; // samplingFreq/2

    npitch_max = 1 + (unsigned int)samplingFreq / min_F0;

    // frameLen should include at least 2*T0
    if (npitch_max > frameLen / 2)
      npitch_max = frameLen / 2;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm, int zcr) const
  {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.
    /// \DONE voiced/unvoiced rules implemented (although improvements are allowed)
    if ((pot <= max_power && r1norm >= r1norm_max) || (rmaxnorm >= rmaxnorm_max && zcr <= zcr_max) /*(pot > max_power && r1norm < r1norm_max && rmaxnorm > rmaxnorm_max && zcr < zcr_max)*/) // change as desired
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  int sgn(float x)
  {
    if (x > 0.0)
      return 1;
    else if (x < 0.0)
      return -1;
    else
      return 0;
  }

  int PitchAnalyzer::compute_zcr(std::vector<float> &x) const
  {
    int sum = 0;
    for (int i = 0; i < x.size(); i++)
    {
      if (sgn(x[i]) != sgn(x[i - 1]))
      {
        sum++;
      }
    }
    return (int)samplingFreq / (2 * (x.size() - 1)) * sum;
  }

  float PitchAnalyzer::compute_pitch(vector<float> &x) const
  {
    if (x.size() != frameLen)
      return -1.0F;

    // Window input frame
    for (unsigned int i = 0; i < x.size(); ++i)
      x[i] *= window[i];

    vector<float> r(npitch_max);

    // Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR = r.begin(), iRMax = iR;

    /// \TODO
    /// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
    /// Choices to set the minimum value of the lag are:
    ///    - The first negative value of the autocorrelation.
    ///    - The lag corresponding to the maximum value of the pitch.
    ///	   .
    /// In either case, the lag should not exceed that of the minimum value of the pitch.
    /// \DONE Find the second maximum value of the autocorrelation

    for (iRMax = iR = r.begin() + npitch_min; iR < r.begin() + npitch_max; ++iR) //<--implemented in lab
    {
      if (*iR > *iRMax)
      {
        iRMax = iR;
      }
    }

    unsigned int lag = iRMax - r.begin();

    float pot = abs(10 * log10(r[0]));

    // export metrics to file
    ofstream outFile("plot_samples.txt", ios::app);
    // if (outFile.is_open())
    // {
    //   outFile << pot << " " << r[1] / r[0] << " " << r[lag] / r[0] << " " << compute_zcr(x) << "\n"; // Write value to file followed by newline
    //   outFile.close();                                                                               // Close the file
    // }
    // You can print these (and other) features, look at them using wavesurfer
    // Based on that, implement a rule for unvoiced
    // change to #if 1 and compile
#if 0
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << endl;
#endif

    if (unvoiced(pot, r[1] / r[0], r[lag] / r[0], compute_zcr(x)))
      return 0;
    else
      return (float)samplingFreq / (float)lag;
  }

  vector<float> PitchAnalyzer::center_clipping_NoOffset(vector<float> &floats, float threshold) const
  {
    std::vector<float> result;
    result.reserve(floats.size()); // Reserve space to avoid reallocation

    for (float value : floats)
    {
      if (abs(value) < threshold)
      {
        result.push_back(0.0f);
      }
      else
      {
        result.push_back(value);
      }
    }

    return result;
  }

  vector<float> PitchAnalyzer::center_clipping_YesOffset(vector<float> &floats, float threshold, float offset) const
  {
    std::vector<float> result;
    result.reserve(floats.size()); // Reserve space to avoid reallocation

    for (float value : floats)
    {

      if (value > threshold)
      {
        result.push_back(value - offset);
      }
      else if (value < -threshold)
      {
        result.push_back(value + threshold);
      }
      else
      {
        result.push_back(0.0f);
      }
    }

    return result;
  }
}
