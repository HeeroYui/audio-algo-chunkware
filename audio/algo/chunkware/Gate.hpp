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
#pragma once

#include <etk/types.hpp>
#include <audio/algo/chunkware/AttRelEnvelope.hpp>
#include <audio/algo/chunkware/Gain.hpp>
#include <audio/format.hpp>

namespace audio {
	namespace algo {
		namespace chunkware {
			class Gate : public AttRelEnvelope {
				protected:
					bool m_isConfigured;
				public:
					Gate();
					virtual ~Gate() {}
				public:
					/**
					 * @brief Initialize the Algorithm
					 */
					virtual void init();
					/**
					 * @brief Get list of format suported in input.
					 * @return list of supported format
					 */
					virtual std::vector<enum audio::format> getSupportedFormat();
					/**
					 * @brief Get list of algorithm format suported. No format convertion.
					 * @return list of supported format
					 */
					virtual std::vector<enum audio::format> getNativeSupportedFormat();
					/**
					 * @brief Main input algo process.
					 * @param[in,out] _output Output data.
					 * @param[in] _input Input data.
					 * @param[in] _nbChunk Number of chunk in the input buffer.
					 * @param[in] _nbChannel Number of channel in the stream.
					 * @param[in] _format Input data format.
					 */
					virtual void process(void* _output, const void* _input, size_t _nbChunk, int8_t _nbChannel = 2, enum audio::format _format = audio::format_double);
				protected:
					virtual void processDouble(double* _out, const double* _in, int8_t _nbChannel);
					virtual void processDouble(double* _out, const double* _in, int8_t _nbChannel, double _value);
					/*
					virtual void processFloat(float* _out, const float* _in, int8_t _nbChannel);
					virtual void process16_16(int16_16_t* _out, const int16_16_t* _in, int8_t _nbChannel);
					virtual void process16_32(int16_32_t* _out, const int16_32_t* _in, int8_t _nbChannel);
					virtual void process24_32(int24_32_t* _out, const int24_32_t* _in, int8_t _nbChannel);
					virtual void process32_32(int32_32_t* _out, const int32_32_t* _in, int8_t _nbChannel);
					virtual void process32_64(int32_64_t* _out, const int32_64_t* _in, int8_t _nbChannel);
					*/
				public:
					// parameters
					virtual void setThreshold(double _dB);
					virtual double getThreshold() const {
						return m_thresholddB;
					}
				private:
					// transfer function
					double m_thresholddB; //!< threshold (dB)
					double m_threshold; //!< threshold (linear)
					// runtime variables
					double m_overThresholdEnvelope; //!< over-threshold envelope (linear)
			};
		}
	}
}

