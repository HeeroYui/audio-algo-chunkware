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


#include <audio/algo/chunkware/Gate.h>

audio::algo::chunkware::Gate::Gate() :
  AttRelEnvelope(1.0, 100.0),
  m_threshdB(0.0),
  m_threshold(1.0),
  m_overThresholdEnvelope(DC_OFFSET) {
	
}

void audio::algo::chunkware::Gate::setThresh(double dB) {
	m_threshdB = dB;
	m_threshold = dB2lin(dB);
}

void audio::algo::chunkware::Gate::initRuntime() {
	m_overThresholdEnvelope = DC_OFFSET;
}


void audio::algo::chunkware::Gate::process(double &in1, double &in2) {
	// create sidechain
	double rect1 = fabs(in1);	// rectify input
	double rect2 = fabs(in2);
	/* if desired, one could use another EnvelopeDetector to smooth
	 * the rectified signal.
	 */
	double link = std::max(rect1, rect2);	// link channels with greater of 2
	process(in1, in2, link);	// rest of process
}

void audio::algo::chunkware::Gate::process(double &in1, double &in2, double keyLinked) {
	keyLinked = fabs(keyLinked);	// rectify (just in case)
	// threshold
	// key over threshold (0.0 or 1.0)
	double over = double(keyLinked > m_threshold);
	// attack/release
	over += DC_OFFSET;					// add DC offset to avoid denormal
	AttRelEnvelope::run(over, m_overThresholdEnvelope);	// run attack/release
	over = m_overThresholdEnvelope - DC_OFFSET;			// subtract DC offset
	/* REGARDING THE DC OFFSET: In this case, since the offset is added before 
	 * the attack/release processes, the envelope will never fall below the offset,
	 * thereby avoiding denormals. However, to prevent the offset from causing
	 * constant gain reduction, we must subtract it from the envelope, yielding
	 * a minimum value of 0dB.
	 */
	// output gain
	in1 *= over;	// apply gain reduction to input
	in2 *= over;
}
