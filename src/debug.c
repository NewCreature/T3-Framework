#include "t3f.h"

void t3f_debug_log(const char * format, ...)
{
	#ifdef T3F_DEBUG
		char buf[1024] = {0};
		va_list vap;

		va_start(vap, format);
		vsnprintf(buf, 1024, format, vap);
		va_end(vap);
		
		#ifdef T3F_ANDROID
			ALLEGRO_DEBUG_CHANNEL("main");
			ALLEGRO_DEBUG(buf);
		#else
			printf("%s", buf);
		#endif
	#else
		(void)format;
	#endif
}
