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


#include <audio/algo/chunkware/Limiter.h>
#include <audio/algo/chunkware/debug.h>

audio::algo::chunkware::Limiter::Limiter() :
  m_threshdB(0.0),
  m_threshold(1.0),
  m_peakHold(0),
  m_peakTimer(0),
  m_maxPeak(1.0),
  m_attack(1.0),
  m_release(10.0),
  m_overThresholdEnvelope(1.0),
  m_bufferMask(BUFFER_SIZE-1),
  m_cursor(0) {
	setAttack(1.0);
	m_outputBuffer[ 0 ].resize(BUFFER_SIZE, 0.0);
	m_outputBuffer[ 1 ].resize(BUFFER_SIZE, 0.0);
}

void audio::algo::chunkware::Limiter::setThresh(double _dB) {
	m_threshdB = _dB;
	m_threshold = dB2lin(_dB);
}

void audio::algo::chunkware::Limiter::setAttack(double _ms) {
	unsigned int samp = int(0.001 * _ms * m_attack.getSampleRate());
	AA_CHUNK_ASSERT(samp < BUFFER_SIZE, "input function error");
	m_peakHold = samp;
	m_attack.setTc(_ms);
}

void audio::algo::chunkware::Limiter::setRelease(double _ms) {
	m_release.setTc(_ms);
}

void audio::algo::chunkware::Limiter::setSampleRate(double _sampleRate) {
	m_attack.setSampleRate(_sampleRate);
	m_release.setSampleRate(_sampleRate);
}

void audio::algo::chunkware::Limiter::initRuntime() {
	m_peakTimer = 0;
	m_maxPeak = m_threshold;
	m_overThresholdEnvelope = m_threshold;
	m_cursor = 0;
	m_outputBuffer[ 0 ].assign(BUFFER_SIZE, 0.0);
	m_outputBuffer[ 1 ].assign(BUFFER_SIZE, 0.0);
}

void audio::algo::chunkware::Limiter::FastEnvelope::setCoef() {
	// rises to 99% of in value over duration of time constant
	m_coefficient = pow(0.01, (1000.0 / (m_timeMs * m_sampleRate)));
}


void audio::algo::chunkware::Limiter::process(double& _in1, double& _in2) {
	// create sidechain
	double rect1 = fabs(_in1); // rectify input
	double rect2 = fabs(_in2);
	double keyLink = std::max(rect1, rect2); // link channels with greater of 2
	// threshold
	// we always want to feed the sidechain AT LEATS the threshold value
	if (keyLink < m_threshold)
		keyLink = m_threshold;
	// test:
	// a) whether peak timer has "expired"
	// b) whether new peak is greater than previous max peak
	if ((++m_peakTimer >= m_peakHold) || (keyLink > m_maxPeak)) {
		// if either condition is met:
		m_peakTimer = 0; // reset peak timer
		m_maxPeak = keyLink; // assign new peak to max peak
	}
	/* REGARDING THE MAX PEAK: This method assumes that the only important
	 * sample in a look-ahead buffer would be the highest peak. As such,
	 * instead of storing all samples in a look-ahead buffer, it only stores
	 * the max peak, and compares all incoming samples to that one.
	 * The max peak has a hold time equal to what the look-ahead buffer
	 * would have been, which is tracked by a timer (counter). When this
	 * timer expires, the sample would have exited from the buffer. Therefore,
	 * a new sample must be assigned to the max peak. We assume that the next
	 * highest sample in our theoretical buffer is the current input sample.
	 * In reality, we know this is probably NOT the case, and that there has
	 * been another sample, slightly lower than the one before it, that has
	 * passed the input. If we do not account for this possibility, our gain
	 * reduction could be insufficient, resulting in an "over" at the output.
	 * To remedy this, we simply apply a suitably long release stage in the
	 * envelope follower.
	 */
	// attack/release
	if (m_maxPeak > m_overThresholdEnvelope) {
		// run attack phase
		m_attack.run(m_maxPeak, m_overThresholdEnvelope);
	} else {
		// run release phase
		m_release.run(m_maxPeak, m_overThresholdEnvelope);
	}
	/* REGARDING THE ATTACK: This limiter achieves "look-ahead" detection
	 * by allowing the envelope follower to attack the max peak, which is
	 * held for the duration of the attack phase -- unless a new, higher
	 * peak is detected. The output signal is buffered so that the gain
	 * reduction is applied in advance of the "offending" sample.
	 */
	/* NOTE: a DC offset is not necessary for the envelope follower,
	 * as neither the max peak nor envelope should fall below the
	 * threshold (which is assumed to be around 1.0 linear).
	 */
	// gain reduction
	double gR = m_threshold / m_overThresholdEnvelope;
	// unload current buffer index
	// (m_cursor - delay) & m_bufferMask gets sample from [delay] samples ago
	// m_bufferMask variable wraps index
	unsigned int delayIndex = (m_cursor - m_peakHold) & m_bufferMask;
	double delay1 = m_outputBuffer[0][delayIndex];
	double delay2 = m_outputBuffer[1][delayIndex];
	// load current buffer index and advance current index
	// m_bufferMask wraps m_cursor index
	m_outputBuffer[0][m_cursor] = _in1;
	m_outputBuffer[1][m_cursor] = _in2;
	++m_cursor &= m_bufferMask;
	// output gain
	_in1 = delay1 * gR;	// apply gain reduction to input
	_in2 = delay2 * gR;
	/* REGARDING THE GAIN REDUCTION: Due to the logarithmic nature
	 * of the attack phase, the sidechain will never achieve "full"
	 * attack. (Actually, it is only guaranteed to achieve 99% of
	 * the input value over the given time constant.) As such, the
	 * limiter cannot achieve "brick-wall" limiting. There are 2
	 * workarounds:
	 *
	 * 1) Set the threshold slightly lower than the desired threshold.
	 *		i.e. 0.0dB -> -0.1dB or even -0.5dB
	 *
	 * 2) Clip the output at the threshold, as such:
	 *
	 *		if (in1 > m_threshold) in1 = m_threshold;
	 *		else if (in1 < -m_threshold) in1 = -m_threshold;
	 *
	 *		if (in2 > m_threshold) in2 = m_threshold;
	 *		else if (in2 < -m_threshold) in2 = -m_threshold;
	 *
	 *		(... or replace with your favorite branchless clipper ...)
	 */
}