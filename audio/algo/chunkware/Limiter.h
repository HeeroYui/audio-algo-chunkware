/**
 * @author Bojan MARKOVIC
 * @author Edouard DUPIN
 * @copyright 2006, ChunkWare Music Software, OPEN-SOURCE
 * @license BSD-1 (see license file)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#ifndef __AUDIO_ALGO_CHUNKWARE_LIMITER_H__
#define __AUDIO_ALGO_CHUNKWARE_LIMITER_H__

#include <etk/types.h>
#include <audio/format.h>
#include <audio/algo/chunkware/AttRelEnvelope.h>
#include <audio/algo/chunkware/Gain.h>
#include <etk/chrono.h>
#include <vector>

namespace audio {
	namespace algo {
		namespace chunkware {
			// class for faster attack/release
			class FastEnvelope : public audio::algo::chunkware::EnvelopeDetector {
				public:
					FastEnvelope(double _ms = 1.0, double _sampleRate = 44100.0) :
					  EnvelopeDetector(_ms, _sampleRate) {
						
					}
					virtual ~FastEnvelope() {}
				protected:
					// override setCoef() - coefficient calculation
					virtual void setCoef();
			};
			
			class Limiter {
				protected:
					bool m_isConfigured;
				public:
					Limiter();
					virtual ~Limiter() {}
				
				protected:
					double m_threshdB; //!< threshold (dB)
				public:
					virtual void setThreshold(double _dB);
					virtual double getThreshold() const {
						return m_threshdB;
					}
				
				protected:
					std11::chrono::microseconds m_attackTime; //!< attaque time in ms.
				public:
					virtual void setAttack(double _ms);
					virtual double getAttack() const {
						return m_attack.getTc();
					}
				
				protected:
					std11::chrono::microseconds m_releaseTime; //!< attaque time in ms.
				public:
					virtual void setRelease(double _ms);
					virtual double getRelease() const {
						return m_release.getTc();
					}
					// latency
				protected:
					unsigned int m_peakHold; //!< peak hold (samples)
				public:
					virtual const unsigned int getLatency() const {
						return m_peakHold;
					}
				
				protected:
					
				public:
					// sample rate dependencies
					virtual void setSampleRate(double _sampleRate);
					virtual double getSampleRate() {
						return m_attack.getSampleRate();
					}
					// runtime
					// call before runtime (in resume())
					virtual void initRuntime();
					void process(audio::format _format, void* _output, const void* _input, size_t _nbChunk, int8_t _nbChannel);
				protected:
					/*
					void process(double* _out, const double* _in, int8_t _nbChannel);
					void process(float* _out, const float* _in, int8_t _nbChannel);
					void process(int16_16_t* _out, const int16_16_t* _in, int8_t _nbChannel);
					void process(int16_32_t* _out, const int16_32_t* _in, int8_t _nbChannel);
					void process(int24_32_t* _out, const int24_32_t* _in, int8_t _nbChannel);
					void process(int32_32_t* _out, const int32_32_t* _in, int8_t _nbChannel);
					void process(int32_64_t* _out, const int32_64_t* _in, int8_t _nbChannel);
					*/
					// limiter runtime process
					void process(double& _in1, double& _in2);
					void processMono(double& _in);
				private:
					// transfer function
					
					double m_threshold; //!< threshold (linear)
					// max peak
					
					unsigned int m_peakTimer; //!< peak hold timer
					double m_maxPeak; //!< max peak
					// attack/release envelope
					audio::algo::chunkware::FastEnvelope m_attack; //!< attack
					audio::algo::chunkware::FastEnvelope m_release; //!< release
					
					
					double m_overThresholdEnvelope; //!< over-threshold envelope (linear)
					// buffer
					// BUFFER_SIZE default can handle up to ~10ms at 96kHz
					// change this if you require more
					static const int BUFFER_SIZE = 1024; //!< buffer size (always a power of 2!)
					unsigned int m_bufferMask; //!< buffer mask
					unsigned int m_cursor; //!< cursor
					std::vector< double > m_outputBuffer[2]; //!< output buffer
			};
		}
	}
}

#endif
