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
#include <audio/algo/chunkware/debug.h>
#include <cmath>

audio::algo::chunkware::Gate::Gate() :
  AttRelEnvelope(1.0, 100.0),
  m_thresholddB(0.0),
  m_threshold(1.0),
  m_overThresholdEnvelope(DC_OFFSET) {
	
}

void audio::algo::chunkware::Gate::setThreshold(double _dB) {
	m_thresholddB = _dB;
	m_threshold = dB2lin(_dB);
}


void audio::algo::chunkware::Gate::init() {
	m_overThresholdEnvelope = DC_OFFSET;
	m_isConfigured = true;
}

std::vector<enum audio::format> audio::algo::chunkware::Gate::getSupportedFormat() {
	std::vector<enum audio::format> out = getNativeSupportedFormat();
	out.push_back(audio::format_int16);
	return out;
}

std::vector<enum audio::format> audio::algo::chunkware::Gate::getNativeSupportedFormat() {
	std::vector<enum audio::format> out;
	out.push_back(audio::format_double);
	return out;
}

void audio::algo::chunkware::Gate::process(void* _output, const void* _input, size_t _nbChunk, int8_t _nbChannel, enum audio::format _format) {
	// TODO : Check init ...
	if (_nbChannel != 1) {
		AA_CHUNK_ERROR("Can not compress with Other than single channel: " << _nbChannel);
	}
	switch (_format) {
		case audio::format_int16:
			{
				const int16_t* input = reinterpret_cast<const int16_t*>(_input);
				int16_t* output = reinterpret_cast<int16_t*>(_output);
				double vals[_nbChannel];
				for (size_t iii=0; iii<_nbChunk ; ++iii) {
					for (int8_t kkk=0; kkk<_nbChannel ; ++kkk) {
						vals[kkk] = double(input[iii*_nbChannel+kkk]) / 32768.0;
					}
					processDouble(vals, vals, _nbChannel);
					for (int8_t kkk=0; kkk<_nbChannel ; ++kkk) {
						vals[kkk] *= 32768.0;
						output[iii*_nbChannel+kkk] = int16_t(std::avg(-32768.0, vals[kkk], 32767.0));
					}
				}
			}
			break;
		case audio::format_double:
			{
				const double* input = reinterpret_cast<const double*>(_input);
				double* output = reinterpret_cast<double*>(_output);
				for (size_t iii=0; iii<_nbChunk ; ++iii) {
					processDouble(&output[iii*_nbChannel], &input[iii*_nbChannel], _nbChannel);
					//AA_CHUNK_INFO(" in=" << input[iii] << " => " << output[iii]);
				}
			}
			break;
		default:
			AA_CHUNK_ERROR("Can not compress with unsupported format : " << _format);
			return;
	}
	
}

void audio::algo::chunkware::Gate::processDouble(double* _out, const double* _in, int8_t _nbChannel) {
	double keyLink = 0;
	// get greater value;
	for (int8_t iii=0; iii<_nbChannel; ++iii) {
		double absValue = std::abs(_in[iii]);
		keyLink = std::max(keyLink, absValue);
	}
	processDouble(_out, _in, _nbChannel, keyLink);
}

void audio::algo::chunkware::Gate::processDouble(double* _out, const double* _in, int8_t _nbChannel, double _keyLinked) {
	// threshold
	// key over threshold (0.0 or 1.0)
	double over = double(_keyLinked > m_threshold);
	// attack/release
	over += DC_OFFSET; // add DC offset to avoid denormal
	audio::algo::chunkware::AttRelEnvelope::run(over, m_overThresholdEnvelope);	// run attack/release
	over = m_overThresholdEnvelope - DC_OFFSET; // subtract DC offset
	/* REGARDING THE DC OFFSET: In this case, since the offset is added before 
	 * the attack/release processes, the envelope will never fall below the offset,
	 * thereby avoiding denormals. However, to prevent the offset from causing
	 * constant gain reduction, we must subtract it from the envelope, yielding
	 * a minimum value of 0dB.
	 */
	// output gain
	for (int8_t iii=0; iii<_nbChannel; ++iii) {
		_out[iii] = _in[iii] * over;
	}
}
