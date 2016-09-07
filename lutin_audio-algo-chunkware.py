#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug


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
	return ["Mr DUPIN Edouard <yui.heero@gmail.com>"]

def get_version():
	return [0,1,"dev"]

def create(target, module_name):
	my_module = module.Module(__file__, module_name, get_type())
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
		'audio/algo/chunkware/Gain.h',
		'audio/algo/chunkware/Limiter.h'
		])
	my_module.add_depend(['etk', 'audio'])
	my_module.add_path(tools.get_current_path(__file__))
	return my_module









