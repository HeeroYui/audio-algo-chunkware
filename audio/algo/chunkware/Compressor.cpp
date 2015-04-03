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

#include <audio/algo/chunkware/Compressor.h>
#include <audio/algo/chunkware/debug.h>

audio::algo::chunkware::Compresssor::Compresssor() :
  AttRelEnvelope(10.0, 100.0),
  m_threshdB(0.0),
  m_ratio(1.0),
  m_overThresholdEnvelopeDB(DC_OFFSET) {
	
}

void audio::algo::chunkware::Compresssor::setThresh(double dB) {
	m_threshdB = dB;
}

void audio::algo::chunkware::Compresssor::setRatio(double ratio) {
	AA_CHUNK_ASSERT(ratio > 0.0, "input function error");
	m_ratio = ratio;
}

void audio::algo::chunkware::Compresssor::initRuntime() {
	m_overThresholdEnvelopeDB = DC_OFFSET;
}

void audio::algo::chunkware::Compresssor::process(double &in1, double &in2) {
	// create sidechain
	double rect1 = fabs(in1);	// rectify input
	double rect2 = fabs(in2);
	/* if desired, one could use another EnvelopeDetector to smooth
	 * the rectified signal.
	 */
	double link = std::max(rect1, rect2);	// link channels with greater of 2
	process(in1, in2, link);	// rest of process
}

void audio::algo::chunkware::Compresssor::process(double &in1, double &in2, double keyLinked) {
	keyLinked = fabs(keyLinked);		// rectify (just in case)
	// convert key to dB
	keyLinked += DC_OFFSET;				// add DC offset to avoid log(0)
	double keydB = lin2dB(keyLinked);	// convert linear -> dB
	// threshold
	double overdB = keydB - m_threshdB;	// delta over threshold
	if (overdB < 0.0)
		overdB = 0.0;
	// attack/release
	overdB += DC_OFFSET; // add DC offset to avoid denormal
	AttRelEnvelope::run(overdB, m_overThresholdEnvelopeDB); // run attack/release envelope
	overdB = m_overThresholdEnvelopeDB - DC_OFFSET; // subtract DC offset
	/* REGARDING THE DC OFFSET: In this case, since the offset is added before 
	 * the attack/release processes, the envelope will never fall below the offset,
	 * thereby avoiding denormals. However, to prevent the offset from causing
	 * constant gain reduction, we must subtract it from the envelope, yielding
	 * a minimum value of 0dB.
	 */
	// transfer function
	double gr = overdB * (m_ratio - 1.0);	// gain reduction (dB)
	gr = dB2lin(gr);						// convert dB -> linear
	// output gain
	in1 *= gr;	// apply gain reduction to input
	in2 *= gr;
}

