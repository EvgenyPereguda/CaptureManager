#pragma once

#ifndef __FUNCTION__
#define __FUNCTION__ "Empty"
#endif

#ifdef _DEBUG
#define _DEBUG_CAPTUREMANAGER
#endif

#define ENABLE

#define RGB2BGR(a_ulColor) (a_ulColor & 0xFF000000) | ((a_ulColor & 0xFF0000) >> 16) | (a_ulColor & 0x00FF00) | ((a_ulColor & 0x0000FF) << 16)

#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_FUNCTION(Function, ...) lresult = Function(__VA_ARGS__);\
	if (FAILED(lresult))\
					{\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
				}\

#else
#define LOG_INVOKE_FUNCTION(Function, ...) lresult = Function(__VA_ARGS__);\
	if (FAILED(lresult))break;\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_POINTER_METHOD(Pointer, Method, ...) lresult = Pointer->Method(__VA_ARGS__);\
	if (FAILED(lresult))\
						{\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
					}\

#else
#define LOG_INVOKE_POINTER_METHOD(Pointer, Method, ...) lresult = Pointer->Method(__VA_ARGS__);\
	if (FAILED(lresult))break;\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_OBJECT_METHOD(Object, Method, ...) lresult = Object.Method(__VA_ARGS__);\
	if (FAILED(lresult))\
							{\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
						}\

#else
#define LOG_INVOKE_OBJECT_METHOD(Object, Method, ...) lresult = Object.Method(__VA_ARGS__);\
	if (FAILED(lresult))break;\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_CHECK_COM_PTR_OBJECT_MEMORY(Object) if (Object == nullptr)\
								{\
								lresult = E_OUTOFMEMORY;\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
							}\

#else
#define LOG_CHECK_COM_PTR_OBJECT_MEMORY(Object) if (Object == nullptr)\
																{\
		lresult = E_OUTOFMEMORY;\
		break;\
		}\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_CHECK_PTR_MEMORY(Object) if (Object == nullptr)\
																{\
								lresult = E_POINTER;\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
														}\

#else
#define LOG_CHECK_PTR_MEMORY(Object) if (Object == nullptr)\
																																{\
		lresult = E_POINTER;\
		break;\
				}\

#endif



#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_CHECK_STATE(Object) if (Object)\
																																{\
								lresult = E_INVALIDARG;\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
																												}\

#else
#define LOG_CHECK_STATE(Object) if (Object)\
									{\
										lresult = E_INVALIDARG;\
										break;\
									}\

#endif



#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_CHECK_STATE_DESCR(Object, Descr) if (Object)\
																																																																{\
								lresult = Descr;\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
																																																								}\

#else
#define LOG_CHECK_STATE_DESCR(Object, Descr) if (Object)\
								{\
								lresult = Descr;\
										break;\
																		}\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_QUERY_INTERFACE_METHOD(Object, PtrPtrPointer) lresult = Object->QueryInterface(IID_PPV_ARGS(PtrPtrPointer));\
	if (FAILED(lresult))\
						{\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Object: ",\
		#Object,\
		L" Error code: ",\
		lresult);\
		break;\
					}\

#else
#define LOG_INVOKE_QUERY_INTERFACE_METHOD(Object, PtrPointer) lresult = Object->QueryInterface(IID_PPV_ARGS(PtrPointer));\
	if (FAILED(lresult))break;\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(Object, aRefIID, PtrPtrPointer) lresult = Object->QueryInterface(aRefIID, PtrPtrPointer);\
	if (FAILED(lresult))\
							{\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Object: ",\
		#Object,\
		L" Error code: ",\
		lresult);\
		break;\
						}\

#else
#define LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(Object, aRefIID, PtrPtrPointer) lresult = Object->QueryInterface(aRefIID, PtrPtrPointer);\
	if (FAILED(lresult))break;\

#endif
