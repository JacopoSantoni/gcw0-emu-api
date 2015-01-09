//
//   Copyright (C) 2007 by sinamas <sinamas at users.sourceforge.net>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License version 2 for more details.
//
//   You should have received a copy of the GNU General Public License
//   version 2 along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include "audiosink.h"

#include <cstdio>

using namespace gcw;
using namespace std;

namespace {

static unsigned long nearestPowerOf2(unsigned long const in) {
	unsigned long out = in;
	out |= out >> 1;
	out |= out >> 2;
	out |= out >> 4;
	out |= out >> 8;
	out |= out >> 16;
	++out;

	if (!(out >> 2 & in))
		out >>= 1;

	return out;
}

static int openAudio(long srate, std::size_t samples,
                     void (*callback)(void *userdata, Uint8 *stream, int len),
                     void *userdata)
{
	SDL_AudioSpec spec;
	spec.freq = srate;
	spec.format = AUDIO_S16SYS;
	spec.channels = 2;
	spec.samples = samples;
	spec.callback = callback;
	spec.userdata = userdata;
	if (SDL_OpenAudio(&spec, 0) < 0) {
		std::fprintf(stderr, "Could not open audio: %s\n", SDL_GetError());
		return -1;
	}

	return 0;
}

} // anon ns

AudioSink::AudioSink(long const srate, int const latency, int const periods)
: rbuf_(nearestPowerOf2(srate * latency / ((periods + 1) * 1000)) * periods * 2)
, rateEst_(srate, rbuf_.size() / periods)
, failed_(openAudio(srate, rbuf_.size() / 2 / periods, fillBuffer, this) < 0)
{
	rbuf_.fill(0);
}

AudioSink::~AudioSink() {
	SDL_PauseAudio(1);
	SDL_CloseAudio();
}

AudioSink::Status AudioSink::write(Sint16 const *inBuf, std::size_t samples) {
	if (failed_)
		return Status(rbuf_.size() / 2, 0, rateEst_.result());

  unique_lock<std::mutex> lock(mut_);
	Status const status(rbuf_.used() / 2, rbuf_.avail() / 2, rateEst_.result());

	for (std::size_t avail; (avail = rbuf_.avail() / 2) < samples;) {
		rbuf_.write(inBuf, avail * 2);
		inBuf += avail * 2;
		samples -= avail;
    bufReadyCond_.wait(lock);
	}

	rbuf_.write(inBuf, samples * 2);
	return status;
}

void AudioSink::read(Uint8 *const stream, std::size_t const len) {
	if (failed_)
		return;

	unique_lock<std::mutex> lock(mut_);
	rbuf_.read(reinterpret_cast<Sint16 *>(stream), std::min(len / 2, rbuf_.used()));
	rateEst_.feed(len / 4);
  bufReadyCond_.notify_one();
}
