//
// Created by Admin on 5/2/2026.
//
#include <jni.h>
#include <string>
#include <algorithm>
#include <climits>
#include <android/log.h>

#define LOG_TAG "JNI_DEMO"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_MainActivity_helloFromJNI(
        JNIEnv* env, jobject) {
    LOGI("Appel de helloFromJNI depuis le natif");
    return env->NewStringUTF("Hello from C++ via JNI !");
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jnidemo_MainActivity_factorial(
        JNIEnv* env, jobject, jint n) {
    if (n < 0) { LOGE("Erreur : n negatif"); return -1; }
    long long fact = 1;
    for (int i = 1; i <= n; i++) {
        fact *= i;
        if (fact > INT_MAX) { LOGE("Overflow pour n=%d", n); return -2; }
    }
    LOGI("Factoriel de %d = %lld", n, fact);
    return static_cast<jint>(fact);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_MainActivity_reverseString(
        JNIEnv* env, jobject, jstring javaString) {
    if (javaString == nullptr)
        return env->NewStringUTF("Erreur : chaine nulle");
    const char* chars = env->GetStringUTFChars(javaString, nullptr);
    if (chars == nullptr) return env->NewStringUTF("Erreur JNI");
    std::string s(chars);
    env->ReleaseStringUTFChars(javaString, chars);
    std::reverse(s.begin(), s.end());
    LOGI("String inversee = %s", s.c_str());
    return env->NewStringUTF(s.c_str());
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jnidemo_MainActivity_sumArray(
        JNIEnv* env, jobject, jintArray array) {
    if (array == nullptr) { LOGE("Tableau nul"); return -1; }
    jsize len = env->GetArrayLength(array);
    jint* elements = env->GetIntArrayElements(array, nullptr);
    if (elements == nullptr) return -2;
    long long sum = 0;
    for (jsize i = 0; i < len; i++) sum += elements[i];
    env->ReleaseIntArrayElements(array, elements, 0);
    if (sum > INT_MAX) { LOGE("Overflow somme"); return -3; }
    LOGI("Somme du tableau = %lld", sum);
    return static_cast<jint>(sum);
}