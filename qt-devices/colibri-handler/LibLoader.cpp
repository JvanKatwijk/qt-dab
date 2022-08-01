#include "LibLoader.h"

#ifndef __linux__
bool LibLoader::load(const wstring &file)
#else
bool LibLoader::load(const char *file)
#endif
{
#ifndef __linux__
	hDLL = LoadLibrary(file.data());
#else
#  define GetProcAddress dlsym
	hDLL = dlopen(file, RTLD_LAZY);
#endif
	if (hDLL == nullptr)
	   return false;

	m_initialize =
	     reinterpret_cast<pFunc1> (GetProcAddress (hDLL, "initialize"));
	if (m_initialize == nullptr)
	   return false;

	m_finalize =
	     reinterpret_cast<pFunc1> (GetProcAddress (hDLL, "finalize"));
	if (m_finalize == nullptr)
	   return false;

	m_version =
	     reinterpret_cast<pVersion> (GetProcAddress (hDLL, "version"));
	if (m_version == nullptr)
	   return false;

	m_information =
	    reinterpret_cast<pInformation>(GetProcAddress (hDLL, "information"));
	if (m_information == nullptr)
	   return false;

	m_devices =
	    reinterpret_cast<pDevices>(GetProcAddress (hDLL, "devices"));
	if (m_devices == nullptr)
	   return false;

	m_open = reinterpret_cast<pOpen>(GetProcAddress (hDLL, "open"));
	if (m_open == nullptr)
	   return false;

	m_close =
	       reinterpret_cast<pClose>(GetProcAddress (hDLL, "close"));
	if (m_close == nullptr)
	   return false;

	m_start =
	       reinterpret_cast<pStart>(GetProcAddress (hDLL, "start"));
	if (m_start == nullptr)
	   return false;

	m_stop = reinterpret_cast<pStop>(GetProcAddress(hDLL, "stop"));
	if (m_stop == nullptr)
	   return false;

	m_setPreamp =
	      reinterpret_cast<pSetPreamp>(GetProcAddress(hDLL, "setPream"));
	if (m_setPreamp == nullptr)
	   return false;

	m_setFrequency =
	      reinterpret_cast<pSetFrequency>(GetProcAddress(hDLL, "setFrequency"));
	if (m_setFrequency == nullptr)
	   return false;

	return true;
}

void LibLoader::initialize () {
    if (m_initialize)
        m_initialize();
}

void LibLoader::finalize() {
    if (m_finalize)
        m_finalize();
}

void LibLoader::version(uint32_t &major, uint32_t &minor, uint32_t &patch) {
    if (m_version) {
        m_version (major, minor, patch);
    }
    else {
        major = 0;
        minor = 0;
        patch = 0;
    }
}

string LibLoader::information () {
string t_str;

    if (m_information) {
        char *pStr = nullptr;
        m_information (&pStr);
        if (pStr)
            t_str = string (pStr);
    }

    return t_str;
}

uint32_t LibLoader::devices () {
uint32_t t_count = 0;
    if (m_devices)
        m_devices(t_count);
    return t_count;
}

bool LibLoader::open(Descriptor *pDev, const uint32_t devIndex) {
	if (m_open)
        return m_open(pDev, devIndex);
    return false;
}

void LibLoader::close (Descriptor dev) {
	if (m_open)
	   m_close(dev);
}

bool LibLoader::start (Descriptor dev,
	               SampleRateIndex sr, pCallbackRx p, void *pUserData) {
	if (m_start)
	   return m_start(dev, sr, p, pUserData);
	return false;
}

bool LibLoader::stop(Descriptor dev) {
	if (m_stop)
	   return m_stop(dev);
	return false;
}

bool LibLoader::setPream (Descriptor dev, float value) {
	if (m_setPreamp)
	   return m_setPreamp (dev, value);
	return false;
}

bool LibLoader::setFrequency(Descriptor dev, uint32_t value) {
	if (m_setFrequency)
	   return m_setFrequency(dev, value);
	return false;
}


