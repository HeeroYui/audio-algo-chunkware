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

audio::algo::chunkware::Compressor::Compressor() :
  AttRelEnvelope(10.0, 100.0),
  m_isConfigured(false),
  m_thresholddB(0.0),
  m_ratio(1.0),
  m_overThresholdEnvelopeDB(DC_OFFSET) {
	
}

void audio::algo::chunkware::Compressor::setThreshold(double _dB) {
	m_thresholddB = _dB;
}

void audio::algo::chunkware::Compressor::setRatio(double _ratio) {
	m_ratio = _ratio;
}

void audio::algo::chunkware::Compressor::init() {
	m_overThresholdEnvelopeDB = DC_OFFSET;
	m_isConfigured = true;
}

std::vector<enum audio::format> audio::algo::chunkware::Compressor::getSupportedFormat() {
	std::vector<enum audio::format> out = getNativeSupportedFormat();
	out.push_back(audio::format_int16);
	return out;
}

std::vector<enum audio::format> audio::algo::chunkware::Compressor::getNativeSupportedFormat() {
	std::vector<enum audio::format> out;
	out.push_back(audio::format_double);
	return out;
}

void audio::algo::chunkware::Compressor::process(void* _output, const void* _input, size_t _nbChunk, int8_t _nbChannel, enum audio::format _format) {
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

void audio::algo::chunkware::Compressor::processDouble(double* _out, const double* _in, int8_t _nbChannel) {
	double keyLink = 0;
	// get greater value;
	for (int8_t iii=0; iii<_nbChannel; ++iii) {
		double absValue = std::abs(_in[iii]);
		keyLink = std::max(keyLink, absValue);
	}
	processDouble(_out, _in, _nbChannel, keyLink);
}

void audio::algo::chunkware::Compressor::processDouble(double* _out, const double* _in, int8_t _nbChannel, double _keyLinked) {
	// convert key to dB
	_keyLinked += DC_OFFSET; // add DC offset to avoid log(0)
	double keydB = lin2dB(_keyLinked); // convert linear -> dB
	// threshold
	double overdB = keydB - m_thresholddB; // delta over threshold
	if (overdB < 0.0) {
		overdB = 0.0;
	}
	// attack/release
	overdB += DC_OFFSET; // add DC offset to avoid denormal
	audio::algo::chunkware::AttRelEnvelope::run(overdB, m_overThresholdEnvelopeDB); // run attack/release envelope
	overdB = m_overThresholdEnvelopeDB - DC_OFFSET; // subtract DC offset
	/* REGARDING THE DC OFFSET: In this case, since the offset is added before 
	 * the attack/release processes, the envelope will never fall below the offset,
	 * thereby avoiding denormals. However, to prevent the offset from causing
	 * constant gain reduction, we must subtract it from the envelope, yielding
	 * a minimum value of 0dB.
	 */
	// transfer function
	double gr = overdB * (m_ratio - 1.0); // gain reduction (dB)
	gr = dB2lin(gr); // convert dB -> linear
	// output gain
	for (int8_t iii=0; iii<_nbChannel; ++iii) {
		_out[iii] = _in[iii] * gr;
	}
}

