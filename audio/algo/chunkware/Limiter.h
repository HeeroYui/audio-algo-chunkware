/*
 *	Simple Limiter (header)
 *
 *  File		: SimpleLimit.h
 *	Library		: SimpleSource
 *  Version		: 1.12
 *  Class		: SimpleLimit
 *
 *	© 2006, ChunkWare Music Software, OPEN-SOURCE
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a
 *	copy of this software and associated documentation files (the "Software"),
 *	to deal in the Software without restriction, including without limitation
 *	the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *	and/or sell copies of the Software, and to permit persons to whom the
 *	Software is furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *	DEALINGS IN THE SOFTWARE.
 */


#ifndef __SIMPLE_LIMIT_H__
#define __SIMPLE_LIMIT_H__

#include "header.h"
#include "Envelope.h"
#include "Gain.h"
#include <vector>

namespace audio {
	namespace algo {
		namespace chunkware {
			//-------------------------------------------------------------
			// simple limiter
			//-------------------------------------------------------------
			class Limiter {
				public:
					Limiter();
					virtual ~Limiter() {}
					// parameters
					virtual void setThresh(double dB);
					virtual void setAttack(double ms);
					virtual void setRelease(double ms);
					virtual double getThresh() const {
						return threshdB_;
					}
					virtual double getAttack() const {
						return att_.getTc();
					}
					virtual double getRelease() const {
						return rel_.getTc();
					}
					// latency
					virtual const unsigned int getLatency() const {
						return peakHold_;
					}
					// sample rate dependencies
					virtual void   setSampleRate(double sampleRate);
					virtual double getSampleRate() {
						return att_.getSampleRate();
					}
					// runtime
					// call before runtime (in resume())
					virtual void initRuntime();
					// limiter runtime process
					void process(double &in1, double &in2);
				protected:
					// class for faster attack/release
					class FastEnvelope : public EnvelopeDetector {
						public:
							FastEnvelope(double ms = 1.0, double sampleRate = 44100.0)
								: EnvelopeDetector(ms, sampleRate)
							{}
							virtual ~FastEnvelope() {}
						protected:
							// override setCoef() - coefficient calculation
							virtual void setCoef();
					};
				private:
					// transfer function
					double threshdB_;	// threshold (dB)
					double thresh_;		// threshold (linear)
					// max peak
					unsigned int peakHold_;		// peak hold (samples)
					unsigned int peakTimer_;	// peak hold timer
					double maxPeak_;			// max peak
					// attack/release envelope
					FastEnvelope att_;			// attack
					FastEnvelope rel_;			// release
					double env_;				// over-threshold envelope (linear)
					// buffer
					// BUFFER_SIZE default can handle up to ~10ms at 96kHz
					// change this if you require more
					static const int BUFFER_SIZE = 1024;	//!< buffer size (always a power of 2!)
					unsigned int mask_;						//!< buffer mask
					unsigned int cur_;						//!< cursor
					std::vector< double > outBuffer_[ 2 ];	//!< output buffer
			};
		}
	}
}

#endif
