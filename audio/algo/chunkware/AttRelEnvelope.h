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

#include <audio/algo/chunkware/EnvelopeDetector.h>

namespace audio {
	namespace algo {
		namespace chunkware {
			class AttRelEnvelope {
				public:
					AttRelEnvelope(double _attackms = 10.0,
					               double _releasems = 100.0,
					               double _sampleRate = 44100.0);
					virtual ~AttRelEnvelope() {}
					// attack time constant
					virtual void setAttack(double _ms);
					virtual double getAttack() const {
						return m_attack.getTc();
					}
					// release time constant
					virtual void setRelease(double _ms);
					virtual double getRelease() const {
						return m_release.getTc();
					}
					// sample rate dependencies
					virtual void setSampleRate(double _sampleRate);
					virtual double getSampleRate() const {
						return m_attack.getSampleRate();
					}
					// runtime function
					void run(double _in, double& _state) {
						/* assumes that:
						* positive delta = attack
						* negative delta = release
						* good for linear & log values
						*/
						if (_in > _state) {
							// attack
							m_attack.run(_in, _state);
						} else {
							// release
							m_release.run(_in, _state);
						}
					}
				private:
					audio::algo::chunkware::EnvelopeDetector m_attack;
					audio::algo::chunkware::EnvelopeDetector m_release;
			};
		}
	}
}

