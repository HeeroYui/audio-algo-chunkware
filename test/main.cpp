/** @file
 * @author Edouard DUPIN 
 * @copyright 2015, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <test/debug.h>
#include <etk/etk.h>
#include <audio/algo/chunkware/Compressor.h>
#include <audio/algo/chunkware/Limiter.h>
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

int main(int _argc, const char** _argv) {
	// the only one init for etk:
	etk::init(_argc, _argv);
	std::string inputName = "";
	bool perf = false;
	int64_t sampleRate = 48000;
	for (int32_t iii=0; iii<_argc ; ++iii) {
		std::string data = _argv[iii];
		if (etk::start_with(data,"--in=")) {
			inputName = &data[5];
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
			APPL_PRINT("        --perf              Enable performence test (little slower but real performence test)");
			APPL_PRINT("        --sample-rate=XXXX  Signal sample rate (default 48000)");
			exit(0);
		}
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
	// for CPU consomtion:
	std11::chrono::nanoseconds totalTimeProcessing(0);
	std11::chrono::nanoseconds minProcessing(99999999999999LL);
	std11::chrono::nanoseconds maxProcessing(0);
	int32_t totalIteration = 0;
	
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
		std11::chrono::steady_clock::time_point timeStart = std11::chrono::steady_clock::now();
		algo.process(audio::format_double, &output[iii*blockSize], &inputData[iii*blockSize], blockSize, 1);
		if (perf == true) {
			std11::chrono::steady_clock::time_point timeEnd = std11::chrono::steady_clock::now();
			std11::chrono::nanoseconds time = timeEnd - timeStart;
			minProcessing = std::min(minProcessing, time);
			maxProcessing = std::max(maxProcessing, time);
			totalTimeProcessing += time;
			totalIteration++;
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
		std11::chrono::steady_clock::time_point timeStart = std11::chrono::steady_clock::now();
		algo.process(&output[iii*blockSize], &inputData[iii*blockSize], blockSize, 1, audio::format_double);
		if (perf == true) {
			std11::chrono::steady_clock::time_point timeEnd = std11::chrono::steady_clock::now();
			std11::chrono::nanoseconds time = timeEnd - timeStart;
			minProcessing = std::min(minProcessing, time);
			maxProcessing = std::max(maxProcessing, time);
			totalTimeProcessing += time;
			totalIteration++;
			usleep(1000);
		}
	}
	
	
	if (perf == true) {
		APPL_INFO("Performance Result: ");
		APPL_INFO("    blockSize=" << blockSize << " sample");
		APPL_INFO("    min=" << minProcessing.count() << " ns");
		APPL_INFO("    max=" << maxProcessing.count() << " ns");
		APPL_INFO("    avg=" << totalTimeProcessing.count()/totalIteration << " ns");
		
		APPL_INFO("    min=" << (float((minProcessing.count()*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_INFO("    max=" << (float((maxProcessing.count()*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_INFO("    avg=" << (float(((totalTimeProcessing.count()/totalIteration)*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
	}
	etk::FSNodeWriteAllDataType<int16_t>("output.raw", convert(output));
	
}

