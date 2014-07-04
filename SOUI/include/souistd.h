
#ifdef DLL_SOUI
# ifdef SOUI_EXPORTS
# pragma message("dll export")
#  define SOUI_EXP __declspec(dllexport)
# else
#  define SOUI_EXP __declspec(dllimport)
# endif // SOUI_EXPORTS
#else
#define SOUI_EXP
#endif

#define OR_API SOUI_EXP

// Change these values to use different versions
#define WINVER        0x0500
#define _WIN32_WINNT    0x0501
#define _WIN32_IE    0x0601
#define _RICHEDIT_VER    0x0200

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS


# pragma warning(disable:4661)
# pragma warning(disable:4251)
# pragma warning(disable:4100)    //unreferenced formal parameter
# pragma warning(disable:4355)

#include <Windows.h>
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <OleCtl.h>
#include <tchar.h>
#include <stdio.h>

#include "core/SDefine.h"
#include "helper/color.h"
//export pugixml interface
#include "../pugixml/pugixml.hpp"

#ifdef USING_ATL
    #define _COLL_NS    ATL
    #include <atlbase.h>
    #include <atlapp.h>
    #include <atlmisc.h>
    #include <atlgdi.h>
    #include <atlstr.h>
    #include <atlcoll.h>
    #include <atlcomcli.h>
    #define CDuiComPtr    CComPtr
    #define CDuiComQIPtr    CComQIPtr

    #include "wtl.mini/duicrack.h"
    #define SArray    CAtlArray
    #define SList    CAtlList
    #define SMap        CAtlMap
    #define SStringA    CAtlStringA
    #define SStringW CAtlStringW
    #define SStringT CAtlString

    #include "../../utilities/include/string/strcpcvt.h"
#else//ATL_FREE
    #define _WTYPES_NS SOUI
    #define _COLL_NS    SOUI
    #include "wtl.mini/msgcrack.h"
    #include "wtl.mini/souimisc.h"
    #include "wtl.mini/souigdi.h"
    #include "wtl.mini/souicoll.h"
    #include "atl.mini/atldef.h"
    #include "atl.mini/scomcli.h"
    #include "../../utilities/include/string/tstring.h"
    #include "../../utilities/include/string/strcpcvt.h"
#endif//USING_ATL

#include "../../utilities/include/trace.h"
#include "../../utilities/include/utilities.h"


#include "render/render-i.h"
#include "render/imgdecoder-i.h"

#include "helper/SAttrCracker.h"
#include "res.mgr/sfontpool.h"
#include "res.mgr/sresprovider.h"
#include "SApp.h"
#include "control/SMessageBox.h"


#pragma comment(lib,"Msimg32.lib")
#pragma comment(lib,"shlwapi.lib")

#ifdef DLL_SOUI
    #ifdef _DEBUG
        #pragma comment(lib,"utilities_d.lib")
    #else
        #pragma comment(lib,"utilities.lib")
    #endif
#endif