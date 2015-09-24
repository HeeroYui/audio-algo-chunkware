#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug

def get_desc():
	return "test chunkware"


def create(target):
	my_module = module.Module(__file__, 'audio-algo-chunkware-test', 'BINARY')
	my_module.add_src_file([
		'test/main.cpp',
		'test/debug.cpp'
		])
	my_module.add_module_depend(['audio-algo-chunkware'])
	return my_module









