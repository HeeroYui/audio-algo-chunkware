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


#include <audio/algo/chunkware/CompressorRms.h>


audio::algo::chunkware::CompresssorRms::CompresssorRms() :
  m_averager(5.0),
  m_averageSuares(DC_OFFSET) {
	
}

void audio::algo::chunkware::CompresssorRms::setSampleRate(double sampleRate) {
	audio::algo::chunkware::Compresssor::setSampleRate(sampleRate);
	m_averager.setSampleRate(sampleRate);
}

void audio::algo::chunkware::CompresssorRms::setWindow(double ms) {
	m_averager.setTc(ms);
}

void audio::algo::chunkware::CompresssorRms::initRuntime() {
	audio::algo::chunkware::Compresssor::initRuntime();
	m_averageSuares = DC_OFFSET;
}

void audio::algo::chunkware::CompresssorRms::process(double &in1, double &in2) {
	// create sidechain
	double inSq1 = in1 * in1;	// square input
	double inSq2 = in2 * in2;
	double sum = inSq1 + inSq2; // power summing
	sum += DC_OFFSET; // DC offset, to prevent denormal
	m_averager.run(sum, m_averageSuares); // average of squares
	double rms = sqrt(m_averageSuares); // rms (sort of ...)
	/* REGARDING THE RMS AVERAGER: Ok, so this isn't a REAL RMS
	 * calculation. A true RMS is an FIR moving average. This
	 * approximation is a 1-pole IIR. Nonetheless, in practice,
	 * and in the interest of simplicity, this method will suffice,
	 * giving comparable results.
	 */
	// rest of process
	Compresssor::process(in1, in2, rms);
}
