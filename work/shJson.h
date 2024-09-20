// ============================================================================
// shJson.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.02.15
// ============================================================================

#ifndef _SHJSON_H
#define _SHJSON_H

//#include "json/json.h"
//#pragma pack(push, 2)
#pragma pack(2)
#include "json/json.h"
//#pragma pack(pop)
#include "sh_api.h"
#include "jsonkey.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
//#include "shreg.h"
#include "RegistryAPI.h"
using namespace std;
// ================================================
// _/_/_/  class
// ================================================

class CShJson //: public CShObjHold
{
public:
	CShJson(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName);
	virtual ~CShJson(void);

	void SetParent(CShJson* pParent);
	void Init();

	WCHAR* GetPDName();

	BOOL WriteJsonStrData(const WCHAR* pszKeyPath,const WCHAR* pszEntry,const WCHAR* pData, const DWORD& dwSize, BOOL blForceMB = FALSE);
	BOOL WriteJsonBinaryData(const WCHAR* pszKeyPath,const WCHAR* pszEntry,const WCHAR* pData,const DWORD& dwSize);//transfer binary data to hex string,then save
	BOOL WriteJsonDWORDData(const WCHAR* pszKeyPath,const WCHAR* pszEntry,const DWORD& dwData);

	BOOL ReadJsonStrData(const WCHAR* pszKeyPath,const WCHAR* pszEntry, WCHAR* pData, DWORD& dwSize, BOOL blForceMB = FALSE);
	BOOL ReadJsonBinaryData(const WCHAR* pszKeyPath,const WCHAR* pszEntry, WCHAR* pData, DWORD& dwSize);//read the hex string data, transfer to binary data
	BOOL ReadJsonDWORDData(const WCHAR* pszKeyPath,const WCHAR* pszEntry, DWORD& dwData);

	BOOL WriteJsonDataToFile();
	BOOL DeleteJsonTreeData(const WCHAR* pszKeyPath);

	BOOL GetChildItemsValue(const WCHAR* pszKeyPath,const WCHAR* pszEntry,vector<Json::Value>& vecValue);
	BOOL DeleteChildItemData(const WCHAR* pszKeyPath, DWORD dwIndex);

	virtual void SelectTheCorrectFavItemJson(WCHAR* pszName, short& wIndex);
	
// <S><A> 2022.04.13 Jelly for cr5 function2
	virtual BOOL WriteFavItemsFromJSONToHKLM(WCHAR *pszSvrPrnName);
	virtual BOOL DealWithFavItemsInHKLM(WCHAR *pszSvrPrnName);
	virtual BOOL WriteShareDayTimeFunc(WCHAR *pszSvrPrnName, WCHAR* pszDayTime, BOOL bValid);
	virtual BOOL WriteShareDayTimeFuncToJSON(WCHAR *pszSvrPrnName, WCHAR *pszText, DWORD dwSize);
	virtual BOOL ReadShareDayTimeFuncFromJSON(WCHAR *pszSvrPrnName, WCHAR *pszText, DWORD dwTextSize);
	virtual BOOL ReadShareDayTimeFuncFromHKLM(WCHAR *pszSvrPrnName, WCHAR *pszText, DWORD dwTextSize);
	
	virtual	DWORD ReadCount();
	virtual BOOL GetTitle(WCHAR *pszTitle, DWORD dwBufSize, long lIndex);
	virtual BOOL GetTime(WCHAR  *pszTime, DWORD dwBufSize, long lIndex);
	virtual BOOL GetSelfFlag(long lIndex);
	virtual void DeleteData(short wIndex);

	BOOL GetValidFlag(WCHAR *pszText);
	BOOL GetShareDayTimeStamp(WCHAR *pszText, WCHAR *pszTime);
	WCHAR FAR		*m_pszSvrPrnName;
// <S><C> 2022.08.30 sbc R.Tanaka
	void ParseToJsonData();
// <E> 2022.08.30 sbc R.Tanaka
	string ConvertWCharToString(const wchar_t* wStr);


public:
	virtual DWORD ReadShareData(WCHAR  *pszSvrPrnName,BYTE* pShareFav);
	virtual BOOL WriteShareData(WCHAR  *pszSvrPrnName,BYTE* pShareFav, DWORD dwSize);
	virtual DWORD ReadFavDataFromJSON(WCHAR *pszSvrPrnName, BYTE* pShareFavData,long lIndex);
	virtual BOOL TransferOneFavDataFromHKLMToJSON(WCHAR *pszSvrPrnName, DWORD dwIndex, BYTE* pShareFavData,DWORD dwSize, WCHAR *pszTime, WCHAR * pszTitle,BOOL bOverWrite);

	BOOL WriteShareDayTimeFunc(WCHAR *pszSvrPrnName, WCHAR* pszRegBase, WCHAR *pszDayTime, WCHAR *pszFunName, BOOL bValid);
	BOOL WriteShareDayTimeFuncToJSON(WCHAR *pszSvrPrnName, WCHAR* pszRegBase, WCHAR *pszFunName, WCHAR *pszText, DWORD dwSize);
	BOOL ReadShareDayTimeFuncFromJSON(WCHAR *pszSvrPrnName, WCHAR* pszRegBase, WCHAR *pszFunName, WCHAR *pszText, DWORD dwTextSize);
	BOOL ReadShareDayTimeFuncFromHKLM(WCHAR *pszSvrPrnName, WCHAR *pszFunName, WCHAR *pszText, DWORD dwTextSize);
	DWORD ReadShareData(WCHAR  *pszSvrPrnName, WCHAR  *pszFunName, BYTE* pShareFav);
	BOOL WriteShareData(WCHAR  *pszSvrPrnName, WCHAR  *pszFunName, BYTE* pShareFav, DWORD dwSize);
	BOOL DeleteShareDataInJSON(WCHAR *pszSvrPrnName);
	BOOL TransferFavDataFromHKLMToJSON(WCHAR *pszSvrPrnName, BYTE* pShareFavData,DWORD dwSize);
// <E>2022.04.13 Jelly

	BOOL CopyJsonTreeNode(const WCHAR* pszKeyPathDst, const WCHAR* pszKeyPathSrc);
	CShJson*	m_pParent;

// <S><A> 2022.05.17 sbc R.Tanaka
	virtual void SetFunctionsJsonPath(wchar_t* wzJsonPath);
// <E> 2022.05.17 sbc R.Tanaka

// <S><A> 2022.05.23 sbc R.Tanaka
	BOOL m_blJsonParsed;
// <E> 2022.05.23 sbc R.Tanaka

public:
	BOOL xWriteJsonDataToFile();
	void SetJsonPath(const WCHAR* pszDeviceName);
// <S><A> 2022.06.13 sbc R.Tanaka
	void CreateDirectoryForJson(const WCHAR* pszDeviceName);
// <E> 2022.06.13 sbc R.Tanaka
	void ParseKeyPath(const WCHAR* pszKeyPath,vector<string> &vecNodePath);
	BOOL SetKeyPathNodeValue(const WCHAR* pszKeyPath,const WCHAR* pszEntry,Json::Value& nodeValue);
	BOOL GetKeyPathNodeValue(const WCHAR* pszKeyPath,const WCHAR* pszEntry,Json::Value& nodeValue);

// <S><A> 2022.08.30 sbc R.Tanaka
	void xParseToJsonData();
// <E> 2022.08.30 sbc R.Tanaka

	wchar_t m_wzJsonPath[MAX_PATH];
	Json::Value m_root;
	
	Json::Value* m_psubNode;//record the lowest child node	
	wstring m_strKeyPath;

	WCHAR*	m_pszDeviceName;
	

	BOOL m_blExistFolder;

};

#endif	// #ifndef _SHJSON_H
