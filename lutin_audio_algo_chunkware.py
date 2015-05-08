#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug

def get_desc():
	return "audio_algo_aec : AEC basic algo"


def create(target):
	myModule = module.Module(__file__, 'audio_algo_chunkware', 'LIBRARY')
	myModule.add_src_file([
		'audio/algo/chunkware/debug.cpp',
		'audio/algo/chunkware/Compressor.cpp',
		'audio/algo/chunkware/debug.cpp',
		'audio/algo/chunkware/EnvelopeDetector.cpp',
		'audio/algo/chunkware/AttRelEnvelope.cpp',
		'audio/algo/chunkware/Gate.cpp',
		'audio/algo/chunkware/GateRms.cpp',
		'audio/algo/chunkware/Limiter.cpp'
		])
	myModule.add_module_depend(['etk', 'audio'])
	myModule.add_export_path(tools.get_current_path(__file__))
	# return module
	return myModule









