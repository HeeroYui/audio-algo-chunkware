/** @file
 * @author Edouard DUPIN 
 * @copyright 2015, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <test/debug.h>
#include <etk/etk.h>
#include <audio/algo/chunkware/Compressor.h>
#include <audio/algo/chunkware/Limiter.h>
#include <audio/algo/chunkware/Gate.h>
#include <etk/os/FSNode.h>
#include <etk/chrono.h>

#include <unistd.h>
#undef __class__
#define __class__ "test"


static std::vector<double> convert(const std::vector<int16_t>& _data) {
	std::vector<double> out;
	out.resize(_data.size(), 0.0);
	for (size_t iii=0; iii<_data.size(); ++iii) {
		out[iii] = _data[iii];
		out[iii] /= 32768.0;
		out[iii] *= 2.1;
		//APPL_INFO(" in=" << _data[iii] << " => " << out[iii]);
	}
	return out;
}

static std::vector<int16_t> convert(const std::vector<double>& _data) {
	std::vector<int16_t> out;
	out.resize(_data.size(), 0.0);
	for (size_t iii=0; iii<_data.size(); ++iii) {
		out[iii] = int16_t(std::avg(-32768.0, _data[iii]*32768.0, 32767.0));
	}
	return out;
}


class Performance {
	private:
		std11::chrono::steady_clock::time_point m_timeStart;
		std11::chrono::steady_clock::time_point m_timeStop;
		std11::chrono::nanoseconds m_totalTimeProcessing;
		std11::chrono::nanoseconds m_minProcessing;
		std11::chrono::nanoseconds m_maxProcessing;
		int32_t m_totalIteration;
	public:
		Performance() :
		  m_totalTimeProcessing(0),
		  m_minProcessing(99999999999999LL),
		  m_maxProcessing(0),
		  m_totalIteration(0) {
			
		}
		void tic() {
			m_timeStart = std11::chrono::steady_clock::now();
		}
		void toc() {
			m_timeStop = std11::chrono::steady_clock::now();
			std11::chrono::nanoseconds time = m_timeStop - m_timeStart;
			m_minProcessing = std::min(m_minProcessing, time);
			m_maxProcessing = std::max(m_maxProcessing, time);
			m_totalTimeProcessing += time;
			m_totalIteration++;
			
		}
		
		std11::chrono::nanoseconds getTotalTimeProcessing() {
			return m_totalTimeProcessing;
		}
		std11::chrono::nanoseconds getMinProcessing() {
			return m_minProcessing;
		}
		std11::chrono::nanoseconds getMaxProcessing() {
			return m_maxProcessing;
		}
		int32_t getTotalIteration() {
			return m_totalIteration;
		}
		
};

void performanceCompressor() {
	std::vector<double> input;
	input.resize(8192, 0);
	std::vector<double> output;
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
	APPL_PRINT("Start compressor performance ...");
	Performance perfo;
	audio::algo::chunkware::Compressor algo;
	algo.setThreshold(-10);
	algo.setRatio(-5);
	int32_t lastPourcent = -1;
	for (int32_t iii=0; iii<4096; ++iii) {
		perfo.tic();
		algo.process(&output[0], &input[0], input.size(), 1, audio::format_double);
		perfo.toc();
		usleep(1000);
	}
	APPL_INFO("Performance Compressor (double): ");
	APPL_INFO("    blockSize=" << input.size() << " sample");
	APPL_INFO("    min < avg < max =" << perfo.getMinProcessing().count() << "ns < "
	                                  << perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration() << "ns < "
	                                  << perfo.getMaxProcessing().count() << "ns ");
	APPL_INFO("    min < avg < max= " << (float((perfo.getMinProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                  << (float(((perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration())*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                  << (float((perfo.getMaxProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "%");
}

void performanceLimiter() {
	std::vector<double> input;
	input.resize(8192, 0);
	std::vector<double> output;
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
	APPL_PRINT("Start Limiter performance ...");
	Performance perfo;
	audio::algo::chunkware::Limiter algo;
	algo.setSampleRate(48000);
	algo.setThreshold(0);
	algo.setAttack(0.1);
	algo.setRelease(2);
	algo.init(1);
	int32_t lastPourcent = -1;
	for (int32_t iii=0; iii<4096; ++iii) {
		perfo.tic();
		algo.process(&output[0], &input[0], input.size(), 1, audio::format_double);
		perfo.toc();
		usleep(1000);
	}
	APPL_INFO("Performance Limiter (double): ");
	APPL_INFO("    blockSize=" << input.size() << " sample");
	APPL_INFO("    min < avg < max =" << perfo.getMinProcessing().count() << "ns < "
	                                  << perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration() << "ns < "
	                                  << perfo.getMaxProcessing().count() << "ns ");
	APPL_INFO("    min < avg < max = " << (float((perfo.getMinProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                   << (float(((perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration())*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                   << (float((perfo.getMaxProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "%");
}

void performanceGate() {
	std::vector<double> input;
	input.resize(8192, 0);
	std::vector<double> output;
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
	APPL_PRINT("Start Gate performance ...");
	Performance perfo;
	audio::algo::chunkware::Gate algo;
	algo.setSampleRate(48000);
	algo.setThreshold(0);
	algo.setAttack(0.1);
	algo.setRelease(2);
	algo.init();
	int32_t lastPourcent = -1;
	for (int32_t iii=0; iii<4096; ++iii) {
		perfo.tic();
		algo.process(&output[0], &input[0], input.size(), 1, audio::format_double);
		perfo.toc();
		usleep(1000);
	}
	APPL_INFO("Performance Gate (double): ");
	APPL_INFO("    blockSize=" << input.size() << " sample");
	APPL_INFO("    min < avg < max =" << perfo.getMinProcessing().count() << "ns < "
	                                  << perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration() << "ns < "
	                                  << perfo.getMaxProcessing().count() << "ns ");
	APPL_INFO("    min < avg < max = " << (float((perfo.getMinProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                   << (float(((perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration())*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                   << (float((perfo.getMaxProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "%");
}




int main(int _argc, const char** _argv) {
	// the only one init for etk:
	etk::init(_argc, _argv);
	std::string inputName = "";
	bool performance = false;
	bool perf = false;
	int64_t sampleRate = 48000;
	for (int32_t iii=0; iii<_argc ; ++iii) {
		std::string data = _argv[iii];
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
			APPL_PRINT("Help : ");
			APPL_PRINT("    ./xxx --fb=file.raw --mic=file.raw");
			APPL_PRINT("        --in=YYY.raw        inout file");
			APPL_PRINT("        --performance       Generate signal to force algo to maximum process time");
			APPL_PRINT("        --perf              Enable performence test (little slower but real performence test)");
			APPL_PRINT("        --sample-rate=XXXX  Signal sample rate (default 48000)");
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
		APPL_ERROR("Can not Process missing parameters...");
		exit(-1);
	}
	APPL_INFO("Read input:");
	std::vector<double> inputData = convert(etk::FSNodeReadAllDataType<int16_t>(inputName));
	APPL_INFO("    " << inputData.size() << " samples");
	// resize output :
	std::vector<double> output;
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
			APPL_INFO("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize << " " << lastPourcent << "/100");
		} else {
			APPL_VERBOSE("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize);
		}
		perfo.tic();
		algo.process(audio::format_double, &output[iii*blockSize], &inputData[iii*blockSize], blockSize, 1);
		if (perf == true) {
			perfo.toc();
			usleep(1000);
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
			APPL_INFO("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize << " " << lastPourcent << "/100");
		} else {
			APPL_VERBOSE("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize);
		}
		perfo.tic();
		algo.process(&output[iii*blockSize], &inputData[iii*blockSize], blockSize, 1, audio::format_double);
		if (perf == true) {
			perfo.toc();
			usleep(1000);
		}
	}
	
	
	if (perf == true) {
		APPL_INFO("Performance Result: ");
		APPL_INFO("    blockSize=" << blockSize << " sample");
		APPL_INFO("    min=" << perfo.getMinProcessing().count() << " ns");
		APPL_INFO("    max=" << perfo.getMaxProcessing().count() << " ns");
		APPL_INFO("    avg=" << perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration() << " ns");
		
		APPL_INFO("    min=" << (float((perfo.getMinProcessing().count()*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_INFO("    max=" << (float((perfo.getMaxProcessing().count()*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_INFO("    avg=" << (float(((perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration())*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
	}
	etk::FSNodeWriteAllDataType<int16_t>("output.raw", convert(output));
	
}

