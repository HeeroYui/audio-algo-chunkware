#!/usr/bin/python
import lutin.debug as debug
import lutin.tools as tools


def get_type():
	return "LIBRARY"

def get_desc():
	return "chunkware algo"

def get_licence():
	return "BSD-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def get_version():
	return "version.txt"

def configure(target, my_module):
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
		'audio/algo/chunkware/Compressor.hpp',
		'audio/algo/chunkware/debug.hpp',
		'audio/algo/chunkware/EnvelopeDetector.hpp',
		'audio/algo/chunkware/AttRelEnvelope.hpp',
		'audio/algo/chunkware/Gate.hpp',
		'audio/algo/chunkware/GateRms.hpp',
		'audio/algo/chunkware/Gain.hpp',
		'audio/algo/chunkware/Limiter.hpp'
		])
	my_module.add_depend(['etk', 'audio'])
	my_module.add_path(".")
	return True









