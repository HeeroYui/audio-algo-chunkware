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


#ifndef __AUDIO_ALGO_CHUNKWARE_GATE_H__
#define __AUDIO_ALGO_CHUNKWARE_GATE_H__

#include <etk/types.h>
#include <audio/algo/chunkware/AttRelEnvelope.h>
#include <audio/algo/chunkware/Gain.h>

namespace audio {
	namespace algo {
		namespace chunkware {
			class Gate : public AttRelEnvelope {
				public:
					Gate();
					virtual ~Gate() {}
					// parameters
					virtual void setThresh(double _dB);
					virtual double getThresh() const {
						return m_threshdB;
					}
					// runtime
					// call before runtime (in resume())
					virtual void initRuntime();
					// gate runtime process
					void process(double& _in1, double& _in2);
					// with stereo-linked key in
					void process(double& _in1, double& _in2, double _keyLinked);
				private:
					// transfer function
					double m_threshdB; //!< threshold (dB)
					double m_threshold; //!< threshold (linear)
					// runtime variables
					double m_overThresholdEnvelope; //!< over-threshold envelope (linear)
			};
		}
	}
}

#endif
