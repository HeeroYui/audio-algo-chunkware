/*
 *	Simple Gate (header)
 *
 *  File		: SimpleGate.h
 *	Library		: SimpleSource
 *  Version		: 1.12
 *  Class		: SimpleGate, SimpleGateRms
 *
 *	© 2006, ChunkWare Music Software, OPEN-SOURCE
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a
 *	copy of this software and associated documentation files (the "Software"),
 *	to deal in the Software without restriction, including without limitation
 *	the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *	and/or sell copies of the Software, and to permit persons to whom the
 *	Software is furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *	DEALINGS IN THE SOFTWARE.
 */


#ifndef __SIMPLE_GATE_H__
#define __SIMPLE_GATE_H__

#include "header.h"
#include "Envelope.h"
#include "Gain.h"

namespace audio {
	namespace algo {
		namespace chunkware {
			//-------------------------------------------------------------
			// simple gate
			//-------------------------------------------------------------
			class Gate : public AttRelEnvelope {
				public:
					Gate();
					virtual ~Gate() {}
					// parameters
					virtual void setThresh(double dB);
					virtual double getThresh() const {
						return threshdB_;
					}
					// runtime
					// call before runtime (in resume())
					virtual void initRuntime();
					// gate runtime process
					void process(double &in1, double &in2);
					// with stereo-linked key in
					void process(double &in1, double &in2, double keyLinked);
				private:
					// transfer function
					double threshdB_; //!< threshold (dB)
					double thresh_; //!< threshold (linear)
					// runtime variables
					double env_; //!< over-threshold envelope (linear)
			};
			//-------------------------------------------------------------
			// simple gate with RMS detection
			//-------------------------------------------------------------
			class GateRms : public Gate {
				public:
					GateRms();
					virtual ~GateRms() {}
					// sample rate
					virtual void setSampleRate(double sampleRate);
					// RMS window
					virtual void setWindow(double ms);
					virtual double getWindow() const {
						return ave_.getTc();
					}
					// runtime process
					// call before runtime (in resume())
					virtual void initRuntime();
					// gate runtime process
					void process(double &in1, double &in2);
				private:
					EnvelopeDetector ave_; //!< averager
					double aveOfSqrs_; //!< average of squares
			};
		}
	}
}

#endif
