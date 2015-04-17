/**
 * @author Bojan MARKOVIC
 * @author Edouard DUPIN
 * @copyright 2006, ChunkWare Music Software, OPEN-SOURCE
 * @license BSD-1 (see license file)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __AUDIO_ALGO_CHUNKWARE_DEBUG_H__
#define __AUDIO_ALGO_CHUNKWARE_DEBUG_H__

#include <etk/log.h>

namespace audio {
	namespace algo {
		namespace chunkware {
			int32_t getLogId();
		}
	}
}

#define AA_CHUNK_BASE(info,data) TK_LOG_BASE(audio::algo::chunkware::getLogId(),info,data)

#define AA_CHUNK_PRINT(data)      AA_CHUNK_BASE(-1, data)
#define AA_CHUNK_CRITICAL(data)      AA_CHUNK_BASE(1, data)
#define AA_CHUNK_ERROR(data)         AA_CHUNK_BASE(2, data)
#define AA_CHUNK_WARNING(data)       AA_CHUNK_BASE(3, data)
#ifdef DEBUG
	#define AA_CHUNK_INFO(data)          AA_CHUNK_BASE(4, data)
	#define AA_CHUNK_DEBUG(data)         AA_CHUNK_BASE(5, data)
	#define AA_CHUNK_VERBOSE(data)       AA_CHUNK_BASE(6, data)
	#define AA_CHUNK_TODO(data)          AA_CHUNK_BASE(4, "TODO : " << data)
#else
	#define AA_CHUNK_INFO(data)          do { } while(false)
	#define AA_CHUNK_DEBUG(data)         do { } while(false)
	#define AA_CHUNK_VERBOSE(data)       do { } while(false)
	#define AA_CHUNK_TODO(data)          do { } while(false)
#endif

#define AA_CHUNK_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			AA_CHUNK_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)

#endif
