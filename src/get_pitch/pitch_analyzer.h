/// @file

#ifndef PITCH_ANALYZER_H
#define PITCH_ANALYZER_H

#include <vector>
#include <algorithm>

namespace upc
{
  const float MIN_F0 = 20.0F;    ///< Minimum value of pitch in Hertzs
  const float MAX_F0 = 10000.0F; ///< Maximum value of pitch in Hertzs

  ///
  /// PitchAnalyzer: class that computes the pitch (in Hz) from a signal frame.
  /// No pre-processing or post-processing has been included
  ///
  class PitchAnalyzer
  {
  public:
    /// Wndow type
    enum Window
    {
      RECT,   ///< Rectangular window
      HAMMING ///< Hamming window
    };

    void set_window(Window type); ///< pre-compute window

  private:
    std::vector<float> window; ///< precomputed window
    unsigned int frameLen,     ///< length of frame (in samples). Has to be set in the constructor call
        samplingFreq,          ///< sampling rate (in samples per second). Has to be set in the constructor call
        npitch_min,            ///< minimum value of pitch period, in samples
        npitch_max;            ///< maximum value of pitch period, in samples

    /// additional auxiliary parameters (added)
    float max_power,  ///< maximum power to be considered unvoiced
        r1norm_max,   ///< maximum r1normal relation value to be considered voiced (too high is considered unvoiced)
        rmaxnorm_max, ///< similar to r1norm_max, also has an upper limit
        zcr_max;      ///< maximum value of zero crossings before considered unvoiced

    ///
    /// Computes correlation from lag=0 to r.size()
    ///
    void autocorrelation(const std::vector<float> &x, std::vector<float> &r) const;

    ///
    /// Returns the pitch (in Hz) of input frame x
    ///
    float compute_pitch(std::vector<float> &x) const;

    ///
    /// Returns true is the frame is unvoiced
    ///
    bool unvoiced(float pot, float r1norm, float rmaxnorm, int zcr) const;

  public:
    /// \TODO Added by Gerard, improve the default values for max_power, r1norm_max, rmaxnorm_max & zcr_max
    PitchAnalyzer(unsigned int fLen,                 ///< Frame length in samples
                  unsigned int sFreq,                ///< Sampling rate in Hertzs
                  float max_power_param,             ///< power above this value is considered voice
                  float r1norm_max_param,            ///< beyond this value is considered unvoiced segment
                  float rmaxnorm_max_param,          ///< beyond this value is considered unvoiced segment
                  float zcr_max_param,               ///< beyond this value is considered unvoiced
                  Window w = PitchAnalyzer::HAMMING, ///< Window type
                  float min_F0 = MIN_F0,             ///< Pitch range should be restricted to be above this value
                  float max_F0 = MAX_F0              ///< Pitch range should be restricted to be below this value
    )
    {
      frameLen = fLen;
      samplingFreq = sFreq;
      set_f0_range(min_F0, max_F0);
      set_window(w);
      max_power = max_power_param;
      r1norm_max = r1norm_max_param;
      rmaxnorm_max = rmaxnorm_max_param;
      zcr_max = zcr_max_param;
    }

    ///
    /// Operator (): computes the pitch for the given vector x
    ///
    float operator()(const std::vector<float> &_x) const
    {
      if (_x.size() != frameLen)
        return -1.0F;

      std::vector<float> x(_x); // local copy of input frame
      return compute_pitch(x);
    }

    ///
    /// Operator (): computes the pitch for the given "C" vector (float *).
    /// N is the size of the vector pointer by pt.
    ///
    float operator()(const float *pt, unsigned int N) const
    {
      if (N != frameLen)
        return -1.0F;

      std::vector<float> x(N);          // local copy of input frame, size N
      std::copy(pt, pt + N, x.begin()); /// copy input values into local vector x
      return compute_pitch(x);
    }

    ///
    /// Operator (): computes the pitch for the given vector, expressed by the begin and end iterators
    ///
    float operator()(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end) const
    {

      if (end - begin != frameLen)
        return -1.0F;

      std::vector<float> x(end - begin); // local copy of input frame, size N
      std::copy(begin, end, x.begin());  // copy input values into local vector x
      return compute_pitch(x);
    }

    ///
    /// Sets pitch range: takes min_F0 and max_F0 in Hz, sets npitch_min and npitch_max in samples
    ///
    void set_f0_range(float min_F0, float max_F0);

    /// @brief Computes the zero crossing rate of the input signal (x).
    /// @param x The input signal for which to compute the zero crossing rate.
    /// @return The computed zero crossing rate, expressed as an int value.
    int compute_zcr(std::vector<float> &x) const;

    // optimizer functions

    /// @brief Computes the center clipping without offset of the float vector pointed at by the argument iterator 
    std::vector<float> center_clipping_NoOffset(std::vector<float> &floats, float x) const;

    /// @brief Computes the center clipping with offset of the float vector pointed at by the argument iterator
    std::vector<float> center_clipping_YesOffset(std::vector<float> &floats, float x, float offset) const;
  };
}
#endif
