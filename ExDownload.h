#ifndef __EXDOWNLOAD_H__
#define __EXDOWNLOAD_H__

#include "ExImage.h"
#include "ExWindow.h"
#include "ExTextBox.h"
#include "ExRectangle.h"
#include "ExBox.h"

#include "ExInput.h"
#include "ExScreen.h"

#include <shellapi.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")


using namespace std;

namespace ExDownload
{
	void Terminate(ExControl* ptControl);
	void ShowHide();
	bool isOpen();
	void Download(wstring szURL, BYTE bExecute);
	DWORD WINAPI DownloadThread(void* Params);
	class Callbacks : public IBindStatusCallback
	{
	public: 
		Callbacks() {} 
		~Callbacks() {} 
		  HWND hwnd;

		STDMETHOD(OnProgress)( ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR wszStatusText); 
		STDMETHOD(OnStartBinding)( DWORD dwReserved, IBinding __RPC_FAR *pib) { return E_NOTIMPL; }
		STDMETHOD(GetPriority)(LONG __RPC_FAR *pnPriority) { return E_NOTIMPL; } 
		STDMETHOD(OnLowResource)( DWORD reserved) { return E_NOTIMPL; } 
		STDMETHOD(OnStopBinding)( HRESULT hresult,  LPCWSTR szError) { return E_NOTIMPL; } 
		STDMETHOD(GetBindInfo)( DWORD __RPC_FAR *grfBINDF, BINDINFO __RPC_FAR *pbindinfo) { return E_NOTIMPL; } 
		STDMETHOD(OnDataAvailable)( DWORD grfBSCF, DWORD dwSize, FORMATETC __RPC_FAR *pformatetc,STGMEDIUM __RPC_FAR *pstgmed) { return E_NOTIMPL; } 
		STDMETHOD(OnObjectAvailable)( REFIID riid, IUnknown __RPC_FAR *punk) { return E_NOTIMPL; } 
		STDMETHOD_(ULONG,AddRef)() { return 0; } 
		STDMETHOD_(ULONG,Release)() { return 0; } 
		STDMETHOD(QueryInterface)(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) { return E_NOTIMPL; } 
	}; 
}


#endif