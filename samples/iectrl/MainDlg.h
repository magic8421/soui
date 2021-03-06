// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

class CMainDlg : public SHostWnd
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose()
	{
        PostMessage(WM_QUIT);
	}
	void OnMaximize()
	{
		SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE);
	}
	void OnRestore()
	{
		SendMessage(WM_SYSCOMMAND,SC_RESTORE);
	}
	void OnMinimize()
	{
		SendMessage(WM_SYSCOMMAND,SC_MINIMIZE);
	}

	void OnSize(UINT nType, CSize size)
	{
		SetMsgHandled(FALSE);
        SWindow *pBtnRestore = FindChildByName(L"btn_restore");
        SWindow *pBtnMax = FindChildByName(L"btn_max");
        if(pBtnMax && pBtnRestore)
        {
            if(nType==SIZE_MAXIMIZED)
            {
                pBtnRestore->SetVisible(TRUE);
                pBtnMax->SetVisible(FALSE);
            }else if(nType==SIZE_RESTORED)
            {
                pBtnRestore->SetVisible(FALSE);
                pBtnMax->SetVisible(TRUE);
            }
        }
	}
    
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

    void OnBtnCallJS();
protected:

	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close",OnClose)
        EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
        EVENT_NAME_COMMAND(L"btn_max",OnMaximize)
        EVENT_NAME_COMMAND(L"btn_restore",OnRestore)
        EVENT_NAME_COMMAND(L"btn_calljs",OnBtnCallJS)
	EVENT_MAP_END()	

	BEGIN_MSG_MAP_EX(CMainDlg)
        MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
};
