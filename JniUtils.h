#ifndef LIBORWELLANDROID_JNI_UTILS_H
#define LIBORWELLANDROID_JNI_UTILS_H
#include <jni.h>
#include <string>
#include <vector>
#include <assert.h>
#include <stdexcept>
#include <android/log.h>
#define LOG_TAG "OrwellJNI"
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
namespace liborwellandroid
{
    class JniUtils
    {
    public:
        class SmartAttachCurrentThread
        {
        public:
            SmartAttachCurrentThread(JavaVM *jvm, JNIEnv *&env) : jvm(jvm)
            {
                if (!jvm) {
                    ALOGV("no jvm!");
                    //throw...; // whatever you want
                }

                if (jvm->AttachCurrentThread(&env, NULL) != JNI_OK){
                    ALOGV("attachCurrentThread problem!");
                    //throw...; // whatever you want
                }
            }

            ~SmartAttachCurrentThread()
            {
                jvm->DetachCurrentThread();
            }

        private:
            JavaVM *jvm;
        };

        static std::vector<uint8_t> vectorFromByteArray(JNIEnv *env, jbyteArray array)
        {
            /*
            jbyte *arrayjBytes = (*env)->GetByteArrayElements(env, array, NULL);
            jsize lengthOfArray = (*env)->GetArrayLength(env, arrayjBytes);
            std::vector<uint8_t> messageVector;
            messageVector.resize(lengthOfArray);
            std::copy(messageVector.data(), messageVector.data() + lengthOfArray, arrayjBytes);
            (*env)->messageBytes(env, array, arrayjBytes, 0);
            return messageVector;
            */
            return std::vector<uint8_t>{};
        }

        //static void InitiateSimpleClasses(JNIEnv *env, )

        static void getClassAndCache(JNIEnv *env, jclass *cacheHere, std::string className)
        {
            jclass jc = env->FindClass(className.c_str());
            *cacheHere = reinterpret_cast<jclass>(env->NewGlobalRef(jc));
            assert(*cacheHere != NULL);
            //env->DeleteLocalRef(temporaryReference);
        }

        //Creates an array of int and put into javaInstance on fied field
        static void createFillAndSetIntArrayField(JNIEnv *env, jobject *javaInstance, jfieldID *field, int *array, int arrayLength)
        {
            jintArray intJavaArray = env->NewIntArray(arrayLength);
            jint fill[arrayLength];
            for (int i = 0; i < arrayLength; i++)
            {
                fill[i] = array[i];
            }
            env->SetIntArrayRegion(intJavaArray, 0, arrayLength, fill);
            env->SetObjectField(*javaInstance, *field, intJavaArray);
        }

        static void getFieldIdAndCache(JNIEnv *env, jfieldID *cacheHere, jclass *javaClass, std::string fieldName, std::string fieldSignature)
        {
            *cacheHere = env->GetFieldID(*javaClass, fieldName.c_str(), fieldSignature.c_str());
            //if (*cacheHere != NULL) {
            //     __android_log_print(ANDROID_LOG_ERROR, "JniUtils", "%s", fieldName.c_str());
            //}
            assert(*cacheHere != NULL);
            /*
        jfieldID temporaryReference = env->GetFieldID(*javaClass, fieldName.c_str(), fieldSignature.c_str());
        *cacheHere = (jfieldID) env->NewGlobalRef(temporaryReference);
        assert(*cacheHere != NULL);
        env->DeleteLocalRef(temporaryReference);
        */
        }
        static void getStaticFieldIdAndCache(JNIEnv *env, jfieldID *cacheHere, jclass *javaClass, std::string fieldName, std::string fieldSignature)
        {
            *cacheHere = env->GetStaticFieldID(*javaClass, fieldName.c_str(), fieldSignature.c_str());
            //if (*cacheHere != NULL) {
            //     __android_log_print(ANDROID_LOG_ERROR, "JniUtils", "%s", fieldName.c_str());
            //}
            assert(*cacheHere != NULL);
            /*
        jfieldID temporaryReference = env->GetFieldID(*javaClass, fieldName.c_str(), fieldSignature.c_str());
        *cacheHere = (jfieldID) env->NewGlobalRef(temporaryReference);
        assert(*cacheHere != NULL);
        env->DeleteLocalRef(temporaryReference);
        */
        }

        static void getMethodAndCache(JNIEnv *env, jmethodID *cacheHere, jclass *javaClass, std::string methodName, std::string methodSignature)
        {
            *cacheHere = env->GetMethodID(*javaClass, methodName.c_str(), methodSignature.c_str());
            //if (*cacheHere != NULL) {
            //     __android_log_print(ANDROID_LOG_ERROR, "JniUtils", "%s", fieldName.c_str());
            //}
            assert(*cacheHere != NULL);
        }

        static void getStaticMethodAndCache(JNIEnv *env, jmethodID *cacheHere, jclass *javaClass, std::string methodName, std::string methodSignature)
        {
            *cacheHere = env->GetStaticMethodID(*javaClass, methodName.c_str(), methodSignature.c_str());
            //if (*cacheHere != NULL) {
            //     __android_log_print(ANDROID_LOG_ERROR, "JniUtils", "%s", fieldName.c_str());
            //}
            assert(*cacheHere != NULL);
        }

        //Given an object, gets an object from a field
        static void getObjectFromFieldAndCache(JNIEnv *env, jobject *cacheHere, jobject *object, jfieldID *fieldId)
        {
            *cacheHere = env->GetObjectField(*object, *fieldId);
            assert(*cacheHere != NULL);
        }

        static std::string getString(JNIEnv *env, jstring *javaString)
        {
            const char *nativeString = env->GetStringUTFChars(*javaString, 0);
            std::string r(nativeString);
            env->ReleaseStringUTFChars(*javaString, nativeString);
            return r;
        }

        //https://stackoverflow.com/a/41820336/6655884
        static std::string jstringToString(JNIEnv *env, jstring jStr)
        {
            if (!jStr)
                return "";

            const jclass stringClass = env->GetObjectClass(jStr);
            const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
            const jbyteArray stringJbytes = (jbyteArray)env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

            size_t length = (size_t)env->GetArrayLength(stringJbytes);
            jbyte *pBytes = env->GetByteArrayElements(stringJbytes, NULL);

            std::string ret = std::string((char *)pBytes, length);
            env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

            env->DeleteLocalRef(stringJbytes);
            env->DeleteLocalRef(stringClass);
            return ret;
        }
        /*
    static void inline setIntArrayField(JNIEnv *env, jclass javaClass, jobject javaInstance,
                                        std::string javaField, std::vector<int> arguments)
    {
        jfieldID fieldId = env->GetFieldID(javaClass, javaField.c_str(), "[I");
        assert(fieldId != NULL);
        jintArray newArray = env->NewIntArray(3);
        jint *newArrayElements = env->GetIntArrayElements(newArray, NULL);
        for (int i = 0; i < 3; i++)
        {
            linesizeArrayElements[i] = linesize[i];
        }
        env->SetObjectField(javaInstance, fieldId, newArray);
        env->ReleaseIntArrayElements(newArray, newArrayElements, NULL);
    }
*/
        /*
    static void inline setFractionArrayField(JNIEnv *env, jclass javaClass, jobject javaInstance,
                                             std::string javaField, int numerator, int denominator)
    {
        jclass fractionClass = env->FindClass("com.lucaszanella.orwell_flutter_plugin.PixelFormat.Fraction");
        jobject fractionObject = fractionNumeratorId = env->GetFieldID(javaClass, "numerator", "[I");
        jfieldID fractionDenominatorId = env->GetFieldID(javaClass, "denominator", "[I;");
        jfieldID fieldId = env->GetFieldID(javaClass, javaField.c_str(), "[Lcom/lucaszanella/orwell_flutter_plugin/PixelFormat/Fraction;");
        assert(fieldId != NULL);
        jobjectArray fractionArray = env->NewObjectArray(2, fractionClass, NULL);

        env->SetIntField(fractionArray, fractionNumeratorId, numerator);
        env->SetIntField(fractionArray, fractionDenominatorId, denominator);

        env->SetObjectField(javaInstance, fieldId, fractionArray);
        //env->ReleaseIntArrayElements(fractionArray, newArrayElements, NULL);
    }
*/
    };
} // namespace liborwellandroid
#endif //LIBORWELLANDROID_JNI_UTILS_H
