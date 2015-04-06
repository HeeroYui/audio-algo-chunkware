#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "audio_algo_aec_test: test for LMS ALGO"


def create(target):
	myModule = module.Module(__file__, 'audio_algo_chunkware_test', 'BINARY')
	myModule.add_src_file([
		'test/main.cpp',
		'test/debug.cpp'
		])
	myModule.add_module_depend(['audio_algo_chunkware'])
	return myModule









