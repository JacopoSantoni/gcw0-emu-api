#ifndef __SFX_H__
#define __SFX_H__

#include "../common/defines.h"
#include "audiosink.h"
#include "linint.h"

#include <memory>
#include <functional>


namespace gcw
{
  struct AudioStatus {
    long rate;
    bool low;
    
    AudioStatus(long rate, bool low) : rate(rate), low(low) {}
  };
  
  class AudioOut {
  public:

    
    AudioOut(long sampleRate, int latency, int periods, std::size_t maxInSamplesPerWrite) :
    resampler_(new Linint<2>(2097152, sampleRate)),
    resampleBuf_(resampler_->maxOut(maxInSamplesPerWrite) * 2),
    sink_(sampleRate, latency, periods)
    {
    }
    
    AudioStatus write(Uint32 const *data, std::size_t samples) {
      long const outsamples = resampler_->resample(
                                                   resampleBuf_, reinterpret_cast<Sint16 const *>(data), samples);
      AudioSink::Status const &stat = sink_.write(resampleBuf_, outsamples);
      bool low = stat.fromUnderrun + outsamples < (stat.fromOverflow - outsamples) * 2;
      return AudioStatus(stat.rate, low);
    }
    
  private:
    std::unique_ptr<Resampler> const resampler_;
    Array<Sint16> const resampleBuf_;
    AudioSink sink_;
  };

}
#endif