///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：Ado.h
// 创建者：yuanzy[从Carlos Antollini的程序修改而来]
// 创建时间：2006-3-17
// 内容描述：日志文件操作类头文件
///////////////////////////////////////////////////////////////////////////////////////////
#ifndef _ADO_H_
#define _ADO_H_

#pragma once
#include <atlbase.h>
#include "assert.h"
#include <math.h>
#include <time.h>
#include "iostream"
#include "string"
#include "stdio.h"
#include "stdarg.h"

/*#pragma warning (disable: 4146)
// CG : In order to use this code against a different version of ADO, the appropriate
// ADO library needs to be used in the #import statement
#import "msado15.dll" rename_namespace("ADOCG") rename("EOF", "EndOfFile")
using namespace ADOCG;
#pragma warning (default: 4146)*/

#if defined(WIN32) || defined(__WIN32__)//win32 os
	#include "COMUTIL.H"
	#include "windows.h"
	#import "../DB/thirdlib/mssqlserver/msado15.dll" rename_namespace("ADOCG") rename( "EOF", "EndOfFile" )
	using namespace ADOCG;
#endif 

#include "icrsint.h"
#include "../Log/AppLog.h"
using namespace std;

class CADOCommand;

struct CADOFieldInfo
{
	char m_strName[30]; 
	short m_nType;
	long m_lSize; 
	long m_lDefinedSize;
	long m_lAttributes;
	short m_nOrdinalPosition;
	bool m_bRequired;   
	bool m_bAllowZeroLength; 
	long m_lCollatingOrder;  
};

std::string IntToStr(int nVal);

std::string LongToStr(long lVal);

std::string ULongToStr(unsigned long ulVal);

std::string DblToStr(double dblVal, int ndigits = 20);

std::string DblToStr(float fltVal);

std::string DateTimeToStr(time_t t);


class CADODatabase
{
public:
	CADODatabase()
	{
		::CoInitialize(NULL);			
		m_pConnection = NULL;
		m_strConnection = "";
		m_strLastError = "";
		m_dwLastError = 0;
		HRESULT hr = NULL;
		hr = m_pConnection.CreateInstance(__uuidof(Connection));
		if (!SUCCEEDED(hr)) 
			return ;
		m_nRecordsAffected = 0;
	}
	
	~CADODatabase()
	{
		Close();
		m_pConnection.Release();
		m_pConnection = NULL;
		m_strConnection = "";
		m_strLastError = "";
		m_dwLastError = 0;
		::CoUninitialize();
	}
	
	bool Open(LPCTSTR lpstrConnection = "", LPCTSTR lpstrUserID = "", LPCTSTR lpstrPassword = "");
	_ConnectionPtr GetActiveConnection() 
		{return m_pConnection;};
	bool Execute(LPCTSTR lpstrExec);
	int GetRecordsAffected()
		{return m_nRecordsAffected;};
	DWORD GetRecordCount(_RecordsetPtr m_pRs);
	long BeginTransaction() 
		{return m_pConnection->BeginTrans();};
	long CommitTransaction() 
		{return m_pConnection->CommitTrans();};
	long RollbackTransaction() 
		{return m_pConnection->RollbackTrans();};
	bool IsOpen();
	void Close();
	void SetConnectionString(LPCTSTR lpstrConnection)
		{m_strConnection = lpstrConnection;};
    std::string GetConnectionString()
		{return m_strConnection;};
    std::string GetLastErrorString() 
		{return m_strLastError;};
	DWORD GetLastError()
		{return m_dwLastError;};
protected:
	void dump_com_error(_com_error &e);

public:
	_ConnectionPtr m_pConnection;

protected:
    std::string m_strConnection;
    std::string m_strLastError;
	DWORD m_dwLastError;
	int m_nRecordsAffected;
};

class CADORecordset
{
public:
	bool Clone(CADORecordset& pRs);
	enum cadoOpenEnum
	{
		openUnknown = 0,
		openQuery = 1,
		openTable = 2,
		openStoredProc = 3
	};

	enum cadoEditEnum
	{
		dbEditNone = 0,
		dbEditNew = 1,
		dbEdit = 2
	};
	
	enum cadoPositionEnum
	{
	
		positionUnknown = -1,
		positionBOF = -2,
		positionEOF = -3
	};
	
	enum cadoSearchEnum
	{	
		searchForward = 1,
		searchBackward = -1
	};

	enum cadoDataType
	{
		typeEmpty = adEmpty,
		typeTinyInt = adTinyInt,
		typeSmallInt = adSmallInt,
		typeInteger = adInteger,
		typeBigInt = adBigInt,
		typeUnsignedTinyInt = adUnsignedTinyInt,
		typeUnsignedSmallInt = adUnsignedSmallInt,
		typeUnsignedInt = adUnsignedInt,
		typeUnsignedBigInt = adUnsignedBigInt,
		typeSingle = adSingle,
		typeDouble = adDouble,
		typeCurrency = adCurrency,
		typeDecimal = adDecimal,
		typeNumeric = adNumeric,
		typeBoolean = adBoolean,
		typeError = adError,
		typeUserDefined = adUserDefined,
		typeVariant = adVariant,
		typeIDispatch = adIDispatch,
		typeIUnknown = adIUnknown,
		typeGUID = adGUID,
		typeDate = adDate,
		typeDBDate = adDBDate,
		typeDBTime = adDBTime,
		typeDBTimeStamp = adDBTimeStamp,
		typeBSTR = adBSTR,
		typeChar = adChar,
		typeVarChar = adVarChar,
		typeLongVarChar = adLongVarChar,
		typeWChar = adWChar,
		typeVarWChar = adVarWChar,
		typeLongVarWChar = adLongVarWChar,
		typeBinary = adBinary,
		typeVarBinary = adVarBinary,
		typeLongVarBinary = adLongVarBinary,
		typeChapter = adChapter,
		typeFileTime = adFileTime,
		typePropVariant = adPropVariant,
		typeVarNumeric = adVarNumeric,
		typeArray = adVariant
	};

	
	bool SetFieldValue(int nIndex, int nValue);
	bool SetFieldValue(LPCTSTR lpFieldName, int nValue);
	bool SetFieldValue(int nIndex, long lValue);
	bool SetFieldValue(LPCTSTR lpFieldName, long lValue);
	bool SetFieldValue(int nIndex, unsigned long lValue);
	bool SetFieldValue(LPCTSTR lpFieldName, unsigned long lValue);
	bool SetFieldValue(int nIndex, double dblValue);
	bool SetFieldValue(LPCTSTR lpFieldName, double dblValue);
    bool SetFieldValue(int nIndex, std::string strValue);
    bool SetFieldValue(LPCTSTR lpFieldName, std::string strValue);
	//bool SetFieldValue(int nIndex, COleDateTime time);
	//bool SetFieldValue(LPCTSTR lpFieldName, COleDateTime time);
	bool SetFieldValue(int nIndex, bool bValue);
	bool SetFieldValue(LPCTSTR lpFieldName, bool bValue);
	//bool SetFieldValue(int nIndex, COleCurrency cyValue);
	//bool SetFieldValue(LPCTSTR lpFieldName, COleCurrency cyValue);
	bool SetFieldValue(int nIndex, _variant_t vtValue);
	bool SetFieldValue(LPCTSTR lpFieldName, _variant_t vtValue);

	bool SetFieldEmpty(int nIndex);
	bool SetFieldEmpty(LPCTSTR lpFieldName);


	void CancelUpdate();
	bool Update();
	void Edit();
	bool AddNew();
	bool AddNew(CADORecordBinding &pAdoRecordBinding);

	bool Find(LPCTSTR lpFind, int nSearchDirection = CADORecordset::searchForward);
	bool FindFirst(LPCTSTR lpFind);
	bool FindNext();

	CADORecordset()
	{
		m_pRecordset = NULL;
		m_pCmd = NULL;
		m_strQuery = "";
		m_strLastError = "";
		m_dwLastError = 0;
		m_pRecBinding = NULL;
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pCmd.CreateInstance(__uuidof(Command));
		m_nEditStatus = CADORecordset::dbEditNone;
		m_nSearchDirection = CADORecordset::searchForward;
	}

	CADORecordset(CADODatabase* pAdoDatabase);

	~CADORecordset()
	{
		Close();
		if(m_pRecordset)
			m_pRecordset.Release();
		if(m_pCmd)
			m_pCmd.Release();
		m_pRecordset = NULL;
		m_pCmd = NULL;
		m_pRecBinding = NULL;
		m_strQuery = "";
		m_strLastError = "";
		m_dwLastError = 0;
		m_nEditStatus = dbEditNone;
	}

    std::string GetQuery() 
		{return m_strQuery;};
	void SetQuery(LPCSTR strQuery) 
		{m_strQuery = strQuery;};
	bool RecordBinding(CADORecordBinding &pAdoRecordBinding);
	DWORD GetRecordCount();
	bool IsOpen();
	void Close();
	bool Open(_ConnectionPtr mpdb, LPCTSTR lpstrExec = "", int nOption = CADORecordset::openUnknown);
	bool Open(LPCTSTR lpstrExec = "", int nOption = CADORecordset::openUnknown);
	long GetFieldCount()
		{return m_pRecordset->Fields->GetCount();};
	bool GetFieldValue(LPCTSTR lpFieldName, int& nValue);
	bool GetFieldValue(int nIndex, int& nValue);
	bool GetFieldValue(LPCTSTR lpFieldName, long& lValue);
	bool GetFieldValue(int nIndex, long& lValue);
	bool GetFieldValue(LPCTSTR lpFieldName, unsigned long& ulValue);
	bool GetFieldValue(int nIndex, unsigned long& ulValue);
	bool GetFieldValue(LPCTSTR lpFieldName, double& dbValue);
	bool GetFieldValue(int nIndex, double& dbValue);
    bool GetFieldValue(LPCTSTR lpFieldName, std::string& strValue, std::string strDateFormat = "");
    bool GetFieldValue(int nIndex, std::string& strValue, std::string strDateFormat = "");
	//bool GetFieldValue(LPCTSTR lpFieldName, COleDateTime& time);
	//bool GetFieldValue(int nIndex, COleDateTime& time);
	bool GetFieldValue(int nIndex, bool& bValue);
	bool GetFieldValue(LPCTSTR lpFieldName, bool& bValue);
	//bool GetFieldValue(int nIndex, COleCurrency& cyValue);
	//bool GetFieldValue(LPCTSTR lpFieldName, COleCurrency& cyValue);
	bool GetFieldValue(int nIndex, _variant_t& vtValue);
	bool GetFieldValue(LPCTSTR lpFieldName, _variant_t& vtValue);
	
	bool IsFieldNull(LPCTSTR lpFieldName);
	bool IsFieldNull(int nIndex);
	bool IsFieldEmpty(LPCTSTR lpFieldName);
	bool IsFieldEmpty(int nIndex);	
	bool IsEof()
		{return m_pRecordset->EndOfFile == VARIANT_TRUE;};
	bool IsEOF()
		{return m_pRecordset->EndOfFile == VARIANT_TRUE;};
	bool IsBof()
		{return m_pRecordset->BOF == VARIANT_TRUE;};
	bool IsBOF()
		{return m_pRecordset->BOF == VARIANT_TRUE;};
	void MoveFirst() 
		{m_pRecordset->MoveFirst();};
	void MoveNext() 
		{m_pRecordset->MoveNext();};
	void MovePrevious() 
		{m_pRecordset->MovePrevious();};
	void MoveLast() 
		{m_pRecordset->MoveLast();};
	long GetAbsolutePage()
		{return m_pRecordset->GetAbsolutePage();};
	void SetAbsolutePage(int nPage)
		{m_pRecordset->PutAbsolutePage((enum PositionEnum)nPage);};
	long GetPageCount()
		{return m_pRecordset->GetPageCount();};
	long GetPageSize()
		{return m_pRecordset->GetPageSize();};
	void SetPageSize(int nSize)
		{m_pRecordset->PutPageSize(nSize);};
	long GetAbsolutePosition()
		{return m_pRecordset->GetAbsolutePosition();};
	void SetAbsolutePosition(int nPosition)
		{m_pRecordset->PutAbsolutePosition((enum PositionEnum)nPosition);};
	bool GetFieldInfo(LPCTSTR lpFieldName, CADOFieldInfo* fldInfo);
	bool GetFieldInfo(int nIndex, CADOFieldInfo* fldInfo);
	bool AppendChunk(LPCTSTR lpFieldName, LPVOID lpData, UINT nBytes);
	bool AppendChunk(int nIndex, LPVOID lpData, UINT nBytes);

    bool GetChunk(LPCTSTR lpFieldName, std::string& strValue);
    bool GetChunk(int nIndex, std::string& strValue);
	
	bool GetChunk(LPCTSTR lpFieldName, LPVOID pData);
	bool GetChunk(int nIndex, LPVOID pData);

    std::string GetString(LPCTSTR lpCols, LPCTSTR lpRows, LPCTSTR lpNull, long numRows = 0);
    std::string GetLastErrorString() 
		{return m_strLastError;};
	DWORD GetLastError()
		{return m_dwLastError;};
	void GetBookmark()
		{m_varBookmark = m_pRecordset->Bookmark;};
	bool SetBookmark();
	bool Delete();
	bool IsConnectionOpen()
		{return m_pConnection->GetState() != adStateClosed;};
	_RecordsetPtr GetRecordset()
		{return m_pRecordset;};
	_ConnectionPtr GetActiveConnection() 
		{return m_pConnection;};

	bool SetFilter(LPCTSTR strFilter);
	bool SetSort(LPCTSTR lpstrCriteria);
	bool SaveAsXML(LPCTSTR lpstrXMLFile);
	bool OpenXML(LPCTSTR lpstrXMLFile);
	bool Execute(CADOCommand* pCommand);

public:
	_RecordsetPtr m_pRecordset;
	_CommandPtr m_pCmd;
	
protected:
	_ConnectionPtr m_pConnection;
	int m_nSearchDirection;
    std::string m_strFind;
	_variant_t m_varBookFind;
	_variant_t m_varBookmark;
	int m_nEditStatus;
    std::string m_strLastError;
	DWORD m_dwLastError;
	void dump_com_error(_com_error &e);
	IADORecordBinding *m_pRecBinding;
    std::string m_strQuery;

protected:
	bool PutFieldValue(LPCTSTR lpFieldName, _variant_t vtFld);
	bool PutFieldValue(_variant_t vtIndex, _variant_t vtFld);
	bool GetFieldInfo(FieldPtr pField, CADOFieldInfo* fldInfo);
    bool GetChunk(FieldPtr pField, std::string& strValue);
	bool GetChunk(FieldPtr pField, LPVOID lpData);
	bool AppendChunk(FieldPtr pField, LPVOID lpData, UINT nBytes);
		
};

class CADOParameter
{
public:

	enum cadoParameterDirection
	{
		paramUnknown = adParamUnknown,
		paramInput = adParamInput,
		paramOutput = adParamOutput,
		paramInputOutput = adParamInputOutput,
		paramReturnValue = adParamReturnValue 
	};

    CADOParameter(int nType, long lSize = 0, int nDirection = paramInput, std::string strName = "");
	
	~CADOParameter()
	{
		m_pParameter.Release();
		m_pParameter = NULL;
		m_strName = "";
	}

	bool SetValue(int nValue);
	bool SetValue(long lValue);
	bool SetValue(double dbValue);
    bool SetValue(std::string strValue);
	//bool SetValue(COleDateTime time);
	bool SetValue(_variant_t vtValue);
	bool GetValue(int& nValue);
	bool GetValue(long& lValue);
	bool GetValue(double& dbValue);
    //bool GetValue(std::string& strValue, std::string strDateFormat = "");
	//bool GetValue(COleDateTime& time);
	bool GetValue(_variant_t& vtValue);
	void SetPrecision(int nPrecision)
		{m_pParameter->PutPrecision(nPrecision);};
	void SetScale(int nScale)
		{m_pParameter->PutNumericScale(nScale);};

    std::string SetName(std::string strName)
		{m_strName = strName;};
    std::string GetName()
		{return m_strName;};
	int GetType()
		{return m_nType;};
	_ParameterPtr GetParameter()
		{return m_pParameter;};

protected:
	void dump_com_error(_com_error &e);
	
protected:
	_ParameterPtr m_pParameter;
    std::string m_strName;
	int m_nType;
    std::string m_strLastError;
	DWORD m_dwLastError;
};


class CADOCommand
{
public:
	enum cadoCommandType
	{
		typeCmdText = adCmdText,
		typeCmdTable = adCmdTable,
		typeCmdTableDirect = adCmdTableDirect,
		typeCmdStoredProc = adCmdStoredProc,
		typeCmdUnknown = adCmdUnknown,
		typeCmdFile = adCmdFile
	};
	
    CADOCommand(CADODatabase* pAdoDatabase, std::string strCommandText = "", int nCommandType = typeCmdStoredProc);
		
	~CADOCommand()
	{
		m_pCommand.Release();
		m_pCommand = NULL;
		m_strCommandText = "";
	}

    void SetText(std::string strCommandText);
	void SetType(int nCommandType);
	int GetType()
		{return m_nCommandType;};
	bool AddParameter(CADOParameter* pAdoParameter);
    bool AddParameter(std::string strName, int nType, int nDirection, long lSize, int nValue);
    bool AddParameter(std::string strName, int nType, int nDirection, long lSize, long lValue);
    bool AddParameter(std::string strName, int nType, int nDirection, long lSize, double dblValue, int nPrecision = 0, int nScale = 0);
    bool AddParameter(std::string strName, int nType, int nDirection, long lSize, std::string strValue);
    //bool AddParameter(std::string strName, int nType, int nDirection, long lSize, COleDateTime time);
    bool AddParameter(std::string strName, int nType, int nDirection, long lSize, _variant_t vtValue, int nPrecision = 0, int nScale = 0);
    std::string GetText()
		{return m_strCommandText;};
	bool Execute();
	int GetRecordsAffected()
		{return m_nRecordsAffected;};
	_CommandPtr GetCommand()
		{return m_pCommand;};

protected:
	void dump_com_error(_com_error &e);

protected:
	_CommandPtr m_pCommand;
	int m_nCommandType;
	int m_nRecordsAffected;
    std::string m_strCommandText;
    std::string m_strLastError;
	DWORD m_dwLastError;
};

////////////////////////////////////////////////////////////////////////////
//
//	Ado单一封装 
//
class AdoDbOper
{
public :
	AdoDbOper();
	~AdoDbOper();

	_ConnectionPtr 	FpConnectionPtr;
	_CommandPtr    	FpCommandPtr;
	_RecordsetPtr  	FpRecordsetPtr;

	bool Finit;
	bool Fconnected;
	int  Ftimeout;
	string Fserver;
	string Fdatabase;
	string Fusername;
	string Fpassword;
	string Fport;
	string Fconnectstring;
	//init
	bool BInit();
	bool BUnInit();
	//connectoper
	void VConnDisConnect();
	bool BConnRetryConnect();
	bool BConnIsConnected();
	bool BConnKeepAlive();
	bool BConnByStr();
	bool BConnSqlServerByStr();
	bool BConnAccessByStr();
	bool BConnOracleByStr();
	//sqloper
	bool BOpenFmtSql(const char* format, ...);
	bool BOpenSql(const char* sql);	
	bool BDoFmtSql(const char* format, ...);
	bool BDoSql(const char* sql);
	void VCloseSql();
	//recordsetoper
	bool BRecIsEof();
	bool BRecMoveToFirst();
	bool BRecMoveToLast();
	bool BRecMoveToPrier();
	bool BRecMoveToNext();
	bool BRecMoveTo(int index);
	int  IRecGetRecordCount();
	int  IRecGetFieldCount();
	bool BRecUpdate();
	//fieldoper
	bool  BFldIsValid(const char* field_name);
	bool  BFldIsValid(int field_index);

	string SFldGetBuffer(const char* field_name);
	string SFldGetBuffer(int field_index);
	int    IFldGetLength(int field_index);
	int    IFldGetLength(const char* field_name);
	//	
	bool   BFldItemToVariant(const char* field_name, _variant_t& var_ret);
	bool   BFldItemToVariant(int field_index, _variant_t& var_ret);
	string SFldItemToString(const char* field_name);
	string SFldItemToString(int field_index);

	long   LFldItemtoInteger(const char* field_name);
	long   LFldItemtoInteger(int field_index);
	short  SFldItemtoSmallInteger(const char* field_name);
	short  SFldItemtoSmallInteger(int field_index);

	unsigned char CFldItemtoTinyInteger(const char* field_name);
	unsigned char CFldItemtoTinyInteger(int field_index);
	int    IFldItemToBinary(const char* field_name, void* out_buf, int max_len);
	int    IFldItemToBinary(int field_index, void* out_buf, int max_len);

	bool   BFldItemToBool(const char* field_name);
	bool   BFldItemToBool(int field_index);

	bool BFldItemSetString(const char* field_name, const char* str);	
	bool BFldItemSetInteger(const char* field_name, long value);
	//trans control
	long BeginTransaction() 
		{return FpConnectionPtr->BeginTrans();};
	long CommitTransaction() 
		{return FpConnectionPtr->CommitTrans();};
	long RollbackTransaction() 
		{return FpConnectionPtr->RollbackTrans();};
};


#endif
