// ============================================================================
// shjsonms.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.03.11
// ============================================================================

#ifndef _SHJSONMS_H
#define _SHJSONMS_H

// ================================================
// _/_/_/  class
// ================================================
class CShJsonMS : public CShJson
{
public:

	CShJsonMS(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName/*, CShPrnLog FAR *pLog*/);
	virtual ~CShJsonMS();

	BOOL reset();
	BOOL resetEx();
	BOOL resetExToUs(long lIndex);

	BOOL WriteMSData(long lIndex, LPMARGINSHIFTDATA pmsd);
	BOOL WriteMSDataToUs(long lIndexUs, long lIndex, LPMARGINSHIFTDATA pmsd);
	BOOL WriteMSCount(DWORD dwCount);
	BOOL WriteMSCount(WCHAR FAR *pszBase, long lIndex, DWORD dwCount);

	BOOL ReadMSData(long lIndex, LPMARGINSHIFTDATA pmsd);
	BOOL ReadMSDataToUs(long lIndexUs, long lIndex, LPMARGINSHIFTDATA pmsd);
	DWORD ReadMSCount();
	DWORD ReadMSCount(WCHAR FAR *pszBase, long lIndex);

	DWORD GetCountIni();

	virtual BOOL GetMSDefData(long lIndex, LPMARGINSHIFTDATA pmsd = NULL);

	virtual BOOL Clear();
	long GetLongArray(long *plValue = NULL, long lCount = 0);
	long InitLongArray(void);
	long *GetLongArrayData(void);
	
protected:
	LPMARGINSHIFTDATA	m_pmsd;
	long				m_lIndex;
	long				m_lCount;
	BOOL				m_blWriteFlg;

	BOOL SetMSData(long lIndex, LPMARGINSHIFTDATA pmsd);
	BOOL SetMSCount(DWORD dwCount);
// <S><A> 2022.05.17 sbc R.Tanaka
	virtual void SetFunctionsJsonPath(wchar_t* wzJsonPath);
// <E> 2022.05.17 sbc R.Tanaka

	long FAR			*m_plData;

private:
	BOOL xReadMSData(WCHAR *pszKeyPath,LPMARGINSHIFTDATA pmsd);
	BOOL xWriteMSData(WCHAR *pszKeyPath,LPMARGINSHIFTDATA pmsd);
};

#endif	// #ifndef _SHJSONMS_H
