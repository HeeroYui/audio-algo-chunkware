#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug

def get_desc():
	return "chunkware algo"


def create(target):
	my_module = module.Module(__file__, 'audio-algo-chunkware', 'LIBRARY')
	my_module.add_src_file([
		'audio/algo/chunkware/debug.cpp',
		'audio/algo/chunkware/Compressor.cpp',
		'audio/algo/chunkware/debug.cpp',
		'audio/algo/chunkware/EnvelopeDetector.cpp',
		'audio/algo/chunkware/AttRelEnvelope.cpp',
		'audio/algo/chunkware/Gate.cpp',
		'audio/algo/chunkware/GateRms.cpp',
		'audio/algo/chunkware/Limiter.cpp'
		])
	my_module.add_header_file([
		'audio/algo/chunkware/Compressor.h',
		'audio/algo/chunkware/debug.h',
		'audio/algo/chunkware/EnvelopeDetector.h',
		'audio/algo/chunkware/AttRelEnvelope.h',
		'audio/algo/chunkware/Gate.h',
		'audio/algo/chunkware/GateRms.h',
		'audio/algo/chunkware/Limiter.h'
		])
	my_module.add_module_depend(['etk', 'audio'])
	my_module.add_path(tools.get_current_path(__file__))
	# return module
	return my_module









