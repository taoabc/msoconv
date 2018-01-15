#include <string>
#include <vector>
#include <Windows.h>
#include "comptr.h"

const int kErrUnknown = 1;
const int kErrParamError = 2;
const int kErrPowerPoint = 3;
const int kErrPresentations = 4;
const int kErrOpenFile = 5;
const int kErrDestType = 6;
const int kErrSaveAs = 7;

static std::wstring Utf8ToUtf16(const std::string& str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    if (size <= 0) throw std::runtime_error("MultiByteToWideChar failed");

    std::vector<wchar_t> buffer(size);

    size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer.data(), size);
    if (size <= 0) throw std::runtime_error("MultiByteToWideChar failed");

    return std::wstring(buffer.data(), size);
}

static std::string Utf16ToUtf8(const std::wstring& wstr)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (!size) throw std::runtime_error("WideCharToMultiByte failed");

    std::vector<char> buffer(size);

    size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer.data(), size, NULL, NULL);
    if (!size) throw std::runtime_error("WideCharToMultiByte failed");

    return std::string(buffer.data(), size);
}

//
//   FUNCTION: AutoWrap(int, VARIANT*, IDispatch*, LPOLESTR, int,...)
//
//   PURPOSE: Automation helper function. It simplifies most of the low-level 
//      details involved with using IDispatch directly. Feel free to use it 
//      in your own implementations. One caveat is that if you pass multiple 
//      parameters, they need to be passed in reverse-order.
//
//   PARAMETERS:
//      * autoType - Could be one of these values: DISPATCH_PROPERTYGET, 
//      DISPATCH_PROPERTYPUT, DISPATCH_PROPERTYPUTREF, DISPATCH_METHOD.
//      * pvResult - Holds the return value in a VARIANT.
//      * pDisp - The IDispatch interface.
//      * ptName - The property/method name exposed by the interface.
//      * cArgs - The count of the arguments.
//
//   RETURN VALUE: An HRESULT value indicating whether the function succeeds 
//      or not. 
//
//   EXAMPLE: 
//      AutoWrap(DISPATCH_METHOD, NULL, pDisp, L"call", 2, parm[1], parm[0]);
//
HRESULT AutoWrap(int autoType, VARIANT* pvResult, IDispatch* pDisp,
                 LPOLESTR ptName, int cArgs...)
{
    // Begin variable-argument list
    va_list marker;
    va_start(marker, cArgs);
    // Allocate memory for arguments
    VARIANT* pArgs = new VARIANT[cArgs + 1];
    HRESULT hr;
    do {
        if (!pDisp) {
            // NULL IDispatch passed
            hr = E_INVALIDARG;
            break;
        }

        // Variables used
        DISPPARAMS dp = { NULL, NULL, 0, 0 };
        DISPID dispidNamed = DISPID_PROPERTYPUT;
        DISPID dispID;

        // Get DISPID for name passed
        hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
        if (FAILED(hr)) {
            /* wprintf(L"IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx\n",
            ptName, hr);*/
            break;
        }

        // Extract arguments...
        for (int i = 0; i < cArgs; i++) {
            pArgs[i] = va_arg(marker, VARIANT);
        }

        // Build DISPPARAMS
        dp.cArgs = cArgs;
        dp.rgvarg = pArgs;

        // Handle special-case for property-puts
        if (autoType & DISPATCH_PROPERTYPUT) {
            dp.cNamedArgs = 1;
            dp.rgdispidNamedArgs = &dispidNamed;
        }

        // Make the call
        hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT,
                           autoType, &dp, pvResult, NULL, NULL);
        if (FAILED(hr)) {
            /*wprintf(L"IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx\n",
            ptName, dispID, hr);*/
            break;
        }

    } while (false);

    // End variable-argument section
    va_end(marker);

    delete[] pArgs;

    return hr;
}

HRESULT GetPowerPointDispatch(IDispatch** ppApp)
{
    if (!ppApp) {
        return E_INVALIDARG;
    }
    LPCOLESTR progID = L"PowerPoint.Application";
    CLSID clsid;
    HRESULT hr = CLSIDFromProgID(progID, &clsid);
    if (FAILED(hr)) {
        return hr;
    }
    return CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)ppApp);
}

HRESULT GetPresentations(ult::ComPtr<IDispatch> ppApp, ult::ComPtr<IDispatch>& pres)
{
    if (!ppApp) {
        return E_INVALIDARG;
    }
    VARIANT result; // ref count can not down to 0
    VariantInit(&result);
    HRESULT hr = AutoWrap(DISPATCH_PROPERTYGET, &result, ppApp, L"Presentations", 0);
    pres = result.pdispVal;
    VariantClear(&result); // now we use comptr to take dispatch pointer, so we can free the memory
    return hr;
}

HRESULT OpenPresentation(ult::ComPtr<IDispatch> pres, const std::wstring& file, ult::ComPtr<IDispatch>& openedPres)
{
    if (!pres || file.empty()) {
        return E_INVALIDARG;
    }
    VARIANT result; // ref count can not down to 0
    VariantInit(&result);

    VARIANT fileName;
    fileName.vt = VT_BSTR;
    fileName.bstrVal = SysAllocString(file.c_str());

    VARIANT withWindow;
    withWindow.vt = VT_I4;
    withWindow.lVal = 0; // MsoTriState::msoFalse

    VARIANT readOnly;
    readOnly.vt = VT_I4;
    readOnly.lVal = -1; // MsoTriState::msoTrue

    VARIANT untitled;
    untitled.vt = VT_I4;
    untitled.lVal = 0; // MsoTriState::msoFalse

    HRESULT hr = AutoWrap(DISPATCH_METHOD, &result, pres, L"Open", 4, withWindow, untitled, readOnly, fileName);
    openedPres = result.pdispVal;

    VariantClear(&fileName);
    VariantClear(&result); // now we use comptr to take dispatch pointer, so we can free the memory
    return hr;
}

int GetFileType(const std::wstring& path)
{
    std::wstring realType = L"pdf";
    if (realType.empty()) {
        realType = L"pdf";
    }
    return 32;	// PpSaveAsFileType::ppSaveAsPDF
}

HRESULT SaveAs(ult::ComPtr<IDispatch> inst, const std::wstring& outfile, int type)
{
    if (!inst) {
        return E_INVALIDARG;
    }

    VARIANT fileName;
    fileName.vt = VT_BSTR;
    fileName.bstrVal = SysAllocString(outfile.c_str());

    VARIANT format;
    format.vt = VT_I4;
    format.lVal = type;

    VARIANT embedFont;
    embedFont.vt = VT_I4;
    embedFont.lVal = -2;	// MsoTriState::msoTriStateMixed

                            // If there are more than 1 parameters passed, they MUST be pass in 
                            // reversed order. Otherwise, you may get the error 0x80020009.
    return AutoWrap(DISPATCH_METHOD, NULL, inst, L"SaveAs", 3, embedFont, format, fileName);

    VariantClear(&fileName);
}

int Conv(const std::wstring& src, const std::wstring& dest)
{
    // TODO Init and uninit Com in libuv's common thread may cause problem
    HRESULT hr = CoInitializeEx(NULL,
                                COINIT_MULTITHREADED |
                                COINIT_DISABLE_OLE1DDE |
                                COINIT_SPEED_OVER_MEMORY);
    if (FAILED(hr)) {
        return kErrUnknown;
    }
    int ret = 0;
    ult::ComPtr<IDispatch> ppApp;
    ult::ComPtr<IDispatch> openedPres;
    do {
        hr = GetPowerPointDispatch(&ppApp);
        if (FAILED(hr)) {
            ret = kErrPowerPoint;
            break;
        }

        ult::ComPtr<IDispatch> pres;
        hr = GetPresentations(ppApp, pres);
        if (FAILED(hr)) {
            ret = kErrPresentations;
            break;
        }

        hr = OpenPresentation(pres, src, openedPres);
        if (FAILED(hr)) {
            ret = kErrOpenFile;
            break;
        }
        int typeInt = GetFileType(dest);
        if (typeInt < 0) {
            ret = kErrDestType;
            break;
        }
        hr = SaveAs(openedPres, dest, typeInt);
        if (FAILED(hr)) {
            ret = kErrSaveAs;
            break;
        }
    } while (false);

    if (openedPres) {
        AutoWrap(DISPATCH_METHOD, NULL, openedPres, L"Close", 0);
    }

    if (ppApp) {
        /////////////////////////////////////////////////////////////////////////
        // Quit the PowerPoint application. (i.e. Application.Quit())
        // 

        AutoWrap(DISPATCH_METHOD, NULL, ppApp, L"Quit", 0);
    }
    CoUninitialize();
    return ret;
}

int Conv(const std::string& src, const std::string& dest)
{
    return Conv(Utf8ToUtf16(src), Utf8ToUtf16(dest));
}