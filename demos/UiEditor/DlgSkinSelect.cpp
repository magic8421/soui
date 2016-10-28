#include "stdafx.h"
#include "DlgSkinSelect.h"
#include "helper/SplitString.h"
#include "CDebug.h"
#include "interface/SSkinobj-i.h"

#include "SSkinMutiFrameImg.h"
#include "helper\mybuffer.h"
#include "DlgNewSkin.h"
#include "DlgInput.h"

namespace SOUI
{

	SDlgSkinSelect::SDlgSkinSelect(LPCTSTR pszXmlName, SStringT strSkinName, SStringT strPath, BOOL bGetSkin):SHostDialog(pszXmlName)
	{
		m_strSkinName = strSkinName;
		//m_xmlNodeUiRes = m_xmlDocUiRes.append_copy(xmlNode);
		//m_xmlNodeUiRes = xmlNode;
		//CDebug::Debug(m_xmlDocUiRes);

	    m_strProPath = strPath.Mid(0, strPath.ReverseFind(_T('\\')));
		m_strUIResFile = strPath;
		m_bGetSkin = bGetSkin;
	}

	//TODO:��Ϣӳ��
	void SDlgSkinSelect::OnClose()
	{
		SHostDialog::OnCancel();
	}

	void SDlgSkinSelect::OnMaximize()
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
	}
	void SDlgSkinSelect::OnRestore()
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE);
	}
	void SDlgSkinSelect::OnMinimize()
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	}

	void SDlgSkinSelect::OnOK()
	{
		if (m_bGetSkin)
		{
			if (m_lbResType->GetCurSel() == 0)
			{

				if (m_lbRes->GetCurSel() < 0)
				{
					CDebug::Debug(_T("��ѡ������һ��ϵͳ��Դ"));
					return;
				}

				m_strSkinName = GetLBCurSelText(m_lbRes);
			}else
			{
				if (m_lbSkin->GetCurSel() < 0)
				{
					CDebug::Debug(_T("��ѡ��Ƥ��"));
					return;
				}

				SStringTList strList;
				SplitString(GetLBCurSelText(m_lbSkin), _T(':'), strList);
				if (strList.GetCount() != 2)
				{
					CDebug::Debug(_T("Ƥ��������ȷ!"));
					return;
				}

				strList.GetAt(1).Trim();

				m_strSkinName = strList[1];
			}
		}

		Save();

		SHostDialog::OnOK();
	}

	void SDlgSkinSelect::Save()
	{
		m_xmlDocUiRes.save_file(m_strUIResFile);
		m_xmlDocSkin.save_file(m_strSkinFile);
	}

	BOOL SDlgSkinSelect::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
	{
		m_lbResType = (SListBox *)FindChildByName(L"NAME_UIDESIGNER_LB_ZYLX");
		m_lbRes = (SListBox *)FindChildByName(L"NAME_UIDESIGNER_LB_ZY");
		m_lbSkin = (SListBox *)FindChildByName(L"NAME_UIDESIGNER_LB_SKIN");

		m_pEdit = (SEdit *)FindChildByName(L"NAME_UIDESIGNER_edit_SEARCH");

		m_imgView = (SImageWnd  *)FindChildByName(L"NAME_UIDESIGNER_IMG_VIEW");


		LRESULT lr=m_pEdit->SSendMessage(EM_SETEVENTMASK,0,ENM_CHANGE);
		m_pEdit->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&SDlgSkinSelect::OnReNotify,this));

		m_wndGridContainer = (SWindow *)FindChildByName(L"NAME_UIDESIGNER_GRID_CONTAINER");

		m_lbResType->GetEventSet()->subscribeEvent(EVT_LB_SELCHANGED,Subscriber(&SDlgSkinSelect::OnLbResTypeSelChanged,this));
		m_lbRes->GetEventSet()->subscribeEvent(EVT_LB_SELCHANGED,Subscriber(&SDlgSkinSelect::OnLbResSelChanged,this));
		m_lbSkin->GetEventSet()->subscribeEvent(EVT_LB_SELCHANGED,Subscriber(&SDlgSkinSelect::OnLbSkinSelChanged,this));

		LoadUIRes();
		LoadSysSkin();
		LoadSkinFile();
		InitResType();
		LoadSkinProperty();

		if (!m_strSkinName.IsEmpty())
		{  
			//��������˵�Ƥ������Ϊ�գ���λ��Ƥ��
			GoToSkin();
		}




		return TRUE;
	}

	void SDlgSkinSelect::LoadUIRes()
	{
		if (!m_xmlDocUiRes.load_file(m_strUIResFile))
		{
			CDebug::Debug(_T("����uires�ļ�ʧ��"));
			return;
		}

		m_xmlNodeUiRes = m_xmlDocUiRes.root();
	
	}

	void SDlgSkinSelect::LoadSkinFile()
	{
		pugi::xml_node xmlNode = m_xmlNodeUiRes.child(L"resource").child(L"UIDEF").first_child();
		SStringW strPath;
		strPath = xmlNode.attribute(L"path").value();
		//while (xmlNode)
		//{
		//	SStringW str(L"XML_INIT");
		//	if (str.CompareNoCase(xmlNode.attribute(L"name").value()) == 0 )
		//	{
		//		strPath = xmlNode.attribute(L"path").value();
		//		break;
		//	}
		//	xmlNode = xmlNode.next_sibling();
		//}

		if (!strPath.IsEmpty())
		{
			SStringT strInitFile;
			strInitFile = m_strProPath + _T("\\") + strPath;

			m_strSkinFile = strInitFile;

			pugi::xml_parse_result result = m_xmlDocSkin.load_file(strInitFile);
			if (result)
			{
				//pugi::xml_writer_buff writer;
				//m_xmlDocSkin.root().print(writer,L"\t",pugi::format_default,pugi::encoding_utf16);
				//SStringW *strDebug= new SStringW(writer.buffer(),writer.size());
				//SMessageBox(NULL, *strDebug, _T(""), MB_OK);


				pugi::xml_node xmlNode1 = m_xmlDocSkin.child(L"UIDEF").child(L"skin");
				if (xmlNode1.attribute(L"src"))
				{
					SStringT strSrc = xmlNode1.attribute(L"src").value();
					SStringTList strLst;
					SplitString(strSrc,_T(':'),strLst);
					if(strLst.GetCount() != 2) 
					{
						SASSERT_FMTW(L"Parse pos attribute failed, src=%s",strSrc);
						return ;
					}

					strLst.GetAt(0).TrimBlank();

					strLst[0];

					m_strSkinFile = m_strProPath + _T("\\") + strLst[0] + _T("\\") + strLst[1] + _T(".xml");
					result = m_xmlDocSkin.load_file(m_strSkinFile);
					if (!result)
					{
						SMessageBox(NULL, _T("����skin�ļ�ʧ��"), _T("����skin�ļ�ʧ��"), MB_OK);
					}
				}
			}
		}

	}

	void SDlgSkinSelect::InitResType()
	{
		m_lbResType->AddString(_T("����Ƥ��"));

		pugi::xml_node xmlNode = m_xmlNodeUiRes.child(L"resource").first_child();

		while (xmlNode)
		{
			m_lbResType->AddString(xmlNode.name());

			xmlNode = xmlNode.next_sibling();

		}

	}

	void SDlgSkinSelect::LoadSysSkin()
	{
		pugi::xml_parse_result resutl = m_xmlDocSysSkin.load_file(L"Config\\theme_sys_res\\sys_xml_skin.xml");
		if (!resutl)
		{
			SMessageBox(NULL, _T("����SysSkin.xml�ļ�ʧ��"), _T("����SysSkin.xml�ļ�ʧ��"), MB_OK);
			return;
		}


	}

	bool SDlgSkinSelect::OnLbResTypeSelChanged(EventArgs *pEvtBase)
	{
		DestroyGrid();
		EventLBSelChanged *pEvt =(EventLBSelChanged*)pEvtBase;
		SListBox *listbox=(SListBox*)pEvt->sender;
		pugi::xml_node xmlNode;

		if (pEvt->nNewSel == 0)
		{
			m_lbRes->DeleteAll();
			m_mapSysSkin.RemoveAll();

			xmlNode = m_xmlDocSysSkin.child(L"skin").first_child();
			while (xmlNode)
			{
	
				SStringT *strData = new SStringT(xmlNode.name());

				m_lbRes->AddString(xmlNode.attribute(L"name").value(), -1, (LPARAM)strData);

				if (xmlNode.attribute(L"src"))
				{
					SStringTList strlist;
					SStringT s = xmlNode.attribute(L"src").value();
					SplitString(s, _T(':'), strlist);

					if (strlist.GetCount() == 2)
					{
						m_mapSysSkin[xmlNode.attribute(L"name").value()] = strlist[1] + _T(".png");
					}
				}




				xmlNode = xmlNode.next_sibling();
			}
		}
		else
		{
			SStringT strText;
			listbox->GetText(pEvt->nNewSel, strText);
			m_lbRes->DeleteAll();
			xmlNode = m_xmlNodeUiRes.child(L"resource").child(strText).first_child();
			while(xmlNode)
			{

				SStringT *strData = new SStringT(xmlNode.attribute(L"name").value());
				m_lbRes->AddString(xmlNode.attribute(L"path").value(), -1, (LPARAM)strData);

				xmlNode = xmlNode.next_sibling();
			}
		}

		m_lbSkin->DeleteAll();
		return true;
	}

	bool SDlgSkinSelect::OnLbResSelChanged(EventArgs *pEvtBase)
	{
		DestroyGrid();
		//if (m_lbResType->GetCurSel() == 0)
		//{
		//	ShowSysImage();
		//	m_lbRes->SetFocus();
		//	return true;
		//}






		EventLBSelChanged *pEvt =(EventLBSelChanged*)pEvtBase;
		SListBox *listbox=(SListBox*)pEvt->sender;
		pugi::xml_node xmlNode;
		if (pEvt->nNewSel == -1)
		{
			return false;
		}
		SStringT *s  = (SStringT *)listbox->GetItemData(pEvt->nNewSel);

		SStringT strTemp(*s);


		if (m_lbResType->GetCurSel() == 0)
		{
			m_lbSkin->DeleteAll();
			m_lbSkin->AddString(strTemp + _T(":  ") + GetLBCurSelText(m_lbRes));
			if (m_lbSkin->GetCount()>0)
			{
				SelectLBItem(m_lbSkin, 0);
			}

			ShowImage();
			m_lbRes->SetFocus();
			return true;
		}


		int n = m_lbResType->GetCurSel();

		SStringT strResName;

		m_lbResType->GetText(n, strResName);

		strTemp = strResName + _T(":") + strTemp;

		if (m_xmlDocSkin.child(L"skin"))
		{
			xmlNode = m_xmlDocSkin.child(L"skin").first_child();
		}else if(m_xmlDocSkin.child(L"UIDEF"))
		{
			xmlNode = m_xmlDocSkin.child(L"UIDEF").child(L"skin").first_child();
		}
		m_lbSkin->DeleteAll();


		while (xmlNode)
		{
			if (strTemp.CompareNoCase(xmlNode.attribute(L"src").value()) == 0)
			{
				SStringT s2, s3;
				s2 = xmlNode.name();
				s3 = xmlNode.attribute(L"name").value();

				m_lbSkin->AddString(s2 + _T(":  ") + s3);
			}

			xmlNode = xmlNode.next_sibling();
		}

		if (m_lbSkin->GetCount()>0)
		{
			m_lbSkin->SetCurSel(0);
			EventLBSelChanged evt(m_lbSkin);
			evt.nOldSel = 0;
			evt.nNewSel = 0;
			m_lbSkin->FireEvent(evt);
		}
		m_lbRes->SetFocus();

		ShowImage();
		return true;
	}

	bool SDlgSkinSelect::OnLbSkinSelChanged(EventArgs *pEvtBase)
	{

		EventLBSelChanged *pEvt =(EventLBSelChanged*)pEvtBase;
		SListBox *listbox=(SListBox*)pEvt->sender;
		pugi::xml_node xmlNode;

		int n = m_lbSkin->GetCurSel();

		SStringT strSkinName;

		m_lbSkin->GetText(n, strSkinName);

		SStringTList strLst;
		SplitString(strSkinName,_T(':'),strLst);

		SStringT strSkinType = strLst.GetAt(0).Trim();
		SStringT strSkin = strLst.GetAt(1).Trim();

		if (m_xmlDocSkin.child(L"skin"))
		{
			xmlNode = m_xmlDocSkin.child(L"skin").first_child();
		}else if(m_xmlDocSkin.child(L"UIDEF"))
		{
			xmlNode = m_xmlDocSkin.child(L"UIDEF").child(L"skin").first_child();
		}

		while (xmlNode)
		{
			if (xmlNode.attribute(L"name"))
			{
				if (strSkinType.CompareNoCase(xmlNode.name()) == 0)
				{
					if (strSkin.CompareNoCase(xmlNode.attribute(L"name").value()) == 0)
					{
						m_xmlNodeCurSkin = xmlNode;
					}
				}
			}

			xmlNode = xmlNode.next_sibling();

		}


		DestroyGrid();

		xmlNode = m_xmlDocSkinProperty.child(L"root").child(strSkinType);
		if (xmlNode)
		{
			if (xmlNode)
			{
				m_wndGridContainer->CreateChildren(xmlNode);
				m_pgGrid = (SPropertyGrid *)m_wndGridContainer->GetWindow(GSW_FIRSTCHILD);
				m_wndGridContainer->Invalidate();
				m_pgGrid-> GetEventSet()->subscribeEvent(EventPropGridValueChanged::EventID,Subscriber(&SDlgSkinSelect::OnPropGridValueChanged,this));
				UpdatePropGrid();
			}
		}

		return true;
	}


	void SDlgSkinSelect::LoadSkinProperty()
	{
		pugi::xml_parse_result resutl = m_xmlDocSkinProperty.load_file(L"Config\\SkinProperty.xml");
		if (!resutl)
		{
			SMessageBox(NULL, _T("����SkinProperty.xml�ļ�ʧ��"), _T("����SkinProperty.xml�ļ�ʧ��"), MB_OK);
			return;
		}
	}



	void SDlgSkinSelect::UpdatePropGrid()
	{
		if (m_pgGrid == NULL)
		{
			return;
		}

		m_pgGrid->ClearAllGridItemValue();

		int n = m_lbSkin->GetCurSel();

		SStringT strSkinName;

		m_lbSkin->GetText(n, strSkinName);

		SStringTList strLst;
		SplitString(strSkinName,_T(':'),strLst);

		for(size_t i=0;i<strLst.GetCount();i++)
		{
			strLst.GetAt(i).TrimBlank();
		}

		pugi::xml_node xmlNode;
		if (m_xmlDocSkin.child(L"skin"))
		{
			xmlNode = m_xmlDocSkin.child(L"skin").first_child();
		}else if(m_xmlDocSkin.child(L"UIDEF"))
		{
			xmlNode = m_xmlDocSkin.child(L"UIDEF").child(L"skin").first_child();
		}

		while (xmlNode)
		{
			if (strLst[0].CompareNoCase(xmlNode.name()) == 0 && 
				strLst[1].CompareNoCase(xmlNode.attribute(L"name").value()) == 0)
			{
				break;
			}

			xmlNode = xmlNode.next_sibling();
		}


		if (!xmlNode)
		{
			return;
		}

		pugi::xml_attribute xmlAttr = xmlNode.first_attribute();

		//SStringT sTemp2 = Debug1(xmlNode);


		while (xmlAttr)
		{
			SStringT str = xmlAttr.name();
			IPropertyItem *pItem = m_pgGrid->GetGridItem(str.MakeLower());
			if (pItem)
			{

				pItem->SetStringOnly(xmlAttr.value());
			}

			xmlAttr = xmlAttr.next_attribute();
		}

		m_pgGrid->Invalidate();
	}


	void SDlgSkinSelect::OnZYLXNew()
	{
		//SDlgInput dlg();

		SStringT strName;
		SDlgInput dlg;
		if (IDOK != dlg.DoModal(m_hWnd))
		{
			return;
		}

		strName = dlg.m_strValue;


		pugi::xml_node xmlNode = m_xmlNodeUiRes.child(L"resource");
		if (xmlNode.child(strName))
		{
			CDebug::Debug(_T("����Դ�����Ѵ��ڣ�"));
			return;
		}

		xmlNode = xmlNode.append_child(strName);
		int n = m_lbResType->AddString(strName);
		SelectLBItem(m_lbResType, n);


	}
	void SDlgSkinSelect::OnZYLXDel()
	{
		if (m_lbResType->GetCurSel() < 0)
		{
			CDebug::Debug(_T("��ѡ����Դ����"));
			return;
		}

		if (m_lbResType->GetCurSel() == 0)
		{
			CDebug::Debug(_T("����ѡ������Ƥ������"));
			return;
		}

		if (m_lbRes->GetCount() > 0)
		{
			CDebug::Debug(_T("����ɾ����Դ"));
			return;
		}

		pugi::xml_node xmlNode = m_xmlNodeUiRes.child(L"resource").child(GetLBCurSelText(m_lbResType));
		if (xmlNode)
		{
			xmlNode.parent().remove_child(xmlNode);
			m_lbResType->DeleteString(m_lbResType->GetCurSel());
			/*m_lbResType->Invalidate();*/
			return;
		}

	}


	void SDlgSkinSelect::OnZYNew()
	{
		if (m_lbResType->GetCurSel() < 0)
		{
			CDebug::Debug(_T("��ѡ����Դ����"));
			return;
		}

		if (m_lbResType->GetCurSel() == 0)
		{
			CDebug::Debug(_T("����ѡ������Ƥ������"));
			return;
		}

		CFileDialogEx OpenDlg(TRUE, NULL, NULL, 6, _T("�����ļ� (*.*)\0*.*\0\0"));
		if (IDOK ==OpenDlg.DoModal())
		{
			SStringT strFileName = OpenDlg.m_szFileName;
			int n = strFileName.Find(m_strProPath);
			if (n != 0)
			{
				SMessageBox(NULL, _T("�뽫��Դ�ŵ�uiresĿ¼��"), _T("��ʾ"), MB_OK);
				return;
			}

			SStringT strFile = strFileName.Mid(m_strProPath.GetLength() + 1);

			n = m_lbResType->GetCurSel();
			SStringT strResType;
			m_lbResType->GetText(n, strResType);

			pugi::xml_node xmlNode = m_xmlNodeUiRes.child(L"resource").child(strResType).first_child();
			pugi::xml_node xmlNewNode;

			while (xmlNode)
			{
				if (strFile.CompareNoCase(xmlNode.attribute(L"path").value()) == 0)
				{
					break;
				}

				xmlNode = xmlNode.next_sibling();
			}

			if (!xmlNode)
			{
				// ����ò����ڸ����͵���Դ��������	
				xmlNewNode = m_xmlNodeUiRes.child(L"resource").child(strResType).append_child(L"file");

				SStringT strResName = strFile;
				strResName.Replace(_T("\\"), _T("_"));
				strResName.Replace(_T("."), _T("_"));

				xmlNewNode.append_attribute(L"name").set_value(strResName);
				xmlNewNode.append_attribute(L"path").set_value(strFile);

				SStringT *strData = new SStringT(strResName);
				m_lbRes->AddString(strFile, -1, (LPARAM)strData);
				//CDebug::Debug(xmlNewNode);
			}

			//��λ����Դ
			{
				SStringT strResText;
				for (int i = 0; i < m_lbRes->GetCount(); i++)
				{
					m_lbRes->GetText(i, strResText);
					if (strResText.Compare(strFile) == 0)
					{
						m_lbRes->SetCurSel(i);

						EventLBSelChanged evt(m_lbRes);
						evt.nOldSel = 0;
						evt.nNewSel = i;
						m_lbRes->FireEvent(evt);

						int nMinPos, nMaxPos;
						m_lbRes->GetScrollRange(TRUE, &nMinPos, &nMaxPos);

						m_lbRes->SetScrollPos(TRUE, nMaxPos /m_lbRes->GetCount() * i, TRUE );				 


						if (m_lbSkin->GetCount()>0)
						{
							m_lbSkin->SetCurSel(0);
							EventLBSelChanged evt1(m_lbSkin);
							evt1.nOldSel = 0;
							evt1.nNewSel = 0;
							m_lbSkin->FireEvent(evt1);
						}
						m_lbRes->SetFocus();

						break;

					}

				}
			}
		}
	}
	void SDlgSkinSelect::OnZYDel()
	{

		SStringT strResText = GetLBCurSelText(m_lbRes);
		if (strResText.IsEmpty())
		{
			return;
		}

		SStringT strResType = GetLBCurSelText(m_lbResType);
		if (strResType.IsEmpty())
		{
			return;
		}


		int nResult =  SMessageBox(NULL, _T("ȷ��Ҫɾ����Դ��?"), _T("��ʾ"), MB_OKCANCEL);
		if (nResult != 1)
		{
			return;
		}

		if (m_lbSkin->GetCount() > 0)
		{
			SMessageBox(NULL, _T("�����Ƴ�Ƥ��"), _T("��ʾ"), MB_OK);
			return;
		}

		pugi::xml_node xmlNode = m_xmlNodeUiRes.child(L"resource").child(strResType).first_child();
		while (xmlNode)
		{
			if (strResText.CompareNoCase(xmlNode.attribute(L"path").value()) == 0)
			{

				pugi::xml_node xmlNodeP = xmlNode.parent();
				xmlNodeP.remove_child(xmlNode);
				m_lbRes->DeleteString(m_lbRes->GetCurSel());
				return;
			}
			xmlNode = xmlNode.next_sibling();
		}

	}

	//�½�Ƥ��
	void SDlgSkinSelect::OnSkinNew()
	{
		if (m_lbRes->GetCurSel() < 0)
		{
			CDebug::Debug(_T("����ѡ����Դ"));
			return;
		}

		SStringT strSkinTypeName;
		SDlgNewSkin DlgNewSkin(_T("layout:UIDESIGNER_XML_NEW_SKIN"));
		if (IDOK == DlgNewSkin.DoModal(m_hWnd))
		{
			strSkinTypeName = DlgNewSkin.m_strSkinName;
		}else
		{
			return;
		}


		pugi::xml_node xmlNode;

		if (m_xmlDocSkin.child(L"skin"))
		{
			xmlNode = m_xmlDocSkin.child(L"skin");
		}else if(m_xmlDocSkin.child(L"UIDEF"))
		{
			xmlNode = m_xmlDocSkin.child(L"UIDEF").child(L"skin");
		}

		xmlNode = xmlNode.append_child(strSkinTypeName);

		SStringT strSrc;
		strSrc = GetLBCurSelText(m_lbResType) + _T(":");
		SStringT *s  = (SStringT *)m_lbRes->GetItemData(m_lbRes->GetCurSel());
		strSrc = strSrc + *s;


		xmlNode.append_attribute(_T("src")).set_value(strSrc);
		xmlNode.append_attribute(_T("name")).set_value(*s);

		
		int n = m_lbSkin->AddString(strSkinTypeName + _T(": ") + *s);
		SelectLBItem(m_lbSkin, n);

		//SStringT strSkinName(*s);

		//SSkinPool *pBuiltinSkinPool = SSkinPoolMgr::getSingletonPtr()->GetBuiltinSkinPool();
		//ISkinObj *pSkin=pBuiltinSkinPool->GetSkin(strSkinName); // 
		//if(!pSkin)
		//{
		//	pSkin = SApplication::getSingleton().CreateSkinByName(strSkinTypeName);
		//	if(!pSkin) return ;

		//	pBuiltinSkinPool->AddKeyObject(strSkinName, pSkin);//��������skin����skinpool����
		//}

	}

	//ɾ��Ƥ��
	void SDlgSkinSelect::OnSkinDel()
	{
		if (m_lbSkin->GetCurSel() < 0)
		{
			return;
		}

		int nResult =  SMessageBox(NULL, _T("ȷ��Ҫɾ��Ƥ����?"), _T("��ʾ"), MB_OKCANCEL);
		if (nResult != 1)
		{
			return;
		}

		SStringTList strList;
		SplitString(GetLBCurSelText(m_lbSkin), _T(':'), strList);

		for (int i = 0; i < strList.GetCount(); i++)
		{
			strList.GetAt(i).TrimBlank();
		}

		pugi::xml_node xmlNode;

		if (m_xmlDocSkin.child(L"skin"))
		{
			xmlNode = m_xmlDocSkin.child(L"skin").first_child();
		}else if(m_xmlDocSkin.child(L"UIDEF"))
		{
			xmlNode = m_xmlDocSkin.child(L"UIDEF").child(L"skin").first_child();
		}

		while (xmlNode)
		{
			if (strList[0].CompareNoCase(xmlNode.name()) == 0)
			{
				pugi::xml_attribute attr = xmlNode.attribute(_T("name"));
				if (attr)
				{
					if (strList[1].CompareNoCase(attr.value()) == 0)
					{
						xmlNode.parent().remove_child(xmlNode);
						SelectLBItem(m_lbRes, m_lbRes->GetCurSel());
						break;
					}
				}
				
			}

			xmlNode = xmlNode.next_sibling();
		}


	}

	void SDlgSkinSelect::DestroyGrid()
	{
		m_pgGrid = (SPropertyGrid *)m_wndGridContainer->GetWindow(GSW_FIRSTCHILD);
		if (m_pgGrid)
		{
			m_pgGrid->SetFocus();

			//Debug(tempDoc->root().first_child());

			m_wndGridContainer->DestroyChild(m_pgGrid);
			m_pgGrid = NULL;
		}

	}


	SStringT SDlgSkinSelect::GetLBCurSelText(SListBox * lb)
	{
		SStringT s(_T(""));
		int n = lb->GetCurSel();

		if (n < 0)
		{
			return s;
		}

		lb->GetText(n, s);
		return s;
	}


	void SDlgSkinSelect::SelectLBItem(SListBox * lb, int nIndex)
	{
		//���Ȳ���ϵͳƤ��
		lb->SetCurSel(nIndex);
		EventLBSelChanged evt(lb);
		evt.nOldSel = lb->GetCurSel();
		evt.nNewSel = nIndex;
		lb->FireEvent(evt);

		int nMinPos, nMaxPos;
		lb->GetScrollRange(TRUE, &nMinPos, &nMaxPos);

		lb->SetScrollPos(TRUE, nMaxPos /lb->GetCount() * nIndex, TRUE );




	}


	bool SDlgSkinSelect::OnReNotify(EventArgs *pEvt)
	{
		//����edit�¼�֪ͨ

		if (m_lbResType->GetCurSel() < 0)
		{
			return true;
		}


		EventRENotify *pReEvt = (EventRENotify*)pEvt;
		if(pReEvt->iNotify == EN_CHANGE)
		{
			SStringT strValue=m_pEdit->GetWindowText();
			m_lbRes->DeleteAll();


			if (m_lbResType->GetCurSel() == 0)
			{
				pugi::xml_node xmlNode = m_xmlDocSysSkin.child(L"skin").first_child();
				while (xmlNode)
				{
					SStringT strPath = xmlNode.attribute(L"name").value();



					if (strPath.Find(strValue) >= 0)
					{
						SStringT *strData = new SStringT(xmlNode.name());

						m_lbRes->AddString(xmlNode.attribute(L"name").value(), -1, (LPARAM)strData);
					}

					xmlNode = xmlNode.next_sibling();
				}
			}
			else
			{
				pugi::xml_node  xmlNode = m_xmlNodeUiRes.child(L"resource").child(GetLBCurSelText(m_lbResType)).first_child();
				while(xmlNode)
				{
					SStringT strPath = xmlNode.attribute(L"path").value();



					if (strPath.Find(strValue) >= 0)
					{
						SStringT *strData = new SStringT(xmlNode.attribute(L"name").value());
						m_lbRes->AddString(strPath, -1, (LPARAM)strData);
					}

					xmlNode = xmlNode.next_sibling();
				}
			}

		}
		return true;
	}


	void SDlgSkinSelect::GoToSkin()
	{
		//���Ȳ���ϵͳ��Դ
		pugi::xml_node xmlNode = m_xmlDocSysSkin.child(L"skin").first_child();
		while (xmlNode)
		{
			if (m_strSkinName.CompareNoCase(xmlNode.attribute(L"name").value()) == 0)
			{
				SelectLBItem(m_lbResType, 0);
				SStringT strResText;

				for (int i = 0; i < m_lbRes->GetCount(); i++)
				{
					m_lbRes->GetText(i, strResText);
					if (m_strSkinName.CompareNoCase(strResText) == 0)
					{
						SelectLBItem(m_lbRes, i);
						return;
					}
				}

				return;
			}

			xmlNode = xmlNode.next_sibling();

		}




		//if (m_xmlDocSysSkin.child(L"skin").child(m_strSkinName))
		//{
		//	//���Ȳ���ϵͳƤ��

		//	SelectLBItem(m_lbResType, 0);
		//	SStringT strResText;

		//	for (int i = 0; i < m_lbRes->GetCount(); i++)
		//	{
		//		m_lbRes->GetText(i, strResText);
		//		if (m_strSkinName.CompareNoCase(strResText) == 0)
		//		{
		//			SelectLBItem(m_lbRes, i);
		//			break;
		//		}
		//	}

		//}else
		{
			pugi::xml_node xmlNode;
			SStringT strSrc;

			if (m_xmlDocSkin.child(L"skin"))
			{
				xmlNode = m_xmlDocSkin.child(L"skin").first_child();
			}else if(m_xmlDocSkin.child(L"UIDEF"))
			{
				xmlNode = m_xmlDocSkin.child(L"UIDEF").child(L"skin").first_child();
			}

			while (xmlNode)
			{
				if (m_strSkinName.CompareNoCase(xmlNode.attribute(L"name").value()) == 0)
				{
					strSrc = xmlNode.attribute(L"src").value();
					break;
				}

				xmlNode = xmlNode.next_sibling();
			}

			if (!xmlNode)
			{
				CDebug::Debug(m_strSkinName + _T("�Ҳ���!"));
				return;
			}


			SStringTList strList;
			SplitString(strSrc, _T(':'), strList);

			for (int i = 0; i < strList.GetCount(); i++)
			{
				strList.GetAt(i).TrimBlank();
			}

			int n = GetLbIndexFromText(m_lbResType, strList[0]);
			if (n == -1)
			{
				CDebug::Debug(_T("δ֪����Դ����!"));
				return;
			}

			SelectLBItem(m_lbResType, n);

			SStringT strPath;

			xmlNode = m_xmlNodeUiRes.child(L"resource").child(strList[0]).first_child();
			while (xmlNode)
			{

				if (strList[1].CompareNoCase(xmlNode.attribute(L"name").value()) == 0)
				{
					strPath = xmlNode.attribute(L"path").value();

					break;
				}

				xmlNode = xmlNode.next_sibling();
			}

			if (!xmlNode)
			{
				CDebug::Debug(_T("δ֪����Դ����"));
				return;
			}

			n = GetLbIndexFromText(m_lbRes, strPath);
			if (n == -1)
			{
				CDebug::Debug(_T("δ֪����Դ!"));
				return;
			}

			SelectLBItem(m_lbRes, n);




			//n = GetLbIndexFromText(m_lbRes, strList[1]);
			//if (n == -1)
			//{
			// CDebug::Debug(_T("δ֪��Ƥ��!"));
			// return;
			//}

			//SelectLBItem(m_lbRes)




		}
	}


	int SDlgSkinSelect::GetLbIndexFromText(SListBox *lb, SStringT strText)
	{
		int n = -1;
		if (lb->GetCount() == 0)
		{
			return -1;
		}

		SStringT strLbText;
		for (int i = 0; i < lb->GetCount(); i ++)
		{
			lb->GetText(i, strLbText);
			if (strLbText.CompareNoCase(strText) == 0)
			{
				n = i;
				break;
			}
		}

		return n;
	}


	void SDlgSkinSelect::ShowImage()
	{
		SStringT strImgname = GetLBCurSelText(m_lbRes);
		if (m_lbResType->GetCurSel() == 0)
		{
			SMap<SStringT, SStringT>::CPair *p = m_mapSysSkin.Lookup(strImgname);
			if (!p)
			{
				return;
			}

			strImgname =  _T("Config\\theme_sys_res\\")+ p->m_value;
		}else
		{
					strImgname = m_strProPath + _T("\\")+ strImgname;
		}




		SSkinPool *pBuiltinSkinPool = SSkinPoolMgr::getSingletonPtr()->GetBuiltinSkinPool();
		ISkinObj *pSkin=pBuiltinSkinPool->GetSkin(strImgname); // ���ļ�����ΪKEY
		SSkinAni *aniSkin;
		if(pSkin)
		{
			if(!pSkin->IsClass(SSkinAni::GetClassName())) return ;
			aniSkin=static_cast<SSkinAni*>(pSkin);
		}else
		{
			SSkinAni *pSkin = (SSkinAni*)SApplication::getSingleton().CreateSkinByName(SSkinMutiFrameImg::GetClassName());
			//pSkin = SApplication::getSingleton().CreateSkinByName(SSkinMutiFrameImg::GetClassName());
			if(!pSkin) return ;
			if(0==pSkin->LoadFromFile(strImgname))
			{
				pSkin->Release();
				return ;
			}

			pSkin->SetAttribute(L"filterLevel", L"high");
			pBuiltinSkinPool->AddKeyObject(strImgname, pSkin);//��������skin����skinpool����
			aniSkin = pSkin;
		}

		int nWidth, nHeight, nPWidth, nPHeight;
		nWidth = aniSkin->GetSkinSize().cx;
		nHeight = aniSkin->GetSkinSize().cy;

		CRect rect;
		m_imgView->GetParent()->GetWindowRect(rect);
		nPWidth = rect.right - rect.left;
		nPHeight = rect.bottom - rect.top;
		if (nWidth > nPWidth)
		{
			float bl = (float)nPWidth / (float)nWidth;

			nWidth = (float)nWidth * bl;
			nHeight = (float)nHeight * bl;
		}
		m_imgView->SetAttribute(L"skin", strImgname);
		SwndLayout *playout = m_imgView->GetLayout();
		playout->SetWidth(nWidth);
		playout->SetHeight(nHeight);


		m_imgView->GetParent()->UpdateChildrenPosition();
	}


	void SDlgSkinSelect::ShowSysImage()
	{
		SStringT strImgname = GetLBCurSelText(m_lbRes);

		SSkinPool *pBuiltinSkinPool = SSkinPoolMgr::getSingletonPtr()->GetBuiltinSkinPool();
		ISkinObj *pSkin=pBuiltinSkinPool->GetSkin(strImgname); 
		if(!pSkin)
		{
			return;
		}


		int nWidth, nHeight, nPWidth, nPHeight;
		nWidth = pSkin->GetSkinSize().cx;
		nHeight = pSkin->GetSkinSize().cy;

		CRect rect;
		m_imgView->GetParent()->GetWindowRect(rect);
		nPWidth = rect.right - rect.left;
		nPHeight = rect.bottom - rect.top;
		if (nWidth > nPWidth)
		{
			float bl = (float)nPWidth / (float)nWidth;

			nWidth = (float)nWidth * bl;
			nHeight = (float)nHeight * bl;
		}

		SwndLayout *playout = m_imgView->GetLayout();
		playout->SetWidth(nWidth);
		playout->SetHeight(nHeight);

		m_imgView->SetAttribute(L"skin", strImgname);
		m_imgView->GetParent()->UpdateChildrenPosition();
	}


bool SDlgSkinSelect::OnPropGridValueChanged( EventArgs *pEvt )
{

	if (m_lbResType->GetCurSel() == 0)
	{
		return true;
	}

	pugi:: xml_node xmlNode;

	IPropertyItem* pItem = ((EventPropGridValueChanged*)pEvt)->pItem;
	SStringT s = pItem->GetName2();  //��������pos skin name id �ȵ�

	SStringT s1 = pItem->GetString();   //���Ե�ֵ

	if (s.IsEmpty())
	{
		return false;
	}

	
	xmlNode = m_xmlNodeCurSkin;


	pugi::xml_attribute attr = xmlNode.attribute(s);
	if (attr)
	{
		if (s1.IsEmpty())
		{

			  xmlNode.remove_attribute(s);
		}else
		{
			attr.set_value(s1);
		}
	}
	else
	{
		if (!s1.IsEmpty())
		{
			xmlNode.append_attribute(s).set_value(s1);
		}
	}



	return true;
}

}

