#pragma once

#define BEGIN_DEFINE_ENUM_ADAPTOR(ClassName, EnumInterface, CallbackType) \
class ClassName : \
	public CComObjectRoot, \
	public EnumInterface \
{ \
	BEGIN_COM_MAP(ClassName) \
		COM_INTERFACE_ENTRY(EnumInterface) \
	END_COM_MAP() \
public: \
	ClassName() \
		: m_cb(NULL), m_state(NULL) \
	{ } \
	HRESULT Init(CallbackType cb, PVOID state) \
	{ m_cb = cb; \
		m_state = state; \
		return S_OK; \
	} \
private: \
	CallbackType m_cb; \
	PVOID m_state; \
public:

#define DEFINE_ENUM_CALLBACK(CallbackName,...) \
	HRESULT CallbackName(__VA_ARGS__) { return S_OK; }

#define END_DEFINE_ENUM_ADAPTOR };