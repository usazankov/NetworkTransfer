
#include "stdafx.h"


#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#endif

#include <stdio.h>

#include <io.h>
#include <errno.h>

#include <wchar.h>
#include "NetworkTransfer.h"
#include <string.h>
#include <locale.h>

//#include <gdiplus.h>

#define TIME_LEN 34
#define BASE_ERRNO     7                                             


//ULONG_PTR m_gdiplusToken;
//using namespace Gdiplus;


static wchar_t *g_PropNames[] = { L"" };
static wchar_t *g_PropNamesRu[] = { L"" };

static wchar_t *g_MethodNamesRu[] = {
	L"Версия",
	L"СтартоватьСервер",
	L"ОстановитьСервер",
	L"ЕстьЛиДанные",
	L"СчитатьДанные",
	L"ОжиданиеВходящегоСоединения"
};

static wchar_t *g_MethodNames[] = {
	L"Version",
	L"StartServer",
	L"StopServer",
	L"IsThereData",
	L"ReadData",
	L"WaitForConnect"
};


static const wchar_t g_kClassNames[] = L"NetworkTransfer";
static IAddInDefBase *pAsyncEvent = NULL;

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);
std::wstring replace(std::wstring text, std::wstring s, std::wstring d);

//---------------------------------------------------------------------------//

const wchar_t* kComponentVersion = L"0.0.0.2";

const wchar_t* kErrMsg_UndErrorToStart = L"Error to start: Undefined Error";
const wchar_t* kErrMsg_UndErrorToStop = L"Error to stop: Undefined Error";
const wchar_t* kErrMsg_ErrorToCallIsThereData = L"Error to call IsThereData: Undefined Error";
const wchar_t* kErrMsg_ErrorToReadData = L"Error to read data: Undefined Error";

//---------------------------------------------------------------------------//
extern "C" Q_DECL_EXPORT long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
	if (!*pInterface)
	{
		*pInterface = new NetworkTransfer;
		return (long)*pInterface;
	}
	return 0;
}
//---------------------------------------------------------------------------//
extern "C" Q_DECL_EXPORT long DestroyObject(IComponentBase** pIntf)
{
	if (!*pIntf)
		return -1;

	delete *pIntf;
	*pIntf = 0;
	return 0;
}
//---------------------------------------------------------------------------//
extern "C" Q_DECL_EXPORT const WCHAR_T* GetClassNames()
{
	static WCHAR_T* names = 0;
	if (!names)
		::convToShortWchar(&names, g_kClassNames);
	return names;
}

//---------------------------------------------------------------------------//
#ifndef __linux__
VOID CALLBACK MyTimerProc(
	HWND hwnd, // handle of window for timer messages
	UINT uMsg, // WM_TIMER message
	UINT idEvent, // timer identifier
	DWORD dwTime // current system time
);
#else
static void MyTimerProc(int sig);
#endif //__linux__

// NetworkTransfer
//---------------------------------------------------------------------------//
NetworkTransfer::NetworkTransfer()
{
	m_iMemory = 0;
	m_iConnect = 0;
	server = new Server(7635);
	
}
//---------------------------------------------------------------------------//
NetworkTransfer::~NetworkTransfer()
{
	delete server;
}
//---------------------------------------------------------------------------//
bool NetworkTransfer::Init(void* pConnection)
{
	m_iConnect = (IAddInDefBase*)pConnection;
	return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long NetworkTransfer::GetInfo()
{
	// Component should put supported component technology version 
	// This component supports 2.0 version
	return 2000;
}
//---------------------------------------------------------------------------//
void NetworkTransfer::Done()
{
}
/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase
//---------------------------------------------------------------------------//
bool NetworkTransfer::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{
	wchar_t *wsExtension = L"AddInNativeExtension";
	int iActualSize = ::wcslen(wsExtension) + 1;
	WCHAR_T* dest = 0;

	if (m_iMemory)
	{
		if (m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(wsExtensionName, wsExtension, iActualSize);
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------//
long NetworkTransfer::GetNProps()
{
	// You may delete next lines and add your own implementation code here
	return ePropLast;
}
//---------------------------------------------------------------------------//
long NetworkTransfer::FindProp(const WCHAR_T* wsPropName)
{
	long plPropNum = -1;
	wchar_t* propName = 0;

	::convFromShortWchar(&propName, wsPropName);
	plPropNum = findName(g_PropNames, propName, ePropLast);

	if (plPropNum == -1)
		plPropNum = findName(g_PropNamesRu, propName, ePropLast);

	delete[] propName;

	return plPropNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* NetworkTransfer::GetPropName(long lPropNum, long lPropAlias)
{
	if (lPropNum >= ePropLast)
		return NULL;

	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsPropName = NULL;
	int iActualSize = 0;

	switch (lPropAlias)
	{
	case 0: // First language
		wsCurrentName = g_PropNames[lPropNum];
		break;
	case 1: // Second language
		wsCurrentName = g_PropNamesRu[lPropNum];
		break;
	default:
		return 0;
	}

	iActualSize = wcslen(wsCurrentName) + 1;

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
	}

	return wsPropName;
}
//---------------------------------------------------------------------------//
bool NetworkTransfer::GetPropVal(const long /*lPropNum*/, tVariant* /*pvarPropVal*/)
{
	return false;
}
//---------------------------------------------------------------------------//
bool NetworkTransfer::SetPropVal(const long /*lPropNum*/, tVariant* /*varPropVal*/)
{
	return false;
}
//---------------------------------------------------------------------------//
bool NetworkTransfer::IsPropReadable(const long /*lPropNum*/)
{
	return false;
}
//---------------------------------------------------------------------------//
bool NetworkTransfer::IsPropWritable(const long /*lPropNum*/)
{
	return false;
}
//---------------------------------------------------------------------------//
long NetworkTransfer::GetNMethods()
{
	return eMethLast;
}
//---------------------------------------------------------------------------//
long NetworkTransfer::FindMethod(const WCHAR_T* wsMethodName)
{
	long plMethodNum = -1;
	wchar_t* name = 0;

	::convFromShortWchar(&name, wsMethodName);

	plMethodNum = findName(g_MethodNames, name, eMethLast);

	if (plMethodNum == -1)
		plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

	return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* NetworkTransfer::GetMethodName(const long lMethodNum, const long lMethodAlias)
{
	if (lMethodNum >= eMethLast)
		return NULL;

	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsMethodName = NULL;
	int iActualSize = 0;

	switch (lMethodAlias)
	{
	case 0: // First language
		wsCurrentName = g_MethodNames[lMethodNum];
		break;
	case 1: // Second language
		wsCurrentName = g_MethodNamesRu[lMethodNum];
		break;
	default:
		return 0;
	}

	iActualSize = wcslen(wsCurrentName) + 1;

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
	}

	return wsMethodName;
}
//---------------------------------------------------------------------------//
long NetworkTransfer::GetNParams(const long lMethodNum)
{
	switch (lMethodNum)
	{
	case eVersion:
		return 0;
	case eReadData:
		return 0;
	case eStartServer:
		return 0;
	case eStopServer:
		return 0;
	case eIsThereData:
		return 0;
	case eWaitForConnect:
		return 1;
	default:
		return 0;
	}

	return 0;
}
//---------------------------------------------------------------------------//
bool NetworkTransfer::GetParamDefValue(const long lMethodNum, const long lParamNum,
	tVariant *pvarParamDefValue)
{
	TV_VT(pvarParamDefValue) = VTYPE_EMPTY;

	switch (lMethodNum)
	{
	case eVersion:
	case eReadData:
	case eStartServer:
	case eStopServer:
	case eIsThereData:
	case eWaitForConnect:
		// There are no parameter values by default 
		break;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool NetworkTransfer::HasRetVal(const long lMethodNum)
{
	switch (lMethodNum)
	{
	case eVersion:
		return true;
	case eReadData:
		return true;
	case eIsThereData:
		return true;
	case eStartServer:
		return true;
	case eStopServer:
		return true;
	case eWaitForConnect:
		return true;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool NetworkTransfer::CallAsProc(const long lMethodNum,
	tVariant* paParams, const long lSizeArray)
{
	switch (lMethodNum)
	{
	case eVersion:
	case eReadData:
		break;

	default:
		return false;
	}

	return false;   // as func
}


//---------------------------------------------------------------------------//
bool NetworkTransfer::CallAsFunc(const long lMethodNum,
	tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
	switch (lMethodNum)
	{
	case eVersion:
	{
		if (pvarRetValue)
		{
			size_t strLen = wcslen(kComponentVersion);
			qDebug() << "strLen=" << strLen;
			if (m_iMemory->AllocMemory((void**)&pvarRetValue->pwstrVal, (strLen + 1) * sizeof(kComponentVersion[0])))
			{
				wcscpy_s(pvarRetValue->pwstrVal, strLen + 1, kComponentVersion);
				//memcpy(pvarRetValue->pwstrVal, kComponentVersion, strLen + 1);
				pvarRetValue->wstrLen = strLen;
				TV_VT(pvarRetValue) = VTYPE_PWSTR;
			}
		}

		break;
	}
	case eStartServer:
		if (pvarRetValue == NULL)
		{
			break;
		}
		pvarRetValue->vt = VTYPE_BOOL;
		pvarRetValue->bVal = 0;

		if (server) 
		{
			if (server->start()) 
			{
				pvarRetValue->bVal = 1;
			}
			else
			{
				if (server->isError())
				{
					wchar_t* str_t = NULL;
					server->errorMessage().toWCharArray(str_t);
					addError(str_t);
				}
				else
				{
					addError(kErrMsg_UndErrorToStart);
				}
			}
		}
		else 
		{
			addError(kErrMsg_UndErrorToStart);
		}
		break;
	case eStopServer:
		if (pvarRetValue == NULL)
		{
			break;
		}
		pvarRetValue->vt = VTYPE_BOOL;
		pvarRetValue->bVal = 0;
		if (server)
		{
			if (server->stop())
			{
				pvarRetValue->bVal = 1;
			}
			else 
			{
				if (server->isError())
				{
					wchar_t* str_t = NULL;
					server->errorMessage().toWCharArray(str_t);
					addError(str_t);
				}
				else
				{
					addError(kErrMsg_UndErrorToStop);
				}
			}
		}
		else
		{
			addError(kErrMsg_UndErrorToStop);
		}
		break;
	case eIsThereData:
		if (pvarRetValue == NULL)
		{
			break;
		}
		pvarRetValue->vt = VTYPE_BOOL;
		pvarRetValue->bVal = 0;
		if (server)
		{
			if (server->dataIsNotNull()) 
			{
				pvarRetValue->bVal = 1;
			}
		}
		else 
		{
			addError(kErrMsg_ErrorToCallIsThereData);
		}
		break;
	case eReadData:
	{
		if (pvarRetValue == NULL)
		{
			break;
		}
		TV_VT(pvarRetValue) = VTYPE_PWSTR;
		pvarRetValue->wstrLen = 0;
		if (server)
		{
			
			if(server->dataIsNotNull()){
				qDebug() << "if(server->dataIsNotNull()){";
				QString d = server->readData();
				
				wchar_t* str_t = (wchar_t*)malloc((d.size() + 1) * sizeof(wchar_t));
				d.toWCharArray(str_t);
				str_t[d.size() * sizeof(wchar_t)] = L'\0';
				qDebug() << "wcslen" << wcslen(str_t);
				size_t strSize = wcslen(str_t);
				if(d.size() != 0)
				{
					if (m_iMemory->AllocMemory((void**)&pvarRetValue->pwstrVal, (strSize + 1) * sizeof(str_t[0])))
					{
						qDebug() << "if (m_iMemory->AllocMemory((void**)&pvarRetValue->pwstrVal, (strLen + 1) * sizeof(str_t[0])))";
						wcscpy_s(pvarRetValue->pwstrVal, strSize + 1, str_t);
						qDebug() << "wcscpy_s(pvarRetValue->pwstrVal, strLen + 1, str_t);";
						//memcpy(pvarRetValue->pwstrVal, str_t, strLen + 1);
						pvarRetValue->wstrLen = d.size();
					}
				}
				free(str_t);
			}
		}
		else 
		{
			addError(kErrMsg_ErrorToReadData);
		}
		break;
	}
	case eWaitForConnect:
	{
		if (pvarRetValue == NULL)
		{
			break;
		}
		tVariant& pParam0 = paParams[0];
		pvarRetValue->vt = VTYPE_BOOL;
		pvarRetValue->bVal = 0;
		break;
	}
	default:
		return false;
	}

	return true;
}


//---------------------------------------------------------------------------//
// This code will work only on the client!
#ifndef __linux__
VOID CALLBACK MyTimerProc(
	HWND hwnd,    // handle of window for timer messages
	UINT uMsg,    // WM_TIMER message
	UINT idEvent, // timer identifier
	DWORD dwTime  // current system time
)
{
	if (!pAsyncEvent)
		return;

	wchar_t *who = L"ComponentNative", *what = L"Timer";

	wchar_t *wstime = new wchar_t[TIME_LEN];
	if (wstime)
	{
		wmemset(wstime, 0, TIME_LEN);
		::_ultow(dwTime, wstime, 10);
		pAsyncEvent->ExternalEvent(who, what, wstime);
		delete[] wstime;
	}
}
#else
void MyTimerProc(int sig)
{
	if (pAsyncEvent)
		return;

	WCHAR_T *who = 0, *what = 0, *wdata = 0;
	wchar_t *data = 0;
	time_t dwTime = time(NULL);

	data = new wchar_t[TIME_LEN];

	if (data)
	{
		wmemset(data, 0, TIME_LEN);
		swprintf(data, TIME_LEN, L"%ul", dwTime);
		::convToShortWchar(&who, L"ComponentNative");
		::convToShortWchar(&what, L"Timer");
		::convToShortWchar(&wdata, data);

		pAsyncEvent->ExternalEvent(who, what, wdata);

		delete[] who;
		delete[] what;
		delete[] wdata;
		delete[] data;
	}
}
#endif
//---------------------------------------------------------------------------//
void NetworkTransfer::SetLocale(const WCHAR_T* loc)
{
#ifndef __linux__
	_wsetlocale(LC_ALL, loc);
#else
	//We convert in char* char_locale
	//also we establish locale
	//setlocale(LC_ALL, char_locale);
#endif
}
/////////////////////////////////////////////////////////////////////////////
// LocaleBase
//---------------------------------------------------------------------------//
bool NetworkTransfer::setMemManager(void* mem)
{
	m_iMemory = (IMemoryManager*)mem;
	return m_iMemory != 0;
}
//---------------------------------------------------------------------------//
void NetworkTransfer::addError(uint32_t wcode, const wchar_t* source,
	const wchar_t* descriptor, long code)
{
	if (m_iConnect)
	{
		WCHAR_T *err = 0;
		WCHAR_T *descr = 0;

		::convToShortWchar(&err, source);
		::convToShortWchar(&descr, descriptor);

		m_iConnect->AddError(wcode, err, descr, code);
		delete[] err;
		delete[] descr;
	}
}


//---------------------------------------------------------------------------//
void NetworkTransfer::addError(const wchar_t* errorText)
{
	if (m_iConnect)
		m_iConnect->AddError(ADDIN_E_NONE, L"NetworkTransfer", errorText, 0);
}


//---------------------------------------------------------------------------//
long NetworkTransfer::findName(wchar_t* names[], const wchar_t* name,
	const uint32_t size) const
{
	long ret = -1;
	for (uint32_t i = 0; i < size; i++)
	{
		if (!wcscmp(names[i], name))
		{
			ret = i;
			break;
		}
	}
	return ret;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
	if (!len)
		len = ::wcslen(Source) + 1;

	if (!*Dest)
		*Dest = new WCHAR_T[len];

	WCHAR_T* tmpShort = *Dest;
	wchar_t* tmpWChar = (wchar_t*)Source;
	uint32_t res = 0;

	::memset(*Dest, 0, len * sizeof(WCHAR_T));
	do
	{
		*tmpShort++ = (WCHAR_T)*tmpWChar++;
		++res;
	} while (len-- && *tmpWChar);

	return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
	if (!len)
		len = getLenShortWcharStr(Source) + 1;

	if (!*Dest)
		*Dest = new wchar_t[len];

	wchar_t* tmpWChar = *Dest;
	WCHAR_T* tmpShort = (WCHAR_T*)Source;
	uint32_t res = 0;

	::memset(*Dest, 0, len * sizeof(wchar_t));
	do
	{
		*tmpWChar++ = (wchar_t)*tmpShort++;
		++res;
	} while (len-- && *tmpShort);

	return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
	uint32_t res = 0;
	WCHAR_T *tmpShort = (WCHAR_T*)Source;

	while (*tmpShort++)
		++res;

	return res;
}

std::wstring replace(std::wstring text, std::wstring s, std::wstring d)
{
	for (unsigned index = 0; index = text.find(s, index), index != std::wstring::npos;)
	{
		text.replace(index, s.length(), d);
		index += d.length();
	}
	return text;
}
