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


#ifndef __AUDIO_ALGO_CHUNKWARE_COMPRESSOR_H__
#define __AUDIO_ALGO_CHUNKWARE_COMPRESSOR_H__

#include <etk/types.h>
#include <audio/format.h>
#include <audio/algo/chunkware/AttRelEnvelope.h>
#include <audio/algo/chunkware/Gain.h>

namespace audio {
	namespace algo {
		namespace chunkware {
			class Compressor : public audio::algo::chunkware::AttRelEnvelope {
				protected:
					bool m_isConfigured;
				public:
					Compressor();
					virtual ~Compressor() {}
				public:
					/**
					 * @brief Initialize the Algorithm
					 */
					virtual void init();
					/**
					 * @brief Get list of format suported in input.
					 * @return list of supported format
					 */
					std::vector<enum audio::format> getSupportedFormat();
					/**
					 * @brief Get list of algorithm format suported. No format convertion.
					 * @return list of supported format
					 */
					std::vector<enum audio::format> getNativeSupportedFormat();
					/**
					 * @brief Main input algo process.
					 * @param[in,out] _output Output data.
					 * @param[in] _input Input data.
					 * @param[in] _nbChunk Number of chunk in the input buffer.
					 * @param[in] _nbChannel Number of channel in the stream.
					 * @param[in] _format Input data format.
					 */
					void process(void* _output, const void* _input, size_t _nbChunk, int8_t _nbChannel = 2, enum audio::format _format = audio::format_double);
				protected:
					virtual void processDouble(double* _out, const double* _in, int8_t _nbChannel);
					void processDouble(double* _out, const double* _in, int8_t _nbChannel, double _value);
					/*
					void process(float* _out, const float* _in, int8_t _nbChannel);
					void process(int16_16_t* _out, const int16_16_t* _in, int8_t _nbChannel);
					void process(int16_32_t* _out, const int16_32_t* _in, int8_t _nbChannel);
					void process(int24_32_t* _out, const int24_32_t* _in, int8_t _nbChannel);
					void process(int32_32_t* _out, const int32_32_t* _in, int8_t _nbChannel);
					void process(int32_64_t* _out, const int32_64_t* _in, int8_t _nbChannel);
					*/
				protected:
					double m_threshdB;//!< threshold (dB)
				public:
					virtual void setThreshold(double _dB);
					virtual double getThreshold() const {
						return m_threshdB;
					}
				protected:
					double m_ratio; //!< ratio (compression: < 1 ; expansion: > 1)
				public:
					virtual void setRatio(double _dB);
					virtual double getRatio()  const {
						return m_ratio;
					}
				protected:
					// runtime variables
					double m_overThresholdEnvelopeDB; //!< over-threshold envelope (dB)
			};
		}
	}
}

#endif
