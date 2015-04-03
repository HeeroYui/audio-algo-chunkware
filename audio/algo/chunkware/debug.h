/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __APPL_DEBUG_H__
#define __APPL_DEBUG_H__

#include <etk/log.h>

namespace audio {
	namespace algo {
		namespace aec {
			int32_t getLogId();
		}
	}
}

#define AA_AEC_BASE(info,data) TK_LOG_BASE(audio::algo::aec::getLogId(),info,data)

#define AA_AEC_CRITICAL(data)      AA_AEC_BASE(1, data)
#define AA_AEC_ERROR(data)         AA_AEC_BASE(2, data)
#define AA_AEC_WARNING(data)       AA_AEC_BASE(3, data)
#ifdef DEBUG
	#define AA_AEC_INFO(data)          AA_AEC_BASE(4, data)
	#define AA_AEC_DEBUG(data)         AA_AEC_BASE(5, data)
	#define AA_AEC_VERBOSE(data)       AA_AEC_BASE(6, data)
	#define AA_AEC_TODO(data)          AA_AEC_BASE(4, "TODO : " << data)
#else
	#define AA_AEC_INFO(data)          do { } while(false)
	#define AA_AEC_DEBUG(data)         do { } while(false)
	#define AA_AEC_VERBOSE(data)       do { } while(false)
	#define AA_AEC_TODO(data)          do { } while(false)
#endif

#define AA_AEC_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			APPL_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)

#endif
