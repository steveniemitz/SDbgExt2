#pragma once
#include <functional>

//std::for_each

#define BEGIN_DEFINE_ENUM_ADAPTOR(ClassName, EnumInterface) \
template<typename _State> \
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
	
#define DEFINE_ENUM_ADAPTOR_CALLBACK typedef BOOL (CALLBACK *CallbackFunc)

#define BEGIN_DEFINE_ENUM_ADAPTOR_BODY() \
	HRESULT Init(CallbackFunc cb, _State *state) \
	{ m_cb = cb; \
		m_state = state; \
		return S_OK; \
	} \
	ULONG InternalAddRef() { return 1; } \
	ULONG InternalRelease() { return 1; } \
private: \
	void* operator new(size_t s); \
	CallbackFunc m_cb; \
	_State *m_state; \
public:

#define END_DEFINE_ENUM_ADAPTOR };

#define BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(ClassName, EnumInterface, FuncType) \
class ClassName : \
	public CComObjectRoot, \
	public EnumInterface \
{ \
	BEGIN_COM_MAP(ClassName) \
		COM_INTERFACE_ENTRY(EnumInterface) \
	END_COM_MAP() \
public: \
	HRESULT Init(std::function<FuncType> cb) \
	{ \
		m_cb = cb; \
		return S_OK; \
	} \
	ULONG InternalAddRef() { return 1; } \
	ULONG InternalRelease() { return 1; } \
private: \
	void* operator new(size_t s); \
	std::function<FuncType> m_cb; \
public:

#define END_DEFINE_ENUM_ADAPTOR_FUNCTOR };