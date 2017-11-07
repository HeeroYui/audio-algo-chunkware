/** @file
 * @author Edouard DUPIN 
 * @copyright 2015, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <test-debug/debug.hpp>
#include <etk/etk.hpp>
#include <audio/algo/chunkware/Compressor.hpp>
#include <audio/algo/chunkware/Limiter.hpp>
#include <audio/algo/chunkware/Gate.hpp>
#include <etk/os/FSNode.hpp>
#include <echrono/Steady.hpp>
#include <ethread/Thread.hpp>
#include <ethread/tools.hpp>



static etk::Vector<double> convert(const etk::Vector<int16_t>& _data) {
	etk::Vector<double> out;
	out.resize(_data.size(), 0.0);
	for (size_t iii=0; iii<_data.size(); ++iii) {
		out[iii] = _data[iii];
		out[iii] /= 32768.0;
		out[iii] *= 2.1;
		//TEST_INFO(" in=" << _data[iii] << " => " << out[iii]);
	}
	return out;
}

static etk::Vector<int16_t> convert(const etk::Vector<double>& _data) {
	etk::Vector<int16_t> out;
	out.resize(_data.size(), 0.0);
	for (size_t iii=0; iii<_data.size(); ++iii) {
		out[iii] = int16_t(etk::avg(-32768.0, _data[iii]*32768.0, 32767.0));
	}
	return out;
}


class Performance {
	private:
		echrono::Steady m_timeStart;
		echrono::Steady m_timeStop;
		echrono::Duration m_totalTimeProcessing;
		echrono::Duration m_minProcessing;
		echrono::Duration m_maxProcessing;
		int32_t m_totalIteration;
	public:
		Performance() :
		  m_totalTimeProcessing(0),
		  m_minProcessing(int64_t(99999999999999LL)),
		  m_maxProcessing(0),
		  m_totalIteration(0) {
			
		}
		void tic() {
			m_timeStart = echrono::Steady::now();
		}
		void toc() {
			m_timeStop = echrono::Steady::now();
			echrono::Duration time = m_timeStop - m_timeStart;
			m_minProcessing = etk::min(m_minProcessing, time);
			m_maxProcessing = etk::max(m_maxProcessing, time);
			m_totalTimeProcessing += time;
			m_totalIteration++;
			
		}
		
		echrono::Duration getTotalTimeProcessing() {
			return m_totalTimeProcessing;
		}
		echrono::Duration getMinProcessing() {
			return m_minProcessing;
		}
		echrono::Duration getMaxProcessing() {
			return m_maxProcessing;
		}
		int32_t getTotalIteration() {
			return m_totalIteration;
		}
		
};

void performanceCompressor() {
	etk::Vector<double> input;
	input.resize(8192, 0);
	etk::Vector<double> output;
	output.resize(8192, 0);
	double sampleRate = 48000.0;
	{
		double phase = 0;
		double baseCycle = 2.0*M_PI/sampleRate * 1280.0;
		for (int32_t iii=0; iii<input.size(); iii++) {
			input[iii] = cos(phase) * 5.0;
			phase += baseCycle;
			if (phase >= 2*M_PI) {
				phase -= 2*M_PI;
			}
		}
	}
	TEST_PRINT("Start compressor performance ...");
	Performance perfo;
	audio::algo::chunkware::Compressor algo;
	algo.setThreshold(-10);
	algo.setRatio(-5);
	algo.init();
	for (int32_t iii=0; iii<4096; ++iii) {
		perfo.tic();
		algo.process(&output[0], &input[0], input.size(), 1, audio::format_double);
		perfo.toc();
		ethread::sleepMilliSeconds((1));
	}
	TEST_PRINT("Performance Compressor (double): ");
	TEST_PRINT("    blockSize=" << input.size() << " sample");
	TEST_PRINT("    min < avg < max =" << perfo.getMinProcessing() << " < "
	                                   << perfo.getTotalTimeProcessing().get()/perfo.getTotalIteration() << "ns < "
	                                   << perfo.getMaxProcessing());
	TEST_PRINT("    min < avg < max= " << (float((perfo.getMinProcessing().get()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                   << (float(((perfo.getTotalTimeProcessing().get()/perfo.getTotalIteration())*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                   << (float((perfo.getMaxProcessing().get()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "%");
}

void performanceLimiter() {
	etk::Vector<double> input;
	input.resize(8192, 0);
	etk::Vector<double> output;
	output.resize(8192, 0);
	double sampleRate = 48000.0;
	{
		double phase = 0;
		double baseCycle = 2.0*M_PI/sampleRate * 1280.0;
		for (int32_t iii=0; iii<input.size(); iii++) {
			input[iii] = cos(phase) * 5.0;
			phase += baseCycle;
			if (phase >= 2*M_PI) {
				phase -= 2*M_PI;
			}
		}
	}
	TEST_PRINT("Start Limiter performance ...");
	Performance perfo;
	audio::algo::chunkware::Limiter algo;
	algo.setSampleRate(48000);
	algo.setThreshold(0);
	algo.setAttack(0.1);
	algo.setRelease(2);
	algo.init(1);
	for (int32_t iii=0; iii<4096; ++iii) {
		perfo.tic();
		algo.process(&output[0], &input[0], input.size(), 1, audio::format_double);
		perfo.toc();
		ethread::sleepMilliSeconds((1));
	}
	TEST_PRINT("Performance Limiter (double): ");
	TEST_PRINT("    blockSize=" << input.size() << " sample");
	TEST_PRINT("    min < avg < max =" << perfo.getMinProcessing() << " < "
	                                   << perfo.getTotalTimeProcessing().get()/perfo.getTotalIteration() << "ns < "
	                                   << perfo.getMaxProcessing());
	TEST_PRINT("    min < avg < max = " << (float((perfo.getMinProcessing().get()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                    << (float(((perfo.getTotalTimeProcessing().get()/perfo.getTotalIteration())*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                    << (float((perfo.getMaxProcessing().get()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "%");
}

void performanceGate() {
	etk::Vector<double> input;
	input.resize(8192, 0);
	etk::Vector<double> output;
	output.resize(8192, 0);
	double sampleRate = 48000.0;
	{
		double phase = 0;
		double baseCycle = 2.0*M_PI/sampleRate * 1280.0;
		for (int32_t iii=0; iii<input.size(); iii++) {
			input[iii] = cos(phase) * 5.0;
			phase += baseCycle;
			if (phase >= 2*M_PI) {
				phase -= 2*M_PI;
			}
		}
	}
	TEST_PRINT("Start Gate performance ...");
	Performance perfo;
	audio::algo::chunkware::Gate algo;
	algo.setSampleRate(48000);
	algo.setThreshold(0);
	algo.setAttack(0.1);
	algo.setRelease(2);
	algo.init();
	for (int32_t iii=0; iii<4096; ++iii) {
		perfo.tic();
		algo.process(&output[0], &input[0], input.size(), 1, audio::format_double);
		perfo.toc();
		ethread::sleepMilliSeconds((1));
	}
	TEST_PRINT("Performance Gate (double): ");
	TEST_PRINT("    blockSize=" << input.size() << " sample");
	TEST_PRINT("    min < avg < max =" << perfo.getMinProcessing() << " < "
	                                   << perfo.getTotalTimeProcessing().get()/perfo.getTotalIteration() << "ns < "
	                                   << perfo.getMaxProcessing());
	TEST_PRINT("    min < avg < max = " << (float((perfo.getMinProcessing().get()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                    << (float(((perfo.getTotalTimeProcessing().get()/perfo.getTotalIteration())*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                    << (float((perfo.getMaxProcessing().get()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "%");
}




int main(int _argc, const char** _argv) {
	// the only one init for etk:
	etk::init(_argc, _argv);
	etk::String inputName = "";
	bool performance = false;
	bool perf = false;
	int64_t sampleRate = 48000;
	for (int32_t iii=0; iii<_argc ; ++iii) {
		etk::String data = _argv[iii];
		if (etk::start_with(data,"--in=")) {
			inputName = &data[5];
		} else if (data == "--performance") {
			performance = true;
		} else if (data == "--perf") {
			perf = true;
		} else if (etk::start_with(data,"--sample-rate=")) {
			data = &data[14];
			sampleRate = etk::string_to_int32_t(data);
		} else if (    data == "-h"
		            || data == "--help") {
			TEST_PRINT("Help : ");
			TEST_PRINT("    ./xxx --fb=file.raw --mic=file.raw");
			TEST_PRINT("        --in=YYY.raw        inout file");
			TEST_PRINT("        --performance       Generate signal to force algo to maximum process time");
			TEST_PRINT("        --perf              Enable performence test (little slower but real performence test)");
			TEST_PRINT("        --sample-rate=XXXX  Signal sample rate (default 48000)");
			exit(0);
		}
	}
	// PERFORMANCE test only ....
	if (performance == true) {
		performanceCompressor();
		performanceLimiter();
		performanceGate();
		return 0;
	}
	if (inputName == "") {
		TEST_ERROR("Can not Process missing parameters...");
		exit(-1);
	}
	TEST_INFO("Read input:");
	etk::Vector<double> inputData = convert(etk::FSNodeReadAllDataType<int16_t>(inputName));
	TEST_INFO("    " << inputData.size() << " samples");
	// resize output :
	etk::Vector<double> output;
	output.resize(inputData.size(), 0);
	// process in chunk of 256 samples
	int32_t blockSize = 256;
	
	Performance perfo;
	/*
	audio::algo::chunkware::Compressor algo;
	algo.setThreshold(-10);
	algo.setRatio(-5);
	int32_t lastPourcent = -1;
	for (int32_t iii=0; iii<output.size()/blockSize; ++iii) {
		if (lastPourcent != 100*iii / (output.size()/blockSize)) {
			lastPourcent = 100*iii / (output.size()/blockSize);
			TEST_INFO("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize << " " << lastPourcent << "/100");
		} else {
			TEST_VERBOSE("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize);
		}
		perfo.tic();
		algo.process(audio::format_double, &output[iii*blockSize], &inputData[iii*blockSize], blockSize, 1);
		if (perf == true) {
			perfo.toc();
			ethread::sleepMilliSeconds((1));
		}
	}
	*/
	audio::algo::chunkware::Limiter algo;
	algo.setSampleRate(48000);
	algo.setThreshold(0);
	algo.setAttack(0.1);
	algo.setRelease(2);
	algo.init(1);
	int32_t lastPourcent = -1;
	for (int32_t iii=0; iii<output.size()/blockSize; ++iii) {
		if (lastPourcent != 100*iii / (output.size()/blockSize)) {
			lastPourcent = 100*iii / (output.size()/blockSize);
			TEST_INFO("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize << " " << lastPourcent << "/100");
		} else {
			TEST_VERBOSE("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize);
		}
		perfo.tic();
		algo.process(&output[iii*blockSize], &inputData[iii*blockSize], blockSize, 1, audio::format_double);
		if (perf == true) {
			perfo.toc();
			ethread::sleepMilliSeconds((1));
		}
	}
	
	
	if (perf == true) {
		TEST_INFO("Performance Result: ");
		TEST_INFO("    blockSize=" << blockSize << " sample");
		TEST_INFO("    min=" << perfo.getMinProcessing());
		TEST_INFO("    max=" << perfo.getMaxProcessing());
		TEST_INFO("    avg=" << perfo.getTotalTimeProcessing().get()/perfo.getTotalIteration() << " ns");
		
		TEST_INFO("    min=" << (float((perfo.getMinProcessing().get()*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		TEST_INFO("    max=" << (float((perfo.getMaxProcessing().get()*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		TEST_INFO("    avg=" << (float(((perfo.getTotalTimeProcessing().get()/perfo.getTotalIteration())*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
	}
	etk::FSNodeWriteAllDataType<int16_t>("output.raw", convert(output));
	
}

