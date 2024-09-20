// ============================================================================
// shJson.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.02.15
// ============================================================================

#include <precomp.h>
#include "shJson.h"


//#define PARENT_CLASSNAME 	CShObjHold
#define MY_CLASSNAME 		CShJson
CRegistryAPI regApi;
BOOL blDirectReg = TRUE;
//=============================================================================
// function
//      CShJson
//
// parameters
//      hInst               モジュールハンドル
//      pLog                CShPrnLog クラスアドレス
//
// return value
//      無し
//
// outline
//      CShJson クラスコンストラクタ
//=============================================================================
CShJson::CShJson(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName /*CShPrnLog FAR *pLog*/)
			//: PARENT_CLASSNAME(hInst, pszSvrPrnName, pLog)
{
	m_pszSvrPrnName = pszSvrPrnName;
	m_pszDeviceName = NULL;
	m_blJsonParsed = FALSE;
	SecureZeroMemory(m_wzJsonPath,MAX_PATH);
	m_blExistFolder = FALSE;
	m_pParent = this;
}


//=============================================================================
// function
//      ~CShJson
//
// parameters
//      無し
//
// return value
//      無し
//
// outline
//      CShJson クラスデストラクタ
//=============================================================================
CShJson::~CShJson()
{
	if (m_pszDeviceName != NULL)
		delete[] m_pszDeviceName;
}

//=============================================================================
// function
//      SetParent
//
// parameters
//      m_pParent            the parent pointer
//
// return value
//     
//
// outline
//      the parent pointer is cshjsonus,use the parent to read/write data
//		in order to operate the same json data
//=============================================================================
void CShJson::SetParent(CShJson* pParent)
{
	if(pParent != NULL)
		m_pParent = pParent;
}

//=============================================================================
// function
//      GetPDName
//
// parameters
//      なし
//
// return value
//		プリンタ名orドライバ名
//
// outline
//      MCFの設定により、ドライバ名またはプリンタ名を返す。
//=============================================================================
WCHAR* CShJson::GetPDName()
{
	BOOL blRet = FALSE;
	WCHAR *pRet = m_pszSvrPrnName;

	if (m_pszDeviceName != NULL)
	{
		pRet = m_pszDeviceName;
		goto EXIT;
	}
	else 
	{
		m_pszDeviceName = new WCHAR[SCUI_PRNNAME_MAX * 2];
	}

	if (m_pszDeviceName == NULL)
		goto EXIT;

	memset (m_pszDeviceName, 0,  (SCUI_PRNNAME_MAX * 2));

	// MCF チェック & Device Name Flag Check
	/*if ((CreateObj(scui_obj_ini_mcf) == TRUE) && 
	    ((*m_pmcf).GetLong(MCF_SEC_PRNOPTONS, MCF_KEY_PRNOPT_DRVNAME_BASE, 0) == 1))
	{
		blRet = GetPrnDrvName(m_pszSvrPrnName, m_pszDeviceName, SCUI_PRNNAME_MAX);
	}
	*/
	if (blRet == FALSE)
		wcscat_s(m_pszDeviceName, SCUI_PRNNAME_MAX, m_pszSvrPrnName);

	pRet = m_pszDeviceName;

EXIT:
	return pRet;
}

//=============================================================================
// function
//      WriteJsonStrData
//
// parameters
//      pszKeyPath          multibyte
//      pszEntry            multibyte
//      pData               string data //if blForceMB = FALSE, it is wchar; else it is multibyte
//      dwSize              pData size
//      blForceMB           強制MultiByteフラグ（データのみ）//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     write the json str data
//=============================================================================
BOOL CShJson::WriteJsonStrData(const WCHAR* pszKeyPath, const WCHAR* pszEntry, const WCHAR* pData,const DWORD& dwSize, BOOL blForceMB)
{
	BOOL bRet = FALSE;
	string strUtf8;
	
	if(pszKeyPath == NULL || pszEntry == NULL || pData == NULL || dwSize <= 0)
		goto EXIT;

	if(m_blJsonParsed == FALSE){
		ParseToJsonData();
	}

	if(blForceMB == FALSE)
	{
		//the pData is wide char, transfer the wide char to utf-8 
		strUtf8 = UnicodeToUTF8((wchar_t*)pData);
		bRet = SetKeyPathNodeValue(pszKeyPath,pszEntry,Json::Value(strUtf8));
	}
	else
	{
		//save the pData directly
		bRet = SetKeyPathNodeValue(pszKeyPath,pszEntry,Json::Value(pData));
	}

EXIT:
	return bRet;
}

//=============================================================================
// function
//      WriteJsonBinaryData
//
// parameters
//      pszKeyPath          multibyte
//      pszEntry            multibyte
//      pData               binary data 
//      dwSize              pData size
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     write the json binary data
//=============================================================================
BOOL CShJson::WriteJsonBinaryData(const WCHAR* pszKeyPath, const WCHAR* pszEntry,const WCHAR* pData, const DWORD& dwSize)
{
	BOOL bRet = FALSE;
	//string strHex;
	std::wstring strHex;
	std::string tempstr(strHex.begin(), strHex.end());
	if(pszKeyPath == NULL || pszEntry == NULL || pData == NULL || dwSize <= 0)
		goto EXIT;

	if(m_blJsonParsed == FALSE){
		ParseToJsonData();
	}

	strHex = BinarytoHexString((WCHAR *)pData, dwSize);

	bRet = SetKeyPathNodeValue(pszKeyPath,pszEntry,Json::Value(tempstr));

EXIT:
	return bRet;
}

//=============================================================================
// function
//      WriteJsonDWORDData
//
// parameters
//      pszKeyPath          multibyte
//      pszEntry            multibyte
//      dwData              DWORD data
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     write the json DWORD data
//=============================================================================
BOOL CShJson::WriteJsonDWORDData(const WCHAR* pszKeyPath,const WCHAR* pszEntry,const DWORD& dwData)
{
	BOOL bRet = FALSE;
	
	if(pszKeyPath == NULL || pszEntry == NULL)
		goto EXIT;

	if(m_blJsonParsed == FALSE){
		ParseToJsonData();
	}

	bRet = SetKeyPathNodeValue(pszKeyPath,pszEntry,Json::Value((Json::Value::UInt)dwData));

EXIT:
	return bRet;
}

//=============================================================================
// function
//      ReadJsonStrData
//
// parameters
//      pszKeyPath          multibyte
//      pszEntry            multibyte
//      pData               string data //if blForceMB = FALSE, it is wchar; else it is multibyte
//      dwSize              pData size
//      blForceMB           強制MultiByteフラグ（データのみ）//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     read the json str data
//=============================================================================
BOOL CShJson::ReadJsonStrData(const WCHAR* pszKeyPath,const WCHAR* pszEntry, WCHAR* pData, DWORD& dwSize, BOOL blForceMB)
{
	BOOL bRet = FALSE;
	string strUtf8;
	Json::Value nodeValue;
	
	if(pszKeyPath == NULL || pszEntry == NULL || pData == NULL || dwSize <= 0)
		goto EXIT;

	if(m_blJsonParsed == FALSE){
		ParseToJsonData();
	}

	bRet = GetKeyPathNodeValue(pszKeyPath,pszEntry,nodeValue);

	if(bRet == TRUE)
	{
		strUtf8 = nodeValue.asString();

		if(blForceMB == FALSE)
		{
			wstring wszUnicode = UTF8ToUnicode(strUtf8.c_str());
			dwSize = (wszUnicode.length() + 1) * sizeof(wchar_t);
			memcpy(pData,wszUnicode.c_str(),dwSize);
		}
		else
		{
			dwSize = (strUtf8.length() + 1) * sizeof(WCHAR);
			memcpy(pData,strUtf8.c_str(),dwSize);
		}
	}

EXIT:
	return bRet;
}

//=============================================================================
// function
//      ReadJsonBinaryData
//
// parameters
//      pszKeyPath          multibyte
//      pszEntry            multibyte
//      pData               binary data 
//      dwSize              pData size
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     read the json binary data
//=============================================================================
BOOL CShJson::ReadJsonBinaryData(const WCHAR* pszKeyPath,const WCHAR* pszEntry, WCHAR* pData, DWORD& dwSize)
{
	BOOL bRet = FALSE;

	Json::Value nodeValue;
	
	if(pszKeyPath == NULL || pszEntry == NULL || pData == NULL || dwSize <= 0)
		goto EXIT;

	if(m_blJsonParsed == FALSE){
		ParseToJsonData();
	}

	bRet = GetKeyPathNodeValue(pszKeyPath,pszEntry,nodeValue);

	if(bRet == TRUE)
	{
		string tempstr= nodeValue.asString();

		std::wstring strHex(tempstr.begin(), tempstr.end());

		bRet = HexStringtoBinary(strHex.c_str(),strHex.length(),(BYTE*)pData,dwSize);
	}

EXIT:
	return bRet;
}

//=============================================================================
// function
//      ReadJsonDWORDData
//
// parameters
//      pszKeyPath          multibyte
//      pszEntry            multibyte
//      dwData              DWORD data
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     read the json DWORD data
//=============================================================================
BOOL CShJson::ReadJsonDWORDData(const WCHAR* pszKeyPath,const WCHAR* pszEntry, DWORD& dwData)
{
	BOOL bRet = FALSE;
	Json::Value nodeValue;
	//Json::Value* nodeValue = new Json::Value();
	//std::unique_ptr<Json::Value> nodeValue = std::make_unique<Json::Value>();

	if(pszKeyPath == NULL || pszEntry == NULL)
		goto EXIT;

	if(m_blJsonParsed == FALSE){
		ParseToJsonData();
	}

	bRet = GetKeyPathNodeValue(pszKeyPath,pszEntry,nodeValue);

	if(bRet == TRUE)
	{
		dwData = nodeValue.asUInt();
	}

EXIT:
	return bRet;
}

//=============================================================================
// function
//      SetJsonPath
//
// parameters
//      pszDeviceName		
//
// return value
//      無し
//
// outline
//     construct the json file path
//=============================================================================
void CShJson::SetJsonPath(const WCHAR* pszDeviceName)
{
	wchar_t	wzLocalAppData[MAX_PATH] = {0};
	WCHAR	szDevName[SCUI_PRNNAME_MAX * 2] = {0};
	WCHAR	szCompanyName[MAX_PATH * 2] = {0};
	WCHAR	szDevNameHash[SCUI_HASH_LENGTH + 1] = {0};
	wchar_t	wzDevNameHash[SCUI_HASH_LENGTH + 1] = {0};	

	/*json temp if(	CreateObj(scui_obj_ini_rlv) == FALSE)
		return;*/

	wcscpy(szCompanyName, L"SHARP");

	if(pszDeviceName != NULL)
	{
		memcpy(szDevName,pszDeviceName,sizeof(szDevName));
	}
	else
	{
		memcpy(szDevName,m_pszSvrPrnName,sizeof(szDevName));
	}

	TransferNameToHash(szDevName, szDevNameHash);
	//::MultiByteToWideChar(SCUI_CP, SCUI_MB, szDevNameHash, -1, wzDevNameHash, SCUI_HASH_LENGTH);
	GetSharpLocalAppDataPath(wzLocalAppData, szDevNameHash, (wchar_t*)szCompanyName);

	::wcscpy_s(m_wzJsonPath, MAX_PATH, wzLocalAppData);

	if(PathIsDirectoryW(m_wzJsonPath))
		m_blExistFolder = TRUE;

	::wcscat_s(m_wzJsonPath,MAX_PATH, L"\\");

	//::wcscat_s(m_wzJsonPath,MAX_PATH, (wchar_t*)szDevName);
	::wcscat_s(m_wzJsonPath, MAX_PATH, L"printer_ui_");
	SetFunctionsJsonPath(m_wzJsonPath);
	::wcscat_s(m_wzJsonPath,MAX_PATH,JSON_EXTW);

}

//=============================================================================
// function
//      CreateDirectoryForJson
//
// parameters
//      pszDeviceName		
//
// return value
//      無し
//
// outline
//     Create a directory to save the json file
//=============================================================================
void CShJson::CreateDirectoryForJson(const WCHAR* pszDeviceName)
{
	WCHAR	szDevName[SCUI_PRNNAME_MAX * 2] = {0};
	wchar_t wzJsonPath[MAX_PATH] = {0};

	::wcscpy_s(wzJsonPath, MAX_PATH, m_wzJsonPath);
	*(wcsrchr(wzJsonPath, '\\')) = '\0';
	if(PathIsDirectoryW(wzJsonPath) != 0)
	{
		m_blExistFolder = TRUE;
		return;
	}

	if(pszDeviceName != NULL)
	{
		memcpy(szDevName,pszDeviceName,sizeof(szDevName));
	}
	else
	{
		memcpy(szDevName,m_pszSvrPrnName,sizeof(szDevName));
	}

	CreateSharpLocalAppDataDierctory(m_wzJsonPath);
	WritePrinerNameToFile(wzJsonPath, (const wchar_t*)pszDeviceName);
	m_blExistFolder = TRUE;

}


// <S><A> 2022.05.17 sbc R.Tanaka
//=============================================================================
// function
//      SetFunctionsJsonPath
//
// parameters
//      pszDeviceName		
//
// return value
//      無し
//
// outline
//     add functions name to json file path
//=============================================================================
void CShJson::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_NULL);
	return;
}
// <E> 2022.05.17 sbc R.Tanaka

// <S><C> 2022.05.23 sbc R.Tanaka
//=============================================================================
// function
//      Init
//
// parameters
//      none
//
// return value
//      void
//
// outline
//     Get and Set PrinterDevice name
//=============================================================================
void CShJson::Init()
{
	GetPDName();
	SetJsonPath(m_pszDeviceName);
}

//=============================================================================
// function
//      xParseToJsonData
//
// parameters
//      none
//
// return value
//      void
//
// outline
//     parse the json file
//=============================================================================
void CShJson::xParseToJsonData()
{
	Json::Reader reader;
	ifstream in(m_wzJsonPath, ios::binary);
 
	if (in.is_open())
	{
		reader.parse(in, m_root);
		in.close();
	}

	m_blJsonParsed = TRUE;
}
// <E> 2022.05.23 sbc R.Tanaka

// <S><A> 2022.08.30 sbc R.Tanaka
//=============================================================================
// function
//      ParseToJsonData
//
// parameters
//      none
//
// return value
//      void
//
// outline
//     parse the json file
//=============================================================================
void CShJson::ParseToJsonData()
{
	m_pParent->m_strKeyPath.clear();
	m_pParent->xParseToJsonData();
}
// <E> 2022.08.30 sbc R.Tanaka

//=============================================================================
// function
//      xWriteJsonDataToFile
//
// parameters
//      none
//
// return value
//      BOOL
//
// outline
//     write the json data to file
//=============================================================================
BOOL CShJson::xWriteJsonDataToFile()
{
	BOOL bRet = FALSE;
	Json::StyledWriter sw;
	ofstream os;

	if(m_blExistFolder == FALSE)
	{
		CreateDirectoryForJson(m_pszDeviceName);
	}

	os.open(m_wzJsonPath, std::ios::out);
	if (!os.is_open())
		goto EXIT;

	os << sw.write(m_root);
	os.close();

	bRet = TRUE;

EXIT:
	return bRet;
}

//=============================================================================
// function
//      ParseKeyPath
//
// parameters
//      pszKeyPath	multibyte key path
//		vec			vector to save the parsed path
// return value
//      void
//
// outline
//     parse the key path to vector
//=============================================================================
void CShJson::ParseKeyPath(const WCHAR* pszKeyPath,vector<string> &vecNodePath)
{
	string strKeyPath;
	string strSubPath;
	string::size_type tailPos;
	string::size_type headPos;
	string::size_type len = 0;

	std::string strKeypath = ConvertWCharToString(pszKeyPath);
	//strKeyPath = pszKeyPath;
	strKeyPath = strKeypath;
    len = strKeyPath.length();
	tailPos = 0;
	headPos = tailPos;

	while((tailPos = strKeyPath.find("\\",tailPos)) != string::npos)
	{
		strSubPath = strKeyPath.substr(headPos,tailPos-headPos);
		vecNodePath.push_back(strSubPath);
		tailPos++;
		headPos = tailPos;
	}

	if(len > headPos) //the last one
	{
		strSubPath = strKeyPath.substr(headPos,len-headPos);
		vecNodePath.push_back(strSubPath);
	}
}

//=============================================================================
// function
//      SetKeyPathNodeValue
//
// parameters
//      pszKeyPath		the key path
//      pszEntry		the entry key
//		nodeValue		the entry value
// return value
//      BOOL
//
// outline
//     set the key path node 
//=============================================================================
BOOL CShJson::SetKeyPathNodeValue(const WCHAR* pszKeyPath,const WCHAR* pszEntry,Json::Value& nodeValue)
{
	BOOL bRet = FALSE;
	vector<string> vecNodePath;
	int nVecNodePathSize = 0;
	int iNotExistNode = -1;
	std::string strKey = ConvertWCharToString(pszEntry);
	if(pszKeyPath == NULL || pszEntry == NULL)
		goto EXIT;

	if(m_strKeyPath.compare(pszKeyPath) != 0)
	{
		m_psubNode = &m_root;

		ParseKeyPath(pszKeyPath,vecNodePath);

		nVecNodePathSize = vecNodePath.size();
		if(nVecNodePathSize <= 0)
			goto EXIT;

		for(int i = 0; i < nVecNodePathSize; i++)
		{
			if(m_psubNode->isMember(vecNodePath[i]))
			{
				m_psubNode = &((*m_psubNode)[vecNodePath[i]]);
			}
			else
			{
				iNotExistNode = i;
				break;
			}
		}

		if(iNotExistNode > -1 && iNotExistNode < nVecNodePathSize)
		{
			for(int i = iNotExistNode; i <nVecNodePathSize ; i++)
			{
				Json::Value notExistNode;
				
				(*m_psubNode)[vecNodePath[i]] = notExistNode;
				m_psubNode = &((*m_psubNode)[vecNodePath[i]]);
			}
		}			
		m_strKeyPath = pszKeyPath;
	}
	
	//(*m_psubNode)[pszEntry] = nodeValue;
	(*m_psubNode)[strKey] = nodeValue;


	bRet = TRUE;

EXIT:
	return bRet;
}

//=============================================================================
// function
//      GetKeyPathNodeValue
//
// parameters
//      pszKeyPath		the key path
//      pszEntry		the entry key
//		nodeValue		the entry value
// return value
//      BOOL
//
// outline
//     get the key path node 
//=============================================================================
BOOL CShJson::GetKeyPathNodeValue(const WCHAR* pszKeyPath,const WCHAR* pszEntry,Json::Value& nodeValue)
{
	BOOL bRet = FALSE;
	vector<string> vecNodePath;
	int nVecNodePathSize = 0;
	std::string strKey = ConvertWCharToString(pszEntry);
	if(pszKeyPath == NULL || pszEntry == NULL)
		goto EXIT;

	if(m_strKeyPath.compare(pszKeyPath) != 0)
	{
		m_psubNode = &m_root;

		ParseKeyPath(pszKeyPath,vecNodePath);

		nVecNodePathSize = vecNodePath.size();
		if(nVecNodePathSize <= 0)
			goto EXIT;

		for(int i = 0; i < nVecNodePathSize; i++)
		{
			if(m_psubNode->isMember(vecNodePath[i]))
			{
				m_psubNode = &((*m_psubNode)[vecNodePath[i]]);
			}
			else //not find the key path
			{
				m_strKeyPath.clear();
				goto EXIT;
			}
		}
		m_strKeyPath = pszKeyPath;
	}
	
	//nodeValue = (*m_psubNode)[pszEntry];
	nodeValue = (*m_psubNode)[strKey];

	if(nodeValue.isNull() == false)
		bRet = TRUE;

EXIT:
	return bRet;
}

//=============================================================================
// function
//      DeleteJsonTreeData
//
// parameters
//      pszKeyPath       the key path(multibyte)
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      delete the key path
//=============================================================================
BOOL CShJson::DeleteJsonTreeData(const WCHAR* pszKeyPath)
{
	BOOL bRet = FALSE;
	vector<string> vecNodePath;
	int nVecNodePathSize = 0;
	Json::Value* psubNode = &m_root;
	
	if(pszKeyPath == NULL)
		goto EXIT;

	ParseKeyPath(pszKeyPath,vecNodePath);

	nVecNodePathSize = vecNodePath.size();
	if(nVecNodePathSize <= 0)
		goto EXIT;

	for(int i = 0; i < nVecNodePathSize; i++)
	{
		if(psubNode->isMember(vecNodePath[i]))
		{
			psubNode = &((*psubNode)[vecNodePath[i]]);
		}
		else
		{
			//there is not exist node, so it can't delete the key path
			goto EXIT;
		}
	}

	if(psubNode->isObject())
		psubNode->clear();

	bRet = TRUE;

EXIT:
	return bRet;
}

//=============================================================================
// function
//      GetChildItemsValue
//
// parameters
//      pszKeyPath		the key path:"printer_ui\\user_setting"
//      pszEntry		the entry key
//		vecValue		the vector of entry value
// return value
//      BOOL
//
// outline
//     As SearchUnusedID is slow, so add this function to improve the speed,get all of the us entry values(id/title ...)
//=============================================================================
BOOL CShJson::GetChildItemsValue(const WCHAR* pszKeyPath,const WCHAR* pszEntry,vector<Json::Value>& vecValue)
{
	BOOL bRet = FALSE;
	int nVecNodePathSize = 0;
	vector<string> vecNodePath;
	Json::Value* psubNode;
	Json::Value nodeValue;
	DWORD dwCount = 0;
	WCHAR FAR	*pszKey = NULL;

	if(pszKeyPath == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	
	psubNode = &m_root;

	ParseKeyPath(pszKeyPath,vecNodePath);

	nVecNodePathSize = vecNodePath.size();
	if(nVecNodePathSize <= 0)
		goto EXIT;

	for(int i = 0; i < nVecNodePathSize; i++)
	{
		if(psubNode->isMember(vecNodePath[i]))
		{
			psubNode = &((*psubNode)[vecNodePath[i]]);
		}
		else //not find the key path
		{
			goto EXIT;
		}
	}

	nodeValue = (*psubNode)[JSON_ENT_US_COUNT];
	dwCount = nodeValue.asInt();

	for(DWORD i = 0; i < dwCount; i++)
	{
		SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
		::wsprintf(pszKey, L"%ld", i);
		std::string strKey = ConvertWCharToString(pszKey);
		std::string strEntry = ConvertWCharToString(pszEntry);
		//nodeValue = (*psubNode)[pszKey][pszEntry];
		nodeValue = (*psubNode)[strKey][strEntry];
		vecValue.push_back(nodeValue);
	}

	bRet = TRUE;
	
EXIT:
	if(pszKey != NULL)
		delete[] pszKey;

	return bRet;
}

//=============================================================================
// function
//      DeleteChildItemData
//
// parameters
//      pszKeyPath       the key path:"printer_ui\\user_setting"
//		dwIndex			the fav item index
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      delete the fav item in key path, recalculate every items index
//=============================================================================
BOOL CShJson::DeleteChildItemData(const WCHAR* pszKeyPath, DWORD dwIndex)
{
	BOOL bRet = FALSE;
	vector<string> vecNodePath;
	int nVecNodePathSize = 0;
	Json::Value* psubNode = &m_root;
	Json::Value nodeValue;
	DWORD dwCount = 0;
	DWORD i = 0;
	WCHAR FAR	*pszKey1 = NULL;
	WCHAR FAR	*pszKey2 = NULL;
	std::string strKey11;
	//if(pszKeyPath == NULL || dwIndex < 0)
	if (pszKeyPath == NULL)
		goto EXIT;

	pszKey1 = new WCHAR[SCUI_REGKEYSIZE];
	pszKey2 = new WCHAR[SCUI_REGKEYSIZE];
	if (pszKey1 == NULL || pszKey2 == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey1, SCUI_REGKEYSIZE);
	SecureZeroMemory(pszKey2, SCUI_REGKEYSIZE);

	ParseKeyPath(pszKeyPath, vecNodePath);

	nVecNodePathSize = vecNodePath.size();
	if (nVecNodePathSize <= 0)
		goto EXIT;

	for (int i = 0; i < nVecNodePathSize; i++)
	{
		if (psubNode->isMember(vecNodePath[i]))
		{
			psubNode = &((*psubNode)[vecNodePath[i]]);
		}
		else
		{
			goto EXIT;
		}
	}

	/*	std::string strCountKey = ConvertWCharToString(JSON_ENT_US_COUNT);
		nodeValue = (*psubNode)[strCountKey];*/// this can also be done 
		//but for now i am making wide char decalaration to normal char
	nodeValue = (*psubNode)[JSON_ENT_US_COUNT];
	dwCount = nodeValue.asInt();

	if (dwCount <= 0)
		goto EXIT;

	for (i = dwIndex; i < dwCount - 1; i++)
	{
		SecureZeroMemory(pszKey1, SCUI_REGKEYSIZE);
		SecureZeroMemory(pszKey2, SCUI_REGKEYSIZE);
		::wsprintf(pszKey1, L"%ld", i);
		::wsprintf(pszKey2, L"%ld", i + 1);
		std::string strKey1 = ConvertWCharToString(pszKey1);
		std::string strKey2 = ConvertWCharToString(pszKey2);
		//(*psubNode)[pszKey1] = (*psubNode)[pszKey2];
		(*psubNode)[strKey1] = (*psubNode)[strKey2];

	}

	//delete the last one
	SecureZeroMemory(pszKey1, SCUI_REGKEYSIZE);
	::wsprintf(pszKey1, L"%ld", i);
	strKey11 = ConvertWCharToString(pszKey1);
	if (psubNode->isObject())

		//(*psubNode)[pszKey1].clear();
		(*psubNode)[strKey11].clear();

	(*psubNode)[JSON_ENT_US_COUNT] = Json::Value((Json::Value::UInt)(dwCount - 1));

	bRet = TRUE;

EXIT:
	if (pszKey1 != NULL)
		delete[] pszKey1;

	if (pszKey2 != NULL)
		delete[] pszKey2;

	return bRet;
}

//=============================================================================
// function
//      WriteJsonDataToFile
//
// parameters
//      none
//
// return value
//      BOOL
//
// outline
//     write the json data to file
//=============================================================================
BOOL CShJson::WriteJsonDataToFile()
{
	BOOL bRet = FALSE;
	if(m_pParent != NULL)
		bRet = m_pParent->xWriteJsonDataToFile();

	return bRet;
}

//=============================================================================
// function
//      CopyJsonTreeNode
//
// parameters
//      pszKeyPathDst       the dst key path
//      pszKeyPathSrc       the src key path
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     copy the json tree node from src to dst,it used for improve the speed of WriteDataJson/ReadDataJson in shjsonus
//=============================================================================
BOOL CShJson::CopyJsonTreeNode(const WCHAR* pszKeyPathDst, const WCHAR* pszKeyPathSrc)
{
	BOOL bRet = FALSE;
	vector<string> vecNodePathSrc,vecNodePathDst;
	int nVecNodePathSizeSrc = 0;
	int nVecNodePathSizeDst = 0;
	Json::Value* psubNodeSrc = &m_root;
	Json::Value* psubNodeDst = &m_root;
	int iNotExistNode = -1;
	
	if(pszKeyPathSrc == NULL || pszKeyPathDst == NULL)
		goto EXIT;

	ParseKeyPath(pszKeyPathSrc,vecNodePathSrc);
	ParseKeyPath(pszKeyPathDst,vecNodePathDst);

	nVecNodePathSizeSrc = vecNodePathSrc.size();
	nVecNodePathSizeDst = vecNodePathDst.size();
	if(nVecNodePathSizeSrc <= 0 || nVecNodePathSizeDst <= 0)
		goto EXIT;

	for(int i = 0; i < nVecNodePathSizeSrc; i++)
	{
		if(psubNodeSrc->isMember(vecNodePathSrc[i]))
		{
			psubNodeSrc = &((*psubNodeSrc)[vecNodePathSrc[i]]);
		}
		else
		{
			//there is not exist node, so it can't copy the key path
			goto EXIT;
		}
	}

	for(int i = 0; i < nVecNodePathSizeDst; i++)
	{
		if(psubNodeDst->isMember(vecNodePathDst[i]))
		{
			psubNodeDst = &((*psubNodeDst)[vecNodePathDst[i]]);
		}
		else
		{
			iNotExistNode = i;//there is not exist node, so need to create node
				break;
		}
	}

	if(iNotExistNode > -1 && iNotExistNode < nVecNodePathSizeDst)
	{
		for(int i = iNotExistNode; i <nVecNodePathSizeDst ; i++)
		{
			Json::Value notExistNode;

			(*psubNodeDst)[vecNodePathDst[i]] = notExistNode;
			psubNodeDst = &((*psubNodeDst)[vecNodePathDst[i]]);
		}
	}	

	*psubNodeDst = *psubNodeSrc;

	bRet = TRUE;

EXIT:
	return bRet;
}

//=============================================================================
// function
//      SelectTheCorrectFavItem
//
// parameters
//		pszNameHash: the selected fav item name hash
//      wIndex IN/OUT:  the selected fav item index	
//
// return value
//      none
//
// outline
//      compare index and name to select the correct fav item
//=============================================================================
void CShJson::SelectTheCorrectFavItemJson(WCHAR* pszNameHash, short& wIndex)
{
	WCHAR		szTitle[_MAX_FNAME * 2] = {0};
	WCHAR		szTitleHash[SCUI_HASH_LENGTH] = {0};
	DWORD		dwCount = 0;
	BOOL		bFind = FALSE;

	if(pszNameHash == NULL)
		goto EXIT;

	dwCount = ReadCount();
	if(dwCount < 1 )
		goto EXIT;

	if(wIndex > 0)
	{
		if(((unsigned short)wIndex < dwCount) && GetTitle(szTitle, sizeof(szTitle), wIndex) == TRUE)
		{
			TransferNameToHash(szTitle, szTitleHash);

			//if the index's hash name is same to the hash name, then the index is the correct item
			if(memcmp(pszNameHash,szTitleHash,SCUI_HASH_LENGTH) == 0)
				goto EXIT;
		}

		//Otherwise, enum the items in reg, find the same name item
		for(DWORD i = 1; i < dwCount; i++)
		{
			SecureZeroMemory(szTitle, sizeof(szTitle));
			SecureZeroMemory(szTitleHash, sizeof(szTitleHash));
			if(GetTitle(szTitle, sizeof(szTitle), i) == TRUE)
			{
				TransferNameToHash(szTitle, szTitleHash);

				if(memcmp(pszNameHash,szTitleHash,SCUI_HASH_LENGTH) == 0)
				{
					wIndex = (short)i;
					bFind = TRUE;
					break;
				}
			}
		}

		if(bFind == FALSE)
		{
			wIndex = 0;
		}
	}

EXIT:

	return;
}

// <S><A> 2022.04.13 Jelly for cr5 function2
//=============================================================================
// function
//      WriteFavItemsFromJSONToHKLM
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//when OnShare,
//write all of the user setting fav items from JSON to HKLM. Format: 200520101000,DWORD1,AAA,DWORD2,[DATA],200521101000,DWORD1,BBB,DWORD2,[DATA], 
//DWORD1 is for the buffer of title. DWORD2 is for the size of [DATA], not add the ',' in the buffer actually.
//=============================================================================
BOOL CShJson::WriteFavItemsFromJSONToHKLM(WCHAR *pszSvrPrnName)
{
	BOOL	blRet = FALSE;
	WCHAR	szFavItemTimeW[ SCUI_FILEDATETIME_STRLEN * 2] = {0};
	WCHAR	szFavItemTitleW[_MAX_FNAME * 2] = {0};
	WCHAR	szTime[ SCUI_FILEDATETIME_STRLEN ] = {0};
	DWORD	dwCount = 0;
	DWORD	dwSize = 0;
	DWORD	dwAllSize = 0;
	BYTE*	pShareFavData= NULL;
	BYTE*	pAllFavData= NULL;
	BYTE*	pAllFavDataTmp = NULL;
	DWORD	dwTitleLength = 0;
	DWORD	dwTitleBuffer = 0;

	if(pszSvrPrnName == NULL)
		goto EXIT;

	dwCount = ReadCount();
		
	//calculate the total size
	for(DWORD i = 1; i < dwCount; i++)
	{
		if(GetTitle(szFavItemTitleW, sizeof(szFavItemTitleW), i) == FALSE)
			goto EXIT;
		dwTitleLength = wcslen(szFavItemTitleW);
		dwTitleBuffer = IsUnicodeOS() ? dwTitleLength*sizeof(WCHAR) : dwTitleLength;

		dwSize = ReadFavDataFromJSON(pszSvrPrnName,NULL,i);
		if(dwSize > 0)
		{
			dwAllSize += sizeof(szFavItemTimeW) + sizeof(dwTitleBuffer) + dwTitleBuffer + sizeof(dwSize) + dwSize;
		}
	}

	if(dwAllSize > 0)
	{
		pAllFavData = new BYTE[dwAllSize];
	
		if(pAllFavData == NULL)
			goto EXIT;

		SecureZeroMemory(pAllFavData, dwAllSize);
		pAllFavDataTmp = pAllFavData;
	}


	//prepare the total data
	for(DWORD i = 1; i < dwCount; i++)
	{
		SecureZeroMemory(szFavItemTimeW, sizeof(szFavItemTimeW));
		SecureZeroMemory(szFavItemTitleW, sizeof(szFavItemTitleW));
		
		if(GetTime(szFavItemTimeW, sizeof(szFavItemTimeW), i) == FALSE)
			goto EXIT;
		memcpy(pAllFavDataTmp, szFavItemTimeW, sizeof(szFavItemTimeW));
		pAllFavDataTmp += sizeof(szFavItemTimeW);
		
		if(GetTitle(szFavItemTitleW, sizeof(szFavItemTitleW), i) == FALSE)
			goto EXIT;
		dwTitleLength = wcslen(szFavItemTitleW);
		dwTitleBuffer = IsUnicodeOS() ? dwTitleLength*sizeof(WCHAR) : dwTitleLength;

		memcpy(pAllFavDataTmp,&dwTitleBuffer, sizeof(dwTitleBuffer));
		pAllFavDataTmp += sizeof(dwTitleBuffer);
		memcpy(pAllFavDataTmp,szFavItemTitleW, dwTitleBuffer);
		pAllFavDataTmp += dwTitleBuffer;

		dwSize = ReadFavDataFromJSON(pszSvrPrnName,NULL,i);
		if(dwSize > 0)
		{
			pShareFavData = new BYTE[dwSize];
			if(pShareFavData != NULL)
			{
				memcpy(pAllFavDataTmp,&dwSize, sizeof(dwSize));
				pAllFavDataTmp += sizeof(dwSize);

				SecureZeroMemory(pShareFavData, dwSize);
				ReadFavDataFromJSON(pszSvrPrnName,pShareFavData,i);

				memcpy(pAllFavDataTmp,pShareFavData, dwSize);
				pAllFavDataTmp += dwSize;

				delete []pShareFavData;
				pShareFavData = NULL;
			}
		}
	}

	//write the total fav data to HKLM
	WriteShareData(pszSvrPrnName, pAllFavData, dwAllSize);

	//write the share_daytime_func in JSON and HKLM.
	GetLocalTimeStr(szTime);
	WriteShareDayTimeFunc(pszSvrPrnName,szTime,TRUE);

	blRet = TRUE;

EXIT:
	if(pAllFavData != NULL)
	{
		delete []pAllFavData;
		pAllFavData = NULL;
	}

	return blRet;
}

//=============================================================================
// function
//      DealWithFavItemsInHKLM
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
// return value
//      成功 : TRUE		the time stamp in json and hklm are diff, deal with it
//      失敗 : FALSE	the time stamp in json and hklm are same, don't deal with it
//
// outline
//When client UI is openned, call this function, it is about user setting
//=============================================================================
BOOL CShJson::DealWithFavItemsInHKLM(WCHAR *pszSvrPrnName)
{
	BOOL			res = FALSE;
	BOOL			bShare = FALSE;
	DWORD			dwReadSize = 0;
	WCHAR			szTextJSONW[REG_ENT_SHARE_KEYSIZEW] = {0};
	WCHAR			szTextHKLMW[REG_ENT_SHARE_KEYSIZEW] = {0};
	WCHAR			szTimeJSONW[ SCUI_FILEDATETIME_STRLEN * 2] = {0};
	WCHAR			szTimeHKLMW[ SCUI_FILEDATETIME_STRLEN * 2] = {0};
	BYTE*			pAllFavReadData= NULL;

	if(pszSvrPrnName == NULL)
		goto EXIT;

	ReadShareDayTimeFuncFromJSON(pszSvrPrnName,szTextJSONW,sizeof(szTextJSONW));
	ReadShareDayTimeFuncFromHKLM(pszSvrPrnName,szTextHKLMW,sizeof(szTextHKLMW));

	if(wcslen(szTextHKLMW) > 0)
	{
		bShare = GetValidFlag(szTextHKLMW);
		
		//if szTextJSONW is not same to szTextHKLMW
		if(sh_strcmp(szTextJSONW, szTextHKLMW) != 0)
		{	
			//Get the valid flag from szTextHKLMW. 
			if(bShare == FALSE)
			{
				GetShareDayTimeStamp(szTextHKLMW,szTimeHKLMW);

				//if it is invalid and the time is not same, delete the shara fav items in JSON
				if(wcslen(szTextJSONW) > 0)
				{
					GetShareDayTimeStamp(szTextJSONW,szTimeJSONW);

					if(sh_strcmp(szTimeJSONW, szTimeHKLMW) != 0)
					{
						DeleteShareDataInJSON(pszSvrPrnName);
					}
				}

			}
			else //else it is valid
			{
				DeleteShareDataInJSON(pszSvrPrnName); //delete all of the shared data in JSON at first.

				dwReadSize = ReadShareData(pszSvrPrnName,NULL);
				if(dwReadSize > 0)
				{
					pAllFavReadData = new BYTE[dwReadSize];
					if(pAllFavReadData != NULL)
					{
						SecureZeroMemory(pAllFavReadData, dwReadSize);
						ReadShareData(pszSvrPrnName,pAllFavReadData);
						TransferFavDataFromHKLMToJSON(pszSvrPrnName,pAllFavReadData,dwReadSize);
					}
				}
			}

			//copy the szTextHKLMW to JSON
			WriteShareDayTimeFuncToJSON(pszSvrPrnName,szTextHKLMW, sizeof(szTextHKLMW) );

			res = TRUE;
		}
	}


EXIT:
	if(pAllFavReadData != NULL)
	{
		delete []pAllFavReadData;
		pAllFavReadData = NULL;
	}

	return res;
}

//=============================================================================
// function
//      TransferFavDataFromHKLMToJSON
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      pShareFavData       the fav data in HKLM. Format: 200520101000,DWORD1,AAA,DWORD2,[DATA],200521101000,DWORD1,BBB,DWORD2,[DATA], 
//							DWORD1 is for the buffer of title. DWORD2 is for the size of [DATA], the ',' are not in the buffer actually.
//		dwSize				the size of pShareFavData
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      transfer the fav data from HKLM to JSON.
//=============================================================================
BOOL CShJson::TransferFavDataFromHKLMToJSON(WCHAR  *pszSvrPrnName, BYTE* pShareFavData,DWORD dwSize)
{
	BOOL	bRet = FALSE;
	DWORD	dwItemDataSize = 0; //one fav item data size, only the [DATA] size
	DWORD	dwTransSize = 0; //the transfered data size
	DWORD	dwCount = 0;
	BOOL	bSameFav= FALSE;
	DWORD	dwIndex = 0;
	DWORD	dwTitleBuffer = 0;

	WCHAR	szFavItemTimeHKLMW[ SCUI_FILEDATETIME_STRLEN * 2] = {0};
	WCHAR	szFavItemTitleHKLMW[_MAX_FNAME * 2] = {0};
	WCHAR	szFavItemTimeJSONW[ SCUI_FILEDATETIME_STRLEN * 2] = {0};
	WCHAR	szFavItemTitleJSONW[_MAX_FNAME * 2] = {0};
	
	BYTE *pszHeadTmp = NULL;	

	if(pszSvrPrnName == NULL || pShareFavData == NULL || dwSize == 0)
		goto EXIT;

	pszHeadTmp = pShareFavData;

	//enum fav items in HKLM 
	while(dwTransSize < dwSize)
	{
		bSameFav= FALSE;	
		SecureZeroMemory(szFavItemTimeHKLMW,sizeof(szFavItemTimeHKLMW));
		memcpy(szFavItemTimeHKLMW,pszHeadTmp, sizeof(szFavItemTimeHKLMW));
		pszHeadTmp += sizeof(szFavItemTimeHKLMW);

		memcpy(&dwTitleBuffer,pszHeadTmp,sizeof(dwTitleBuffer));
		pszHeadTmp += sizeof(dwTitleBuffer);

		SecureZeroMemory(szFavItemTitleHKLMW,sizeof(szFavItemTitleHKLMW));
		memcpy(szFavItemTitleHKLMW,pszHeadTmp,dwTitleBuffer);
		pszHeadTmp += dwTitleBuffer;

		dwItemDataSize = 0;
		memcpy(&dwItemDataSize,pszHeadTmp,sizeof(dwItemDataSize));
		pszHeadTmp += sizeof(dwItemDataSize);

		//enum fav items in JSON
		dwCount = ReadCount();
		for(DWORD i = 1; i < dwCount; i++)
		{
			SecureZeroMemory(szFavItemTimeJSONW,sizeof(szFavItemTimeJSONW));
			SecureZeroMemory(szFavItemTitleJSONW,sizeof(szFavItemTitleJSONW));
			
			if(GetTitle(szFavItemTitleJSONW, sizeof(szFavItemTitleJSONW), i) == FALSE)
				continue;

			GetTime(szFavItemTimeJSONW, sizeof(szFavItemTimeJSONW), i);

			//if there is the same name fav
			if(sh_strcmp(szFavItemTitleJSONW, szFavItemTitleHKLMW) == 0)
			{
				bSameFav = TRUE;
				dwIndex = i;
				break;
			}

		}

		if(bSameFav == TRUE)
		{
			if(sh_strcmp(szFavItemTimeJSONW, szFavItemTimeHKLMW) < 0) //the HKLM time is newer, overwrite the HKLM to JSON
			{
				TransferOneFavDataFromHKLMToJSON(pszSvrPrnName,dwIndex,pszHeadTmp, dwItemDataSize,szFavItemTimeHKLMW,szFavItemTitleHKLMW,TRUE);
			}
		}
		else		//else new the HKLM fav to JSON fav
		{
			TransferOneFavDataFromHKLMToJSON(pszSvrPrnName,dwCount,pszHeadTmp, dwItemDataSize,szFavItemTimeHKLMW,szFavItemTitleHKLMW,FALSE);
		}

		pszHeadTmp += dwItemDataSize;

		dwTransSize += sizeof(szFavItemTimeHKLMW) + sizeof(dwTitleBuffer) + dwTitleBuffer + sizeof(dwItemDataSize) + dwItemDataSize;
	}

	bRet = TRUE;

EXIT:
	return bRet;
}

//=============================================================================
// function
//      TransferOneFavDataFromHKLMToJSON
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//		lIndex				the item index in JSON
//      pShareFavData       the one fav data in HKLM. it is the [DATA] in 20210811103941,AAA,DWORD,[DATA],
//		dwSize				the size of pShareFavData
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      transfer the one fav data from HKLM to JSON.
//=============================================================================
BOOL CShJson::TransferOneFavDataFromHKLMToJSON(WCHAR  *pszSvrPrnName, DWORD dwIndex, BYTE* pShareFavData,DWORD dwSize, WCHAR* pszTime, WCHAR* pszTitle,BOOL bOverWrite)
{
	return FALSE;
}
//=============================================================================
// function
//      WriteShareDayTimeFunc
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      pszDateTime         YYYYMMDDHHmmSS 形式の日時文字列
//      pszFunName         func name. eg:user_setting
//		bValid             TRUE: 2; FALSE: 1
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     write share_daytime_func_user_setting to JSON and HKLM: eg: 2,20210811103941,user_setting
//=============================================================================
BOOL CShJson::WriteShareDayTimeFunc(WCHAR* pszSvrPrnName, WCHAR* pszDayTime, BOOL bValid)
{
	return FALSE;
}
//=============================================================================
// function
//      WriteShareDayTimeFunc
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      pszDateTime         YYYYMMDDHHmmSS 形式の日時文字列
//      pszFunName         func name. eg:user_setting
//		bValid             TRUE: 2; FALSE: 1
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     write share_daytime_func_user_setting to JSON and HKLM: eg: 2,20210811103941,user_setting
//=============================================================================
BOOL CShJson::WriteShareDayTimeFunc(WCHAR* pszSvrPrnName, WCHAR* pszJsonBase, WCHAR* pszDayTime, WCHAR* pszFunName, BOOL bValid)
{
	BOOL			blRet = FALSE;

	//WCHAR			szTextA[REG_ENT_SHARE_KEYSIZEA] = {0};
	WCHAR			szTextW[REG_ENT_SHARE_KEYSIZEW] = {0};
	//WCHAR			szFuncKeyA[REG_ENT_SHARE_KEYSIZEA] = {0};
	WCHAR			szFuncKeyW[REG_ENT_SHARE_KEYSIZEW] = {0};

	// ================================================
	// _/_/_/  パラメータチェック
	// ================================================
	if(pszSvrPrnName == NULL || pszDayTime == NULL || m_pParent == NULL)
		goto EXIT;

	if(bValid == TRUE && pszFunName == NULL)
		goto EXIT;

	if(bValid == TRUE)
	{
		::wsprintf(szTextW,L"2,%s,%s",pszDayTime,pszFunName);
	}
	else
	{
		::wsprintf(szTextW,L"1,%s",pszDayTime);
	}

	//conv_wchar(szTextW, szTextA,sizeof(szTextW));

	::wsprintf(szFuncKeyW,L"%s_%s",REG_ENT_SHARE_DAYTIMEFUNC, pszFunName);

	//write to JSON
	blRet = m_pParent->WriteJsonStrData(pszJsonBase, szFuncKeyW, szTextW, sizeof(szTextW));
	if(blRet == FALSE)
		goto EXIT;

	//write to HKLM
//	conv_wchar(szFuncKeyW,szFuncKeyA, sizeof(szFuncKeyW));
	if(regApi.sh_SetPrinterData(pszSvrPrnName, szFuncKeyW, REG_SZ, (LPBYTE)szTextW, (DWORD)wcslen(szTextW) * 2, blDirectReg) != ERROR_SUCCESS )
		goto EXIT;


	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      WriteShareDayTimeFuncToJSON
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      pszText				the total string: eg: 2,20210811103941,user_setting
//		dwSize				the size of pszText
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     write share_daytime_func_user_setting to JSON
//=============================================================================
BOOL CShJson::WriteShareDayTimeFuncToJSON(WCHAR* pszSvrPrnName, WCHAR* pszText, DWORD dwSize)
{
	return		FALSE;
}
//=============================================================================
// function
//      WriteShareDayTimeFuncToJSON
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      pszFunName         func name. eg:user_setting
//      pszText				the total string: eg: 2,20210811103941,user_setting
//		dwSize				the size of pszText
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     write share_daytime_func_user_setting to JSON
//=============================================================================
BOOL CShJson::WriteShareDayTimeFuncToJSON(WCHAR* pszSvrPrnName, WCHAR* pszJsonBase, WCHAR* pszFunName, WCHAR* pszText, DWORD dwSize)
{
	BOOL		blRet = FALSE;

	WCHAR		szFuncKeyA[REG_ENT_SHARE_KEYSIZEA] = {0};

	// ================================================
	// _/_/_/  パラメータチェック
	// ================================================
	if(pszSvrPrnName == NULL || pszText == NULL || pszFunName == NULL || m_pParent == NULL)
		goto EXIT;

	::wsprintf(szFuncKeyA,L"%s_%s",REG_ENT_SHARE_DAYTIMEFUNC, pszFunName);

	//write to JSON
	blRet = m_pParent->WriteJsonStrData(pszJsonBase, szFuncKeyA,pszText,dwSize);
	if(blRet == FALSE)
		goto EXIT;

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      ReadShareDayTimeFuncFromJSON
//
// parameters
//      pszSvrPrnName      サーバー名付きプリンタ名
//		pszText            the value
//		dwTextSize		   the size of pszText
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     Read share_daytime_func_user_setting from JSON: eg: 2,20210811103941,user_setting
//=============================================================================
BOOL CShJson::ReadShareDayTimeFuncFromJSON(WCHAR* pszSvrPrnName, WCHAR* pszText,DWORD dwTextSize)
{
	return FALSE;
}

//=============================================================================
// function
//      ReadShareDayTimeFuncFromJSON
//
// parameters
//      pszSvrPrnName      サーバー名付きプリンタ名
//      pszFunName         func name. eg:user_setting
//		pszText            the value
//		dwTextSize		   the size of pszText
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     Read share_daytime_func_user_setting from JSON: eg: 2,20210811103941,user_setting
//=============================================================================
BOOL CShJson::ReadShareDayTimeFuncFromJSON(WCHAR* pszSvrPrnName, WCHAR* pszJsonBase, WCHAR* pszFunName, WCHAR* pszText,DWORD dwTextSize)
{
	BOOL		blRet = FALSE;
	WCHAR		szFuncKeyA[REG_ENT_SHARE_KEYSIZEA] = {0};

	// ================================================
	// _/_/_/  パラメータチェック
	// ================================================
	if(pszSvrPrnName == NULL || pszFunName == NULL || pszText == NULL || m_pParent == NULL)
		goto EXIT;

	::wsprintf(szFuncKeyA,L"%s_%s",REG_ENT_SHARE_DAYTIMEFUNC, pszFunName);

	m_pParent->ReadJsonStrData(pszJsonBase, szFuncKeyA,pszText,dwTextSize);

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      ReadShareDayTimeFuncFromHKLM
//
// parameters
//      pszSvrPrnName      サーバー名付きプリンタ名
//		pszText            the value
//		dwTextSize		   the size of pszText
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     Read share_daytime_func_user_setting from HKLM: eg: 2,20210811103941,user_setting
//=============================================================================
BOOL CShJson::ReadShareDayTimeFuncFromHKLM(WCHAR  *pszSvrPrnName, WCHAR  *pszText,DWORD dwTextSize)
{
	return FALSE;
}

//=============================================================================
// function
//      ReadShareDayTimeFuncFromHKLM
//
// parameters
//      pszSvrPrnName      サーバー名付きプリンタ名
//      pszFunName         func name. eg:user_setting
//		pszText            the value
//		dwTextSize		   the size of pszText
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//     Read share_daytime_func_user_setting from HKLM: eg: 2,20210811103941,user_setting
//=============================================================================
BOOL CShJson::ReadShareDayTimeFuncFromHKLM(WCHAR  *pszSvrPrnName, WCHAR  *pszFunName, WCHAR  *pszText,DWORD dwTextSize)
{
	BOOL			blRet = FALSE;
	//WCHAR*			szFuncKeyA[REG_ENT_SHARE_KEYSIZEA] = {0};
	WCHAR			szFuncKeyW[REG_ENT_SHARE_KEYSIZEW] = {0};
	DWORD			dwType = REG_SZ;
	DWORD			dwNeeded = 0;
	// ================================================
	// _/_/_/  パラメータチェック
	// ================================================
	if(pszSvrPrnName == NULL || pszFunName == NULL || pszText == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  読み込み
	// ================================================
	::wsprintf(szFuncKeyW,L"%s_%s",REG_ENT_SHARE_DAYTIMEFUNC, pszFunName);

	//conv_wchar(szFuncKeyW, szFuncKeyW, sizeof(szFuncKeyW));

	if(regApi.sh_GetPrinterData(pszSvrPrnName, szFuncKeyW,  &dwType, (LPBYTE)pszText, dwTextSize, &dwNeeded) != ERROR_SUCCESS)
		goto EXIT;


	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      GetValidFlag
//
// parameters
//		pszText            the value eg: 2,20210811103941,user_setting
// return value
//      valid : TRUE
//      invalid : FALSE
//
// outline
//     Read the valid flag from pszText
//=============================================================================
BOOL CShJson::GetValidFlag(WCHAR  *pszText)
{
	BOOL bValid = FALSE;
	WCHAR szValue[4] = {0};
	if(pszText == NULL)
		goto EXIT;

	if(IsUnicodeOS())
	{
		memcpy(szValue, pszText, 2);
		if(sh_strcmp(szValue, (WCHAR*)L"2") == 0)
			bValid = TRUE;
	}
	else
	{
		memcpy(szValue, pszText, 1);
		if(sh_strcmp(szValue, L"2") == 0)
			bValid = TRUE;
	}
EXIT:
	return bValid;
}

//=============================================================================
// function
//      GetShareDayTimeStamp
//
// parameters
//		pszText          IN		the value eg: 2,20210811103941,user_setting
//		pszTime          OUT	200521111000
// return value
//      success : TRUE
//      fail : FALSE
//
// outline
//     Read the valid flag from pszText
//=============================================================================
BOOL CShJson::GetShareDayTimeStamp(WCHAR  *pszText, WCHAR  *pszTime)
{
	BOOL bRet = FALSE;
	WCHAR  *pszTextTmp = NULL;
	if(pszText == NULL || pszTime == NULL)
		goto EXIT;

	pszTextTmp = pszText;
/*	if(IsUnicodeOS())
	{
	*/	pszTextTmp += 4;
		memcpy(pszTime, pszTextTmp, (SCUI_FILEDATETIME_STRLEN - 1) *2);
	//}
	//else
	//{
	//	pszTextTmp += 2;
	//	memcpy(pszTime, pszTextTmp, SCUI_FILEDATETIME_STRLEN - 1);
	//}

	bRet = TRUE;

EXIT:
	return bRet;
}

//=============================================================================
// function
//      WriteShareData
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      pShareFav           pShareFav data
//      dwSize				the size of pShareFav
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      Write the pShareFav to HKLM's PrinterDriverData
//=============================================================================
BOOL CShJson::WriteShareData(WCHAR  *pszSvrPrnName, BYTE* pShareFav, DWORD dwSize)
{
	return FALSE;
}

//=============================================================================
// function
//      WriteShareData
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      pShareFav           pShareFav data
//      dwSize				the size of pShareFav
//      pszFunName         func name. eg: "user_setting"
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      Write the pShareFav to HKLM's PrinterDriverData
//=============================================================================
BOOL CShJson::WriteShareData(WCHAR  *pszSvrPrnName, WCHAR  *pszFunName, BYTE* pShareFav, DWORD dwSize)
{
	BOOL		bRet = FALSE;

	WCHAR		szKeyA[REG_ENT_SHARE_KEYSIZEA] = {0};
	WCHAR		szKeyW[REG_ENT_SHARE_KEYSIZEW] = {0};
	WCHAR		szSizeKeyA[REG_ENT_SHARE_KEYSIZEA] = {0};
	WCHAR		szSizeKeyW[REG_ENT_SHARE_KEYSIZEW] = {0};

	if(pszSvrPrnName == NULL || pszFunName == NULL)
		goto EXIT;
	
	::wsprintf(szKeyA,L"%s_%s",REG_ENT_SHARE_DAYTIME,pszFunName);
	::wsprintf(szSizeKeyA,L"%s_%s",szKeyA,REG_ENT_SHARE_SIZE);

	/*convert_wchar(szKeyW, szKeyA, sizeof(szKeyW));
	convert_wchar(szSizeKeyW,szSizeKeyA, sizeof(szSizeKeyW));*/
	
	if(regApi.sh_SetPrinterData(pszSvrPrnName, szSizeKeyW, REG_DWORD, (LPBYTE)&dwSize, sizeof(DWORD), blDirectReg) != ERROR_SUCCESS)
		goto EXIT;

	if(pShareFav != NULL)
	{
		if(regApi.sh_SetPrinterData(pszSvrPrnName, szKeyW, REG_BINARY, pShareFav, dwSize, blDirectReg) != ERROR_SUCCESS)
			goto EXIT;
	}

	bRet = TRUE;

EXIT:

	return bRet;
}

//=============================================================================
// function
//      ReadShareData
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      pShareFav           read to the pShareFav
//
// return value
//      成功 : buffer size
//      失敗 : 0
//
// outline
//      Read the share fav data specified by pszFunName
//=============================================================================
DWORD CShJson::ReadShareData(WCHAR  *pszSvrPrnName, BYTE* pShareFav)
{
	return 0;
}

//=============================================================================
// function
//      ReadShareData
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      pszFunName         func name. eg: "user_setting"
//      pShareFav           read to the pShareFav
//
// return value
//      成功 : buffer size
//      失敗 : 0
//
// outline
//      Read the share fav data specified by pszFunName from HKLM
//=============================================================================
DWORD CShJson::ReadShareData(WCHAR  *pszSvrPrnName, WCHAR  *pszFunName, BYTE* pShareFav)
{
	DWORD		dwRet = 0;


	WCHAR		szKeyW[REG_ENT_SHARE_KEYSIZEW] = {0};
	WCHAR		szSizeKeyW[REG_ENT_SHARE_KEYSIZEW] = {0};

	DWORD		dwSize = 0;
	DWORD		dwNeeded = 0;
	DWORD		dwType = REG_DWORD;

	if(pszSvrPrnName == NULL || pszFunName == NULL)
		goto EXIT;

	::wsprintf(szKeyW,L"%s_%s",REG_ENT_SHARE_DAYTIME,pszFunName);
	::wsprintf(szSizeKeyW,L"%s_%s", szKeyW,REG_ENT_SHARE_SIZE);

	/*convert_wchar(szKeyW, szKeyA, sizeof(szKeyW));
	convert_wchar(szSizeKeyW,szSizeKeyA, sizeof(szSizeKeyW));*/
	//conv_wchar(szKeyW, szKeyA, sizeof(szKeyW));
	//conv_wchar(szSizeKeyW, szSizeKeyA, sizeof(szSizeKeyW));
	//sh_GetPrinterData(
	
	if(regApi.sh_GetPrinterData(pszSvrPrnName, szSizeKeyW,  &dwType, (LPBYTE)&dwSize, sizeof(dwSize), &dwNeeded) != ERROR_SUCCESS)
		goto EXIT;

	if(pShareFav == NULL)
	{
		dwRet = dwSize;
		goto EXIT;
	}
	dwType = REG_BINARY;
	if(regApi.sh_GetPrinterData(pszSvrPrnName, szKeyW,  &dwType, pShareFav, dwSize, &dwNeeded) != ERROR_SUCCESS)
		goto EXIT;

	dwRet = dwSize;

EXIT:
	return dwRet;
}

//=============================================================================
// function
//      ReadFavDataFromJSON
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      lIndex              fav list index
//      pShareFav           the buffer to save the fav data
//
// return value
//      成功 : fav data buffer size
//      失敗 : 0
//
// outline
//      Read the fav data specified by lIndex
//=============================================================================
DWORD CShJson::ReadFavDataFromJSON(WCHAR  *pszSvrPrnName, BYTE* pShareFavData,long lIndex)
{
	return 0;
}

//=============================================================================
// function
//      DeleteShareDataInJSON
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      Delete all of the share datas in JSON
//=============================================================================
BOOL CShJson::DeleteShareDataInJSON(WCHAR  *pszSvrPrnName)
{
	BOOL bRet = FALSE;
	DWORD dwCount = 0;

	if(pszSvrPrnName == NULL)
		goto EXIT;

	dwCount = ReadCount();
	if(dwCount <= 0)
		goto EXIT;

	for(DWORD i = dwCount -1; i > 0; i--)
	{
		if(GetSelfFlag(i) == FALSE)
		{
			DeleteData((short)i);
		}
	}

	bRet = TRUE;
EXIT:
	return bRet;
}

//=============================================================================
// function
//      ReadCount
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//
// return value
//      0
//
// outline
//     virtual function, used for cr5 func2 fav
//=============================================================================
DWORD CShJson::ReadCount()
{
	return 0;
}

//=============================================================================
// function
//      GetTitle
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      lIndex              情報を取得するインデックス
//      pszTitle            タイトルを受け取るバッファ
//      dwBufSize           pszTitleサイズ
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      virtual function, used for cr5 func2 fav
//=============================================================================
BOOL CShJson::GetTitle(WCHAR FAR *pszTitle, DWORD dwBufSize, long lIndex)
{
	return FALSE;
}

//=============================================================================
// function
//      GetTime
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      lIndex              情報を取得するインデックス
//      pszTime            タイトルを受け取るバッファ
//      dwBufSize           pszTimeサイズ
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      virtual function, used for cr5 func2 fav
//=============================================================================
BOOL CShJson::GetTime(WCHAR  *pszTime, DWORD dwBufSize, long lIndex)
{
	return FALSE;
}

//=============================================================================
// function
//      GetSelfFlag
//
// parameters
//      pszSvrPrnName       サーバー名付きプリンタ名
//      lIndex              読み込むUserSettingインデックス
//
// return value
//       TRUE: is local fav item
//       FALE: is the shared fav item
//
// outline
//      virtual function, used for cr5 func2 fav
//=============================================================================
BOOL CShJson::GetSelfFlag(long lIndex)
{
	return FALSE;
}

//=============================================================================
// function
//      DeleteData
//
// parameters
//      wIndex              削除するUserSettingインデックス
//
// return value
//      なし
//
// outline
//      virtual function, used for cr5 func2 fav
//=============================================================================
void CShJson::DeleteData(short wIndex)
{
	return;
}
// <E>2022.04.13 Jelly
std::string CShJson::ConvertWCharToString(const wchar_t* wStr)
{
	//std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	//return conv.to_bytes(wStr);
	if (wStr == nullptr)
		return "";  // Handle null input safely

	// Get the required buffer size for the conversion
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wStr, -1, nullptr, 0, nullptr, nullptr);

	if (bufferSize == 0)
	{
		// Handle conversion error (WideCharToMultiByte failed)
		return "";
	}

	// Create a string with the required buffer size
	std::string str(bufferSize - 1, '\0');

	// Perform the actual conversion
	WideCharToMultiByte(CP_UTF8, 0, wStr, -1, &str[0], bufferSize, nullptr, nullptr);

	return str;
}