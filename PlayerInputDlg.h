#pragma once


// CPlayerInputDlg dialog

class CPlayerInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlayerInputDlg)

public:
	CPlayerInputDlg(CString name = _T(""), UINT rating = INIT_RATING, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPlayerInputDlg();

// Dialog Data
	enum { IDD = IDD_PLAYERINPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog(void);
	afx_msg void OnDeltaposSpinrating(NMHDR *pNMHDR, LRESULT *pResult);

public:
	UINT m_iRating;
	CString m_Name;

private:
	const	static UINT	MIN_RATING = 1000, MAX_RATING = 2000, INIT_RATING = DEF_RATING;
	afx_msg void OnBnClickedOk();
};
