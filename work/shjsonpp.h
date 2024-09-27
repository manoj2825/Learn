// ============================================================================
// shjsonpp.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.3.17
// ============================================================================

#ifndef _SHJSONPP_H
#define _SHJSONPP_H


// ================================================
// _/_/_/  class
// ================================================
class CShJsonPP : public CShJson
{
public:

	CShJsonPP(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName/*, CShPrnLog FAR *pLog*/);
	virtual ~CShJsonPP();

	BOOL reset();
	BOOL resetToUs(long lIndex);

	BOOL WritePPData(LPPRINTPOSITION pppd);
	BOOL WritePPDataToUs(long lIndexUs, LPPRINTPOSITION pppd);
	BOOL WritePPCount(DWORD dwCount);
	BOOL WritePPCount(WCHAR FAR *pszBase, long lIndex, DWORD dwCount);

	BOOL ReadPPData(LPPRINTPOSITION pppd);
	BOOL ReadPPDataToUs(long lIndexUs, LPPRINTPOSITION pppd);
	DWORD ReadPPCount();
	DWORD ReadPPCount(WCHAR FAR *pszBase, long lIndex);

	DWORD GetCountIni();

	virtual BOOL GetPPDefData(LPPRINTPOSITION pppd = NULL);

	virtual BOOL Clear();
	long GetLongArray(long *plValue = NULL, long lCount = 0);
	long *GetLongArrayData(void);
	long InitLongArray(void);

protected:

	LPPRINTPOSITION	m_pPrintPositionData;
	DWORD			m_dwCount;
	BOOL			m_blWriteFlg;

	BOOL ReadPPDataBase(LPPRINTPOSITION pppd);

	BOOL SetPPData(LPPRINTPOSITION pppd);
	BOOL SetPPCount(DWORD dwCount);
// <S><A> 2022.05.17 sbc R.Tanaka
	virtual void SetFunctionsJsonPath(wchar_t* wzJsonPath);
// <E> 2022.05.17 sbc R.Tanaka
	long FAR		*m_plData;

private:
	BOOL xReadPPData(WCHAR *pszKeyPath,LPPRINTPOSITION pppd);
	BOOL xWritePPData(WCHAR *pszKeyPath,LPPRINTPOSITION pppd);
};

#endif	// #ifndef _SHJSONPP_H
