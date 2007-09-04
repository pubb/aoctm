// Statdlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "Statdlg.h"


//by mep for statistic
CString civ_name[19] = {_T(""),
						_T("Bri"),
						_T("Fra"),
						_T("Got"),
						_T("Teu"),
						_T("Jap"),
						_T("Chi"),
						_T("Byz"),
						_T("Per"),
						_T("Sar"),
						_T("Tur"),
						_T("Vik"),
						_T("Mon"),
						_T("Cel"),
						_T("Spa"),
						_T("Azt"),
						_T("May"),
						_T("Hun"),
						_T("Kor")};
/*
CString civ_name[19] = {_T(""),
						_T("大不列颠"),
						_T("法兰克"),
						_T("哥特"),
						_T("条顿"),
						_T("日本"),
						_T("中国"),
						_T("拜占庭"),
						_T("波斯"),
						_T("萨拉逊"),
						_T("土耳其"),
						_T("维京"),
						_T("蒙古"),
						_T("凯尔特"),
						_T("西班牙"),
						_T("阿兹特克"),
						_T("玛雅"),
						_T("匈奴"),
						_T("高丽")};
*/
// Statdlg dialog

IMPLEMENT_DYNAMIC(Statdlg, CDialog)

Statdlg::Statdlg(CWnd* pParent /*=NULL*/)
	: CDialog(Statdlg::IDD, pParent)
{

}

Statdlg::~Statdlg()
{
}

void Statdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}


BEGIN_MESSAGE_MAP(Statdlg, CDialog)
END_MESSAGE_MAP()


// Statdlg message handlers

BOOL Statdlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_List.InsertColumn( 0, _T("Player Name"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn( 1, _T("Mostly Used Civ"), LVCFMT_LEFT, 110, -1);
	m_List.InsertColumn( 2, _T("Min Feud Time"), LVCFMT_LEFT, 85, -1);
	m_List.InsertColumn( 3, _T("Avg Feud Time"), LVCFMT_LEFT, 90, -1);
	m_List.InsertColumn( 4, _T("Min Cstl Time"), LVCFMT_LEFT, 85, -1);
	m_List.InsertColumn( 5, _T("Avg Cstl Time"), LVCFMT_LEFT, 85, -1);
	m_List.InsertColumn( 6, _T("Min Impl Time"), LVCFMT_LEFT, 85, -1);
	m_List.InsertColumn( 7, _T("Avg Impl Time"), LVCFMT_LEFT, 85, -1);

	if(!m_pPlayerDB)
		return FALSE;

	m_pPlayerDB->Update();

	int nItem;
	CString str;
	int totalplays = 0, playcount, i;

	for(i = 0; i < m_pPlayerDB->GetCount(); i++)
	{
		playcount = m_pPlayerDB->GetAt(i)->PlayCount;
		if(!playcount)
			continue;

		nItem = m_List.InsertItem(i, m_pPlayerDB->GetAt(i)->NickNames[0]);

		int mostciv = 0;
		int index = 0;
		for(int j = 0;j < 19;j++)
		{
			if( mostciv < m_pPlayerDB->GetAt(i)->Civs[j])
			{
				mostciv = m_pPlayerDB->GetAt(i)->Civs[j];
				index = j;
			}
		}

		if( (index > 0) && (index < 19) )
		{
			str.Format(_T("%9s/%.f%%"), civ_name[index], 100*(float)m_pPlayerDB->GetAt(i)->Civs[index]/(float)m_pPlayerDB->GetAt(i)->PlayCount);
			m_List.SetItemText(nItem, 1, str);
		}
		else
		{
			str.Format(_T("N/A"));
			m_List.SetItemText(nItem, 1, str);
		}

		CTimeSpan avg, min;
		int	count;
		for(int j = 0; j < 3 ; j++)
		{
			switch(j)
			{
			case 0:
				min = m_pPlayerDB->GetAt(i)->MinFeudTime;
				count = m_pPlayerDB->GetAt(i)->FeudCount;
				if(count)
					avg = CTimeSpan(m_pPlayerDB->GetAt(i)->TotalFeudTime.GetTotalSeconds() / count);
				else
					avg = CTimeSpan(0);
				break;
			case 1:
				min = m_pPlayerDB->GetAt(i)->MinCstlTime;
				count = m_pPlayerDB->GetAt(i)->CstlCount;
				if(count)
					avg = CTimeSpan(m_pPlayerDB->GetAt(i)->TotalCstlTime.GetTotalSeconds() / count);
				else
					avg = CTimeSpan(0);
				break;
			case 2:
				min = m_pPlayerDB->GetAt(i)->MinImplTime;
				count = m_pPlayerDB->GetAt(i)->ImplCount;
				if(count)
					avg = CTimeSpan(m_pPlayerDB->GetAt(i)->TotalImplTime.GetTotalSeconds() / count);
				else
					avg = CTimeSpan(0);
				break;
			}
			if(min == MAX_TIMESPAN)
				str = _T("--:--:--");
			else
				if(min.GetHours() == 0)
					str.Format(_T("%d:%.2d"), min.GetMinutes(), min.GetSeconds());
				else
					str.Format(_T("%.2d:%.2d:%.2d"), min.GetHours(), min.GetMinutes(), min.GetSeconds());
			m_List.SetItemText(nItem, 2 * j + 2, str);
			if(avg == CTimeSpan(0))
				str = _T("--:--:--");
			else
				if(avg.GetHours() == 0)
					str.Format(_T("%d:%.2d"), avg.GetMinutes(), avg.GetSeconds());
				else
					str.Format(_T("%.2d:%.2d:%.2d"), avg.GetHours(), avg.GetMinutes(), avg.GetSeconds());
			m_List.SetItemText(nItem, 2 * j + 3, str);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
