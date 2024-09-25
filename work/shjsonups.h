// ============================================================================
// shjsonups.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.03.15
// ============================================================================

#ifndef _SHJSONUPS_H
#define _SHJSONUPS_H

// ================================================
// _/_/_/  class
// ================================================
class CShJsonUserPSize : public CShJson
{
public:

	CShJsonUserPSize(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName /*CShPrnLog FAR *pLog*/);
	virtual ~CShJsonUserPSize();

	BOOL reset();
	BOOL resetUs(long lIndex);
	BOOL resetEx(LONG index);

	BOOL WriteUPSData(long lIndex, LPUSERPAPERSIZEDATA pupsd);
	BOOL WriteUPSDataToUs(long lIndexUs, long lIndex, LPUSERPAPERSIZEDATA pupsd);
	BOOL WriteUPSCount(DWORD dwCount);
	BOOL WriteUPSCount(WCHAR FAR *pszBase, long lIndex, DWORD dwCount);

	BOOL ReadUPSData(long lIndex, LPUSERPAPERSIZEDATA pupsd);
	BOOL ReadUPSDataToUs(long lIndexUs, long lIndex, LPUSERPAPERSIZEDATA pupsd);
	DWORD ReadUPSCount();
	DWORD ReadUPSCount(WCHAR FAR *pszBase, long lIndex);

	virtual BOOL GetUPSDefData(LPUSERPAPERSIZEDATA pupsd = NULL, long lIndex = 0);
	virtual BOOL GetLPSDefData(LPUSERPAPERSIZEDATA pupsd = NULL);

protected:

	BOOL				m_blDelMulChk;
//	CShMultiChk FAR		*m_pmultichk;

	virtual BOOL CreateObj(long lFlag);
	virtual void DeleteObj(long lFlag);
// <S><A> 2022.05.17 sbc R.Tanaka
	virtual void SetFunctionsJsonPath(wchar_t* wzJsonPath);
// <E> 2022.05.17 sbc R.Tanaka

private:
	BOOL xReadUPSData(WCHAR *pszKeyPath,LPUSERPAPERSIZEDATA pmsd);
	BOOL xWriteUPSData(WCHAR *pszKeyPath,LPUSERPAPERSIZEDATA pmsd);
};

#endif	// #ifndef _SHJSONUPS_H
