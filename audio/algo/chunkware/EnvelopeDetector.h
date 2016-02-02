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

#include <etk/types.h>

namespace audio {
	namespace algo {
		namespace chunkware {
			// USE:
			// 1. init envelope state to DC_OFFSET before processing
			// 2. add to input before envelope runtime function
			static const double DC_OFFSET = 1.0E-25;
			class EnvelopeDetector {
				public:
					EnvelopeDetector(double _ms = 1.0,
					                 double _sampleRate = 44100.0);
					virtual ~EnvelopeDetector() {}
					// time constant
					virtual void setTc(double _ms);
					virtual double getTc() const {
						return m_timeMs;
					}
					// sample rate
					virtual void setSampleRate(double _sampleRate);
					virtual double getSampleRate() const {
						return m_sampleRate;
					}
					// runtime function
					void run(double _in, double& _state) {
						_state = _in + m_coefficient * (_state - _in);
					}
				protected:
					double m_sampleRate; //!< sample rate
					double m_timeMs; //!< time constant in ms
					double m_coefficient; //!< runtime coefficient
					virtual void setCoef(); //!< coef calculation
			};
		}
	}
}

