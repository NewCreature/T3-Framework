#include "t3f.h"

/* The code below is from a supposedly working implementation. We should derive
   our implementation from it and see if we can actually find our
   ExampleActivity class. */

#ifdef T3F_ANDROID

	#include <jni.h>
	
	JNIEnv *_al_android_get_jnienv();
	void __jni_checkException(JNIEnv *env, const char *file, const char *fname, int line);
	jobject _al_android_activity_object();

	#define _jni_checkException(env) __jni_checkException(env, __FILE__, __FUNCTION__, __LINE__)

	#define _jni_call(env, rett, method, args...) ({ \
	   rett ret = (*env)->method(env, args); \
	   _jni_checkException(env); \
	   ret; \
	})

	#define _jni_callv(env, method, args...) ({ \
	   (*env)->method(env, args); \
	   _jni_checkException(env); \
	})

	#define _jni_callVoidMethodV(env, obj, name, sig, args...) ({ \
	   jclass class_id = _jni_call(env, jclass, GetObjectClass, obj); \
	   \
	   jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, name, sig); \
	   if(method_id == NULL) { \
	   } else { \
		  _jni_callv(env, CallVoidMethod, obj, method_id, ##args); \
	   } \
	   \
	   _jni_callv(env, DeleteLocalRef, class_id); \
	})

	#define _jni_callBooleanMethodV(env, obj, name, sig, args...) ({ \
	   jclass class_id = _jni_call(env, jclass, GetObjectClass, obj); \
	   \
	   jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, name, sig); \
	   \
	   bool ret = false; \
	   if(method_id == NULL) { \
	   } \
	   else { \
		  ret = _jni_call(env, bool, CallBooleanMethod, obj, method_id, ##args); \
	   } \
	   \
	   _jni_callv(env, DeleteLocalRef, class_id); \
	   \
	   ret; \
	})

	#define _jni_callIntMethodV(env, obj, name, sig, args...) ({ \
	   jclass class_id = _jni_call(env, jclass, GetObjectClass, obj); \
	   \
	   jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, name, sig); \
	   \
	   int ret = -1; \
	   if(method_id == NULL) { \
		  /*ALLEGRO_DEBUG("couldn't find method :(");*/ \
	   } \
	   else { \
		  ret = _jni_call(env, int, CallIntMethod, obj, method_id, ##args); \
	   } \
	   \
	   _jni_callv(env, DeleteLocalRef, class_id); \
	   \
	   ret; \
	})

	#define _jni_callIntMethod(env, obj, name) _jni_callIntMethodV(env, obj, name, "()I");

	static jobject _jni_callObjectMethod(JNIEnv *env, jobject object, const char *name, const char *sig)
	{
	   jclass class_id = _jni_call(env, jclass, GetObjectClass, object);
	   jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, name, sig);
	   jobject ret = _jni_call(env, jobject, CallObjectMethod, object, method_id);
	   _jni_callv(env, DeleteLocalRef, class_id);
	   
	   return ret;
	}
	
	void t3f_show_soft_keyboard(bool toggle)
	{
        if(toggle)
        {
			_jni_callVoidMethodV(_al_android_get_jnienv(), _al_android_activity_object(), "OpenKeyBoard", "()V");
		}
		else
		{
			_jni_callVoidMethodV(_al_android_get_jnienv(), _al_android_activity_object(), "CloseKeyBoard", "()V");
		}
	}

	void t3f_open_url(const char *url)
	{
		JNIEnv * env = _al_android_get_jnienv();
		jstring urlS = (*env)->NewStringUTF(env, url);

		_jni_callVoidMethodV(
			_al_android_get_jnienv(),
			_al_android_activity_object(),
			"openURL",
			"(Ljava/lang/String;)V",
			urlS
		);
		(*env)->DeleteLocalRef(env, urlS);
	}

#else
	void t3f_show_soft_keyboard(bool toggle)
	{
	}

	void t3f_open_url(const char *url)
	{
		char command[256] = {0};

		#ifdef ALLEGRO_WINDOWS
			snprintf(command, 256, "start %s", url);
		#else
			#ifdef ALLEGRO_MACOSX
				snprintf(command, 256, "open %s", url);
			#else
				snprintf(command, 256, "xdg-open %s", url);
			#endif
		#endif
		al_stop_timer(t3f_timer);
		(void) system(command);
		al_start_timer(t3f_timer);
	}
#endif
