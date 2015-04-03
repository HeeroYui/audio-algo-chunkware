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


#include "AttRelEnvelope.h"

audio::algo::chunkware::AttRelEnvelope::AttRelEnvelope(double att_ms, double rel_ms, double sampleRate) :
  att_(att_ms, sampleRate),
  rel_(rel_ms, sampleRate) {
	
}

//-------------------------------------------------------------
void audio::algo::chunkware::AttRelEnvelope::setAttack(double ms) {
	att_.setTc(ms);
}

//-------------------------------------------------------------
void audio::algo::chunkware::AttRelEnvelope::setRelease(double ms) {
	rel_.setTc(ms);
}

//-------------------------------------------------------------
void audio::algo::chunkware::AttRelEnvelope::setSampleRate(double sampleRate) {
	att_.setSampleRate(sampleRate);
	rel_.setSampleRate(sampleRate);
}

