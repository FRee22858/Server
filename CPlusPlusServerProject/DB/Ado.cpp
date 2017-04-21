///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����Ado.h
// �����ߣ�yuanzy[��Carlos Antollini�ĳ����޸Ķ���]
// ����ʱ�䣺2006-3-17
// ������������־�ļ�������ͷ�ļ�
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "Ado.h"

#define ChunkSize 100
///////////////////////////////////////////////////////
//
// CADODatabase Class
//
DWORD CADODatabase::GetRecordCount(_RecordsetPtr m_pRs)
{
	DWORD numRows = 0;
	
	numRows = m_pRs->GetRecordCount();

	if(numRows == -1)
	{
		if(m_pRs->EndOfFile != VARIANT_TRUE)
			m_pRs->MoveFirst();

		while(m_pRs->EndOfFile != VARIANT_TRUE)
		{
			numRows++;
			m_pRs->MoveNext();
		}
		if(numRows > 0)
			m_pRs->MoveFirst();
	}
	return numRows;
}

bool CADODatabase::Open(LPCTSTR lpstrConnection, LPCTSTR lpstrUserID, LPCTSTR lpstrPassword)
{
	HRESULT hr = S_OK;

	if(IsOpen())
		Close();

	if(strcmp(lpstrConnection, "") != 0)
		m_strConnection = lpstrConnection;

    assert(!m_strConnection.empty());

	try
	{
        hr = m_pConnection->Open(_bstr_t(m_strConnection.c_str()), _bstr_t(lpstrUserID), _bstr_t(lpstrPassword), NULL);
		return hr == S_OK;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
	
}

void CADODatabase::dump_com_error(_com_error &e)
{
	char ErrorStr[1024];

    memset(ErrorStr,0,1024);
    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());
    sprintf(ErrorStr,"CADODataBase Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
        e.Error(), e.ErrorMessage(), (const char*)e.Source(), (const char*)e.Description());

    m_strLastError = ErrorStr;
    m_dwLastError = e.Error(); 

#ifdef _DEBUG
    //AfxMessageBox( ErrorStr, MB_OK | MB_ICONERROR );
    printf(ErrorStr);
#endif		
}

bool CADODatabase::IsOpen()
{
	if(m_pConnection )
		return m_pConnection->GetState() != adStateClosed;
	return FALSE;
}

void CADODatabase::Close()
{
	if(IsOpen())
		m_pConnection->Close();
}


///////////////////////////////////////////////////////
//
// CADORecordset Class
//

CADORecordset::CADORecordset(CADODatabase* pAdoDatabase)
{
	m_pRecordset = NULL;
	m_pCmd = NULL;
	m_strQuery = "";
	m_pRecordset.CreateInstance(__uuidof(Recordset));
	m_pCmd.CreateInstance(__uuidof(Command));
	m_nEditStatus = CADORecordset::dbEditNone;
	m_nSearchDirection = CADORecordset::searchForward;

	m_pConnection = pAdoDatabase->GetActiveConnection();
}

bool CADORecordset::Open(_ConnectionPtr mpdb, LPCTSTR lpstrExec, int nOption)
{	
	Close();
	
	if(strcmp(lpstrExec, "") != 0)
		m_strQuery = lpstrExec;
	assert(m_strQuery != "");	
	//m_strQuery.TrimLeft();
	bool bIsSelect = true;//m_strQuery.Mid(0, strlen("Select ")).CompareNoCase("select ") == 0 && nOption == openUnknown;
	try
	{
		m_pRecordset->CursorType = adOpenStatic;
		m_pRecordset->CursorLocation = adUseClient;
		if(bIsSelect || nOption == openQuery || nOption == openUnknown)
            m_pRecordset->Open((LPCSTR)m_strQuery.c_str(), _variant_t((IDispatch*)mpdb, TRUE), 
							adOpenStatic, adLockOptimistic, adCmdUnknown);
		else if(nOption == openTable)
            m_pRecordset->Open((LPCSTR)m_strQuery.c_str(), _variant_t((IDispatch*)mpdb, TRUE), 
							adOpenKeyset, adLockOptimistic, adCmdTable);
		else if(nOption == openStoredProc)
		{
			m_pCmd->ActiveConnection = mpdb;
            m_pCmd->CommandText = _bstr_t(m_strQuery.c_str());
			m_pCmd->CommandType = adCmdStoredProc;
			m_pConnection->CursorLocation = adUseClient;
			
			m_pRecordset = m_pCmd->Execute(NULL, NULL, adCmdText);
		}
		else
		{
			//TRACE( "Unknown parameter. %d", nOption);
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}

	return m_pRecordset != NULL;
}


bool CADORecordset::Open(LPCTSTR lpstrExec, int nOption)
{
    string msg = string("ִ��SQL���: ") + lpstrExec;
//    CAppLog::GetInstance()->WriteLog(OTHER,LOG_EVENT_DEBUG,msg.c_str());
	assert(m_pConnection != NULL);
	assert(m_pConnection->GetState() != adStateClosed);
	return Open(m_pConnection, lpstrExec, nOption);
}

bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, double& dbValue)
{	
	double val = (double)NULL;
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_R4:
			val = vtFld.fltVal;
			break;
		case VT_R8:
			val = vtFld.dblVal;
			break;
		case VT_DECIMAL:
			//Corrected by Jos?Carlos Mart�nez Gal�n
			val = vtFld.decVal.Lo32;
			val *= (vtFld.decVal.sign == 128)? -1 : 1;
			val /= pow(10.0, vtFld.decVal.scale); 
			break;
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_I2:
		case VT_I4:
			val = vtFld.lVal;
			break;
		case VT_INT:
			val = vtFld.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.dblVal;
		}
		dbValue = val;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}


bool CADORecordset::GetFieldValue(int nIndex, double& dbValue)
{	
	double val = (double)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt)
		{
		case VT_R4:
			val = vtFld.fltVal;
			break;
		case VT_R8:
			val = vtFld.dblVal;
			break;
		case VT_DECIMAL:
			//Corrected by Jos?Carlos Mart�nez Gal�n
			val = vtFld.decVal.Lo32;
			val *= (vtFld.decVal.sign == 128)? -1 : 1;
			val /= pow(10.0, vtFld.decVal.scale); 
			break;
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_I2:
		case VT_I4:
			val = vtFld.lVal;
			break;
		case VT_INT:
			val = vtFld.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = 0;
		}
		dbValue = val;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}


bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, long& lValue)
{
	long val = (long)NULL;
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.lVal;
		lValue = val;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, long& lValue)
{
	long val = (long)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.lVal;
		lValue = val;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}


bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, unsigned long& ulValue)
{
	long val = (long)NULL;
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.ulVal;
		ulValue = val;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, unsigned long& ulValue)
{
	long val = (long)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.ulVal;
		ulValue = val;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, int& nValue)
{
	int val = NULL;
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			val = vtFld.boolVal;
			break;
		case VT_I2:
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_INT:
			val = vtFld.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.iVal;
		}	
		nValue = val;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, int& nValue)
{
	int val = (int)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			val = vtFld.boolVal;
			break;
		case VT_I2:
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_INT:
			val = vtFld.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.iVal;
		}	
		nValue = val;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, std::string& strValue, std::string strDateFormat)
{
    std::string str = "";
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_R4:
			str = DblToStr(vtFld.fltVal);
			break;
		case VT_R8:
			str = DblToStr(vtFld.dblVal);
			break;
		case VT_BSTR:
			str = (const char*)_bstr_t(vtFld.bstrVal);
			break;
		case VT_I2:
		case VT_UI1:
			str = IntToStr(vtFld.iVal);
			break;
		case VT_INT:
			str = IntToStr(vtFld.intVal);
			break;
		case VT_I4:
			str = LongToStr(vtFld.lVal);
			break;
		case VT_UI4:
			str = ULongToStr(vtFld.ulVal);
			break;
		case VT_DECIMAL:
			{
			//Corrected by Jos?Carlos Mart�nez Gal�n
			double val = vtFld.decVal.Lo32;
			val *= (vtFld.decVal.sign == 128)? -1 : 1;
			val /= pow(10.0, vtFld.decVal.scale); 
			str = DblToStr(val);
			}
			break;
		case VT_DATE:
			{
                //time_t dt = vtFld;

                //if(strDateFormat.c_str() == "")
					//strDateFormat = "%Y-%m-%d %H:%M:%S";
				//str = DateTimeToStr(dt);
                str = _bstr_t(vtFld);//DateTimeToStr(vtFl);
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			str.empty();
			break;
		default:
			str.empty();
			return FALSE;
		}
		strValue = str;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, std::string& strValue, std::string strDateFormat)
{
    std::string str = "";
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt) 
		{
		case VT_R4:
			str = DblToStr(vtFld.fltVal);
			break;
		case VT_R8:
			str = DblToStr(vtFld.dblVal);
			break;
		case VT_BSTR:
			str = (const char*)_bstr_t(vtFld.bstrVal);
			break;
		case VT_I2:
		case VT_UI1:
			str = IntToStr(vtFld.iVal);
			break;
		case VT_INT:
			str = IntToStr(vtFld.intVal);
			break;
		case VT_I4:
			str = LongToStr(vtFld.lVal);
			break;
		case VT_UI4:
			str = ULongToStr(vtFld.ulVal);
			break;
		case VT_DECIMAL:
			{
			//Corrected by Jos?Carlos Mart�nez Gal�n
			double val = vtFld.decVal.Lo32;
			val *= (vtFld.decVal.sign == 128)? -1 : 1;
			val /= pow(10.0, vtFld.decVal.scale); 
			str = DblToStr(val);
			}
			break;
		case VT_DATE:
			{
                //time_t dt = vtFld.date;

                //if(strDateFormat.c_str() == "")
					//strDateFormat = "%Y-%m-%d %H:%M:%S";
				str = _bstr_t(vtFld);//DateTimeToStr(dt);
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			str.empty();
			break;
		default:
			str.empty();
			return FALSE;
		}
		strValue = str;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}


bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, bool& bValue)
{
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_BOOL:
			bValue = vtFld.boolVal == TRUE? true: false;
			break;
		case VT_EMPTY:
		case VT_NULL:
			bValue = false;
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, bool& bValue)
{
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt) 
		{
		case VT_BOOL:
			bValue = vtFld.boolVal == TRUE? true: false;
			break;
		case VT_EMPTY:
		case VT_NULL:
			bValue = false;
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, _variant_t& vtValue)
{
	try
	{
		vtValue = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, _variant_t& vtValue)
{
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtValue = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::IsFieldNull(LPCTSTR lpFieldName)
{
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		return vtFld.vt == VT_NULL;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::IsFieldNull(int nIndex)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		return vtFld.vt == VT_NULL;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::IsFieldEmpty(LPCTSTR lpFieldName)
{
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		return vtFld.vt == VT_EMPTY || vtFld.vt == VT_NULL;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::IsFieldEmpty(int nIndex)
{
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		return vtFld.vt == VT_EMPTY || vtFld.vt == VT_NULL;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::SetFieldEmpty(LPCTSTR lpFieldName)
{
	_variant_t vtFld;
	vtFld.vt = VT_EMPTY;
	
	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetFieldEmpty(int nIndex)
{
	_variant_t vtFld;
	vtFld.vt = VT_EMPTY;

	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	return PutFieldValue(vtIndex, vtFld);
}


DWORD CADORecordset::GetRecordCount()
{
	DWORD nRows = 0;
	
	nRows = m_pRecordset->GetRecordCount();

	if(nRows == -1)
	{
		nRows = 0;
		if(m_pRecordset->EndOfFile != VARIANT_TRUE)
			m_pRecordset->MoveFirst();
		
		while(m_pRecordset->EndOfFile != VARIANT_TRUE)
		{
			nRows++;
			m_pRecordset->MoveNext();
		}
		if(nRows > 0)
			m_pRecordset->MoveFirst();
	}
	
	return nRows;
}

bool CADORecordset::IsOpen()
{
	if(m_pRecordset)
		return m_pRecordset->GetState() != adStateClosed;
	return FALSE;
}

void CADORecordset::Close()
{
	if(IsOpen())
	{
		if (m_nEditStatus != dbEditNone)
		      CancelUpdate();

		m_pRecordset->Close();	
	}
}


bool CADODatabase::Execute(LPCTSTR lpstrExec)
{
	assert(m_pConnection != NULL);
	assert(strcmp(lpstrExec, "") != 0);
	_variant_t vRecords;
	
	m_nRecordsAffected = 0;

	try
	{
		m_pConnection->CursorLocation = adUseClient;
		m_pConnection->Execute(_bstr_t(lpstrExec), &vRecords, adExecuteNoRecords);
		m_nRecordsAffected = vRecords.iVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;	
	}
}

bool CADORecordset::RecordBinding(CADORecordBinding &pAdoRecordBinding)
{
	HRESULT hr;
	m_pRecBinding = NULL;

	//Open the binding interface.
	if(FAILED(hr = m_pRecordset->QueryInterface(__uuidof(IADORecordBinding), (LPVOID*)&m_pRecBinding )))
	{
		_com_issue_error(hr);
		return FALSE;
	}
	
	//Bind the recordset to class
	if(FAILED(hr = m_pRecBinding->BindToRecordset(&pAdoRecordBinding)))
	{
		_com_issue_error(hr);
		return FALSE;
	}
	return TRUE;
}

bool CADORecordset::GetFieldInfo(LPCTSTR lpFieldName, CADOFieldInfo* fldInfo)
{
	FieldPtr pField = m_pRecordset->Fields->GetItem(lpFieldName);	
	return GetFieldInfo(pField, fldInfo);
}

bool CADORecordset::GetFieldInfo(int nIndex, CADOFieldInfo* fldInfo)
{
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	FieldPtr pField = m_pRecordset->Fields->GetItem(vtIndex);
	return GetFieldInfo(pField, fldInfo);
}


bool CADORecordset::GetFieldInfo(FieldPtr pField, CADOFieldInfo* fldInfo)
{
	memset(fldInfo, 0, sizeof(CADOFieldInfo));

	strcpy(fldInfo->m_strName, (LPCTSTR)pField->GetName());
	fldInfo->m_lDefinedSize = pField->GetDefinedSize();
	fldInfo->m_nType = pField->GetType();
	fldInfo->m_lAttributes = pField->GetAttributes();
	if(!IsEof())
		fldInfo->m_lSize = pField->GetActualSize();
	return TRUE;
}

bool CADORecordset::GetChunk(LPCTSTR lpFieldName, std::string& strValue)
{
	FieldPtr pField = m_pRecordset->Fields->GetItem(lpFieldName);	
    return GetChunk(pField, A2BSTR(strValue.c_str()));
}

bool CADORecordset::GetChunk(int nIndex, std::string& strValue)
{
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	FieldPtr pField = m_pRecordset->Fields->GetItem(vtIndex);	
    return GetChunk(pField, A2BSTR(strValue.c_str()));
}


bool CADORecordset::GetChunk(FieldPtr pField, std::string& strValue)
{
    std::string str = "";
	long lngSize, lngOffSet = 0;
	_variant_t varChunk;

	lngSize = pField->ActualSize;
	
    str.empty();
	while(lngOffSet < lngSize)
	{ 
		try
		{
			varChunk = pField->GetChunk(ChunkSize);
			
			str += (const char*)varChunk.bstrVal;
			lngOffSet += ChunkSize;
		}
		catch(_com_error &e)
		{
			dump_com_error(e);
			return FALSE;
		}
	}

	lngOffSet = 0;
	strValue = str;
	return TRUE;
}

bool CADORecordset::GetChunk(LPCTSTR lpFieldName, LPVOID lpData)
{
	FieldPtr pField = m_pRecordset->Fields->GetItem(lpFieldName);
	return GetChunk(pField, lpData);
}

bool CADORecordset::GetChunk(int nIndex, LPVOID lpData)
{
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	FieldPtr pField = m_pRecordset->Fields->GetItem(vtIndex);
	return GetChunk(pField, lpData);
}

bool CADORecordset::GetChunk(FieldPtr pField, LPVOID lpData)
{
	long lngSize, lngOffSet = 0;
	_variant_t varChunk;    
	UCHAR chData;
	HRESULT hr;
	long lBytesCopied = 0;

	lngSize = pField->ActualSize;
	
	while(lngOffSet < lngSize)
	{ 
		try
		{
			varChunk = pField->GetChunk(ChunkSize);

			//Copy the data only upto the Actual Size of Field.  
            for(long lIndex = 0; lIndex <= (ChunkSize - 1); lIndex++)
            {
                hr= SafeArrayGetElement(varChunk.parray, &lIndex, &chData);
                if(SUCCEEDED(hr))
                {
                    //Take BYTE by BYTE and advance Memory Location
                    //hr = SafeArrayPutElement((SAFEARRAY FAR*)lpData, &lBytesCopied ,&chData); 
					((UCHAR*)lpData)[lBytesCopied] = chData;
                    lBytesCopied++;
                }
                else
                    break;
            }
			lngOffSet += ChunkSize;
		}
		catch(_com_error &e)
		{
			dump_com_error(e);
			return FALSE;
		}
	}
	lngOffSet = 0;
	return TRUE;
}

bool CADORecordset::AppendChunk(LPCTSTR lpFieldName, LPVOID lpData, UINT nBytes)
{
	FieldPtr pField = m_pRecordset->Fields->GetItem(lpFieldName);
	return AppendChunk(pField, lpData, nBytes);
}


bool CADORecordset::AppendChunk(int nIndex, LPVOID lpData, UINT nBytes)
{
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	FieldPtr pField = m_pRecordset->Fields->GetItem(vtIndex);

	return AppendChunk(pField, lpData, nBytes);
}

bool CADORecordset::AppendChunk(FieldPtr pField, LPVOID lpData, UINT nBytes)
{
	HRESULT hr;
	_variant_t varChunk;
	long lngOffset = 0;
	UCHAR chData;
	SAFEARRAY FAR *psa = NULL;
	SAFEARRAYBOUND rgsabound[1];

	try
	{
		//Create a safe array to store the array of BYTES 
		rgsabound[0].lLbound = 0;
		rgsabound[0].cElements = nBytes;
		psa = SafeArrayCreate(VT_UI1,1,rgsabound);

		while(lngOffset < (long)nBytes)
		{
			chData	= ((UCHAR*)lpData)[lngOffset];
			hr = SafeArrayPutElement(psa, &lngOffset, &chData);

			if(FAILED(hr))
				return FALSE;			
			lngOffset++;
		}
		lngOffset = 0;

		//Assign the Safe array  to a variant. 
		varChunk.vt = VT_ARRAY|VT_UI1;
		varChunk.parray = psa;

		hr = pField->AppendChunk(varChunk);

		if(SUCCEEDED(hr))
			return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}

	return FALSE;
}

std::string CADORecordset::GetString(LPCTSTR lpCols, LPCTSTR lpRows, LPCTSTR lpNull, long numRows)
{
	_bstr_t varOutput;
	_bstr_t varNull("");
	_bstr_t varCols("\t");
	_bstr_t varRows("\r");

	if(strlen(lpCols) != 0)
		varCols = _bstr_t(lpCols);

	if(strlen(lpRows) != 0)
		varRows = _bstr_t(lpRows);
	
	if(numRows == 0)
		numRows =(long)GetRecordCount();			
			
	varOutput = m_pRecordset->GetString(adClipString, numRows, varCols, varRows, varNull);

	return (LPCTSTR)varOutput;
}

std::string IntToStr(int nVal)
{
    std::string strRet;
	char buff[10];
	
	itoa(nVal, buff, 10);
	strRet = buff;
	return strRet;
}

std::string LongToStr(long lVal)
{
    std::string strRet;
	char buff[20];
	
	ltoa(lVal, buff, 10);
	strRet = buff;
	return strRet;
}

std::string ULongToStr(unsigned long ulVal)
{
    std::string strRet;
	char buff[20];
	
	ultoa(ulVal, buff, 10);
	strRet = buff;
	return strRet;

}


std::string DblToStr(double dblVal, int ndigits)
{
    std::string strRet;
	char buff[50];

   _gcvt(dblVal, ndigits, buff);
	strRet = buff;
	return strRet;
}

std::string DblToStr(float fltVal)
{
    std::string strRet = _T("");
	char buff[50];
	
   _gcvt(fltVal, 10, buff);
	strRet = buff;
	return strRet;
}

std::string DateTimeToStr(time_t t)
{
    std::string ret("");
	struct tm *cur_t = localtime(&t);
	char now[64];	
    if (0 != cur_t)
    {
	    sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d ", cur_t->tm_year+1900, cur_t->tm_mon+1, cur_t->tm_mday, cur_t->tm_hour, cur_t->tm_min, cur_t->tm_sec);
	    ret = now;
	}
	return ret;
};

void CADORecordset::Edit()
{
	m_nEditStatus = dbEdit;
}

bool CADORecordset::AddNew()
{
	m_nEditStatus = dbEditNone;
	if(m_pRecordset->AddNew() != S_OK)
		return FALSE;

	m_nEditStatus = dbEditNew;
	return TRUE;
}

bool CADORecordset::AddNew(CADORecordBinding &pAdoRecordBinding)
{
	try
	{
		if(m_pRecBinding->AddNew(&pAdoRecordBinding) != S_OK)
		{
			return FALSE;
		}
		else
		{
			m_pRecBinding->Update(&pAdoRecordBinding);
			return TRUE;
		}
			
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}	
}

bool CADORecordset::Update()
{
	bool bret = TRUE;
	if(m_nEditStatus != dbEditNone)
	{
		try
		{
			if(m_pRecordset->Update() != S_OK)
				bret = FALSE;
		}
		catch(_com_error &e)
		{
			dump_com_error(e);
			bret = FALSE;
		}
		if(!bret)
			m_pRecordset->CancelUpdate();
		m_nEditStatus = dbEditNone;
	}
	return bret;
}

void CADORecordset::CancelUpdate()
{
	m_pRecordset->CancelUpdate();
	m_nEditStatus = dbEditNone;
}

bool CADORecordset::SetFieldValue(int nIndex, std::string strValue)
{
	_variant_t vtFld;
	_variant_t vtIndex;	
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

    if(strValue.c_str() !="")
		vtFld.vt = VT_BSTR;
	else
		vtFld.vt = VT_NULL;

	//Corrected by Giles Forster 10/03/2001
    vtFld.bstrVal = A2BSTR((const char*)strValue.c_str());
	return PutFieldValue(vtIndex, vtFld);
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, std::string strValue)
{
	_variant_t vtFld;

    if(strValue.c_str() != "")
		vtFld.vt = VT_BSTR;
	else
		vtFld.vt = VT_NULL;

	//Corrected by Giles Forster 10/03/2001
	vtFld.bstrVal = A2BSTR((const char*)strValue.c_str());

	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetFieldValue(int nIndex, int nValue)
{
	_variant_t vtFld;
	
	vtFld.vt = VT_I2;
	vtFld.iVal = nValue;
	
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	return PutFieldValue(vtIndex, vtFld);
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, int nValue)
{
	_variant_t vtFld;
	
	vtFld.vt = VT_I2;
	vtFld.iVal = nValue;
	
	
	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetFieldValue(int nIndex, long lValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.lVal = lValue;
	
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	return PutFieldValue(vtIndex, vtFld);
	
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, long lValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.lVal = lValue;
	
	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetFieldValue(int nIndex, unsigned long ulValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_UI4;
	vtFld.ulVal = ulValue;
	
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	return PutFieldValue(vtIndex, vtFld);
	
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, unsigned long ulValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_UI4;
	vtFld.ulVal = ulValue;
	
	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetFieldValue(int nIndex, double dblValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_R8;
	vtFld.dblVal = dblValue;

	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	return PutFieldValue(vtIndex, vtFld);
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, double dblValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_R8;
	vtFld.dblVal = dblValue;
		
	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetFieldValue(int nIndex, _variant_t vtValue)
{
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	return PutFieldValue(vtIndex, vtValue);
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, _variant_t vtValue)
{	
	return PutFieldValue(lpFieldName, vtValue);
}


bool CADORecordset::SetBookmark()
{
	if(m_varBookmark.vt != VT_EMPTY)
	{
		m_pRecordset->Bookmark = m_varBookmark;
		return TRUE;
	}
	return FALSE;
}

bool CADORecordset::Delete()
{
	if(m_pRecordset->Delete(adAffectCurrent) != S_OK)
		return FALSE;

	if(m_pRecordset->Update() != S_OK)
		return FALSE;
	
	m_nEditStatus = dbEditNone;
	return TRUE;
}

bool CADORecordset::Find(LPCTSTR lpFind, int nSearchDirection)
{

	m_strFind = lpFind;
	m_nSearchDirection = nSearchDirection;

    assert(m_strFind.c_str() != "");

	if(m_nSearchDirection == searchForward)
	{
        m_pRecordset->Find(_bstr_t(m_strFind.c_str()), 0, adSearchForward, "");
		if(!IsEof())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return TRUE;
		}
	}
	else if(m_nSearchDirection == searchBackward)
	{
        m_pRecordset->Find(_bstr_t(m_strFind.c_str()), 0, adSearchBackward, "");
		if(!IsBof())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return TRUE;
		}
	}
	else
	{
		//TRACE("Unknown parameter. %d", nSearchDirection);
		m_nSearchDirection = searchForward;
	}
	return FALSE;
}

bool CADORecordset::FindFirst(LPCTSTR lpFind)
{
	m_pRecordset->MoveFirst();
	return Find(lpFind);
}

bool CADORecordset::FindNext()
{
	if(m_nSearchDirection == searchForward)
	{
        m_pRecordset->Find(_bstr_t(m_strFind.c_str()), 1, adSearchForward, m_varBookFind);
		if(!IsEof())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return TRUE;
		}
	}
	else
	{
        m_pRecordset->Find(_bstr_t(m_strFind.c_str()), 1, adSearchBackward, m_varBookFind);
		if(!IsBof())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return TRUE;
		}
	}
	return FALSE;
}

bool CADORecordset::PutFieldValue(LPCTSTR lpFieldName, _variant_t vtFld)
{
	if(m_nEditStatus == dbEditNone)
		return FALSE;
	
	try
	{
		m_pRecordset->Fields->GetItem(lpFieldName)->Value = vtFld; 
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;	
	}
}


bool CADORecordset::PutFieldValue(_variant_t vtIndex, _variant_t vtFld)
{
	if(m_nEditStatus == dbEditNone)
		return FALSE;

	try
	{
		m_pRecordset->Fields->GetItem(vtIndex)->Value = vtFld;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::Clone(CADORecordset &pRs)
{
	try
	{
		pRs.m_pRecordset = m_pRecordset->Clone(adLockUnspecified);
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::SetFilter(LPCTSTR strFilter)
{
	assert(IsOpen());
	
	try
	{
		m_pRecordset->PutFilter(strFilter);
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::SetSort(LPCTSTR strCriteria)
{
	assert(IsOpen());
	
	try
	{
		m_pRecordset->PutSort(strCriteria);
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::SaveAsXML(LPCTSTR lpstrXMLFile)
{
	HRESULT hr;

	assert(IsOpen());
	
	try
	{
		hr = m_pRecordset->Save(lpstrXMLFile, adPersistXML);
		return hr == S_OK;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
	return TRUE;
}

bool CADORecordset::OpenXML(LPCTSTR lpstrXMLFile)
{
	HRESULT hr = S_OK;

	if(IsOpen())
		Close();

	try
	{
		hr = m_pRecordset->Open(lpstrXMLFile, "Provider=MSPersist;", adOpenForwardOnly, adLockOptimistic, adCmdFile);
		return hr == S_OK;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADORecordset::Execute(CADOCommand* pAdoCommand)
{
	if(IsOpen())
		Close();

    string msg = "ִ��SQL���: " + pAdoCommand->GetText();
//    CAppLog::GetInstance()->WriteLog(OTHER,LOG_EVENT_DEBUG,msg.c_str());

    assert(pAdoCommand->GetText().c_str() != "");
	try
	{
		m_pConnection->CursorLocation = adUseClient;
		m_pRecordset = pAdoCommand->GetCommand()->Execute(NULL, NULL, pAdoCommand->GetType());
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

void CADORecordset::dump_com_error(_com_error &e)
{
	char ErrorStr[1024];

    memset(ErrorStr,0,1024);
    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());
    sprintf(ErrorStr,"CADODataBase Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
        e.Error(), e.ErrorMessage(), (const char*)e.Source(), (const char*)e.Description());

    m_strLastError = ErrorStr;
    m_dwLastError = e.Error(); 

    #ifdef _DEBUG
    //AfxMessageBox( ErrorStr, MB_OK | MB_ICONERROR );
    printf(ErrorStr);
    #endif		
}


///////////////////////////////////////////////////////
//
// CADOCommad Class
//

CADOCommand::CADOCommand(CADODatabase* pAdoDatabase, std::string strCommandText, int nCommandType)
{
	m_pCommand = NULL;
	m_pCommand.CreateInstance(__uuidof(Command));
	m_strCommandText = strCommandText;
    m_pCommand->CommandText = _bstr_t((const char*)m_strCommandText.c_str());
	m_nCommandType = nCommandType;
	m_pCommand->CommandType = (CommandTypeEnum)m_nCommandType;
	m_pCommand->ActiveConnection = pAdoDatabase->GetActiveConnection();	
	m_nRecordsAffected = 0;
}

bool CADOCommand::AddParameter(CADOParameter* pAdoParameter)
{
	assert(pAdoParameter->GetParameter() != NULL);

	try
	{
		m_pCommand->Parameters->Append(pAdoParameter->GetParameter());
		return TRUE;
	}
	catch(_com_error& e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADOCommand::AddParameter(std::string strName, int nType, int nDirection, long lSize, int nValue)
{

	_variant_t vtValue;

	vtValue.vt = VT_I2;
	vtValue.iVal = nValue;

	return AddParameter(strName, nType, nDirection, lSize, vtValue);
}

bool CADOCommand::AddParameter(std::string strName, int nType, int nDirection, long lSize, long lValue)
{

	_variant_t vtValue;

	vtValue.vt = VT_I4;
	vtValue.lVal = lValue;

	return AddParameter(strName, nType, nDirection, lSize, vtValue);
}

bool CADOCommand::AddParameter(std::string strName, int nType, int nDirection, long lSize, double dblValue, int nPrecision, int nScale)
{

	_variant_t vtValue;

	vtValue.vt = VT_R8;
	vtValue.dblVal = dblValue;

	return AddParameter(strName, nType, nDirection, lSize, vtValue, nPrecision, nScale);
}

bool CADOCommand::AddParameter(std::string strName, int nType, int nDirection, long lSize, std::string strValue)
{

	_variant_t vtValue;

	vtValue.vt = VT_BSTR;
    vtValue.bstrVal = A2BSTR((const char*)strValue.c_str());

	return AddParameter(strName, nType, nDirection, lSize, vtValue);
}

bool CADOCommand::AddParameter(std::string strName, int nType, int nDirection, long lSize, _variant_t vtValue, int nPrecision, int nScale)
{
	try
	{
        _ParameterPtr pParam = m_pCommand->CreateParameter(_bstr_t((const char*)strName.c_str()), (DataTypeEnum)nType, (ParameterDirectionEnum)nDirection, lSize, vtValue);
		pParam->PutPrecision(nPrecision);
		pParam->PutNumericScale(nScale);
		m_pCommand->Parameters->Append(pParam);
		
		return TRUE;
	}
	catch(_com_error& e)
	{
		dump_com_error(e);
		return FALSE;
	}
}


void CADOCommand::SetText(std::string strCommandText)
{
	assert(strCommandText != "");

	m_strCommandText = strCommandText;
	m_pCommand->CommandText = A2BSTR((const char*)strCommandText.c_str());
}

void CADOCommand::SetType(int nCommandType)
{
	m_nCommandType = nCommandType;
	m_pCommand->CommandType = (CommandTypeEnum)m_nCommandType;
}

bool CADOCommand::Execute()
{
	_variant_t vRecords;
	m_nRecordsAffected = 0;
	try
	{
		m_pCommand->Execute(&vRecords, NULL, adCmdStoredProc);
		m_nRecordsAffected = vRecords.iVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

void CADOCommand::dump_com_error(_com_error &e)
{
	char ErrorStr[1024];

    memset(ErrorStr,0,1024);
    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());
    sprintf(ErrorStr,"CADODataBase Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
        e.Error(), e.ErrorMessage(), (const char*)e.Source(), (const char*)e.Description());

    m_strLastError = ErrorStr;
    m_dwLastError = e.Error(); 

    #ifdef _DEBUG
    //AfxMessageBox( ErrorStr, MB_OK | MB_ICONERROR );
    printf(ErrorStr);
    #endif		
}


///////////////////////////////////////////////////////
//
// CADOParameter Class
//

CADOParameter::CADOParameter(int nType, long lSize, int nDirection, std::string strName)
{
	m_pParameter = NULL;
	m_pParameter.CreateInstance(__uuidof(Parameter));
	m_strName = _T("");
	m_pParameter->Direction = (ParameterDirectionEnum)nDirection;
	m_strName = strName;
    m_pParameter->Name = _bstr_t((const char*)m_strName.c_str());
	m_pParameter->Type = (DataTypeEnum)nType;
	m_pParameter->Size = lSize;
}

bool CADOParameter::SetValue(int nValue)
{
	_variant_t vtVal;

	assert(m_pParameter != NULL);
	
	vtVal.vt = VT_I2;
	vtVal.iVal = nValue;

	try
	{
		if(m_pParameter->Size == 0)
			m_pParameter->Size = sizeof(int);

		m_pParameter->Value = vtVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}


bool CADOParameter::SetValue(long lValue)
{
	_variant_t vtVal;

	assert(m_pParameter != NULL);
	
	vtVal.vt = VT_I4;
	vtVal.lVal = lValue;

	try
	{
		if(m_pParameter->Size == 0)
			m_pParameter->Size = sizeof(long);

		m_pParameter->Value = vtVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADOParameter::SetValue(double dblValue)
{
	_variant_t vtVal;

	assert(m_pParameter != NULL);
	
	vtVal.vt = VT_R8;
	vtVal.dblVal = dblValue;

	try
	{
		if(m_pParameter->Size == 0)
			m_pParameter->Size = sizeof(double);

		m_pParameter->Value = vtVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADOParameter::SetValue(std::string strValue)
{
	_variant_t vtVal;

	assert(m_pParameter != NULL);
	
    if(strValue.c_str() !="")
		vtVal.vt = VT_BSTR;
	else
		vtVal.vt = VT_NULL;

	//Corrected by Giles Forster 10/03/2001
    vtVal.bstrVal = A2BSTR((const char*)strValue.c_str());

	try
	{
        size_t size = sizeof(char) * strValue.length();
		if(m_pParameter->Size == 0)
            m_pParameter->Size = (long)size;
		m_pParameter->Value = vtVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADOParameter::SetValue(_variant_t vtValue)
{

	assert(m_pParameter != NULL);

	try
	{
		if(m_pParameter->Size == 0)
			m_pParameter->Size = sizeof(VARIANT);
		
		m_pParameter->Value = vtValue;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADOParameter::GetValue(int& nValue)
{
	_variant_t vtVal;
	int nVal = 0;

	try
	{
		vtVal = m_pParameter->Value;

		switch(vtVal.vt)
		{
		case VT_BOOL:
			nVal = vtVal.boolVal;
			break;
		case VT_I2:
		case VT_UI1:
			nVal = vtVal.iVal;
			break;
		case VT_INT:
			nVal = vtVal.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			nVal = 0;
			break;
		default:
			nVal = vtVal.iVal;
		}	
		nValue = nVal;
		return TRUE;
	}
	catch(_com_error& e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADOParameter::GetValue(long& lValue)
{
	_variant_t vtVal;
	long lVal = 0;

	try
	{
		vtVal = m_pParameter->Value;
		if(vtVal.vt != VT_NULL && vtVal.vt != VT_EMPTY)
			lVal = vtVal.lVal;
		lValue = lVal;
		return TRUE;
	}
	catch(_com_error& e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADOParameter::GetValue(double& dbValue)
{
	_variant_t vtVal;
	double dblVal;
	try
	{
		vtVal = m_pParameter->Value;
		switch(vtVal.vt)
		{
		case VT_R4:
			dblVal = vtVal.fltVal;
			break;
		case VT_R8:
			dblVal = vtVal.dblVal;
			break;
		case VT_DECIMAL:
			//Corrected by Jos?Carlos Mart�nez Gal�n
			dblVal = vtVal.decVal.Lo32;
			dblVal *= (vtVal.decVal.sign == 128)? -1 : 1;
			dblVal /= pow(10.0, vtVal.decVal.scale); 
			break;
		case VT_UI1:
			dblVal = vtVal.iVal;
			break;
		case VT_I2:
		case VT_I4:
			dblVal = vtVal.lVal;
			break;
		case VT_INT:
			dblVal = vtVal.intVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			dblVal = 0;
			break;
		default:
			dblVal = 0;
		}
		dbValue = dblVal;
		return TRUE;
	}
	catch(_com_error& e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

bool CADOParameter::GetValue(_variant_t& vtValue)
{
	try
	{
		vtValue = m_pParameter->Value;
		return TRUE;
	}
	catch(_com_error& e)
	{
		dump_com_error(e);
		return FALSE;
	}
}


void CADOParameter::dump_com_error(_com_error &e)
{
	char ErrorStr[1024];

    memset(ErrorStr,0,1024);
    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());
    sprintf(ErrorStr,"CADODataBase Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
        e.Error(), e.ErrorMessage(), (const char*)e.Source(), (const char*)e.Description());

    m_strLastError = ErrorStr;
    m_dwLastError = e.Error(); 

    #ifdef _DEBUG
    //AfxMessageBox( ErrorStr, MB_OK | MB_ICONERROR );
//    CAppLog::GetInstance()->WriteLog(OTHER, LOG_EVENT_ERROR, ErrorStr);
    #endif		
}

//////////////////////////////////////////////////////////////////////////////////////
//
//Ado ��һ��װʵ��
//
///////////////////////////////////////////////////////////////////////////////////////
//AdoDbOper////////////////////////////////////////////////
AdoDbOper::AdoDbOper()
{
	/*  gaoxd 2009-02-18 ��ʱ���Σ�����ûʲô�ô�
	Finit = false;
	BInit();
	string flocal = PropManager::GetInstance()->GetDbPath() + "/tvmon.mdb";
	WIN32_FIND_DATA * fileInfo = new WIN32_FIND_DATA;
    ::FindFirstFile(flocal.c_str(), fileInfo);
    if(fileInfo->cFileName[0] < 'A' || fileInfo->cFileName[0] > 'z')
    {
        string msg = "�޷���λ���ݿ�,��ȷ�����ݿ�"+Fdatabase+"�Ƿ���ڣ�";
        CAppLog::GetInstance()->WriteLog(OTHER, LOG_EVENT_ERROR, msg.c_str());
        getchar();
        delete fileInfo;        
        exit(0);
    } else if(fileInfo->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
    {
        CAppLog::GetInstance()->WriteLog(OTHER, LOG_EVENT_ERROR, "���ݿ�ֻ��,�޷�ִ�г���");
        delete fileInfo;
        getchar();
        exit(0);
    }
    else
    {
        delete fileInfo;
	}
	string Fconnectstring = "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=";
	Fconnectstring += flocal;	
	Fconnectstring += ";Persist Security Info=False";
	Fconnectstring += ";";
	if (!FpConnectionPtr->State == adStateOpen ? true : false)
	{
		FpConnectionPtr->Open(_bstr_t((Fconnectstring).c_str()), "", "", adModeUnknown);
	}
	*/
}
AdoDbOper::~AdoDbOper()
{
	BUnInit();
}
//init
bool AdoDbOper::BInit()
{
	if (!Finit)
	{
		FpConnectionPtr =NULL ; 
		FpCommandPtr =NULL;
  		FpRecordsetPtr =NULL;

		HRESULT hr= NULL;		
		hr = ::CoInitialize(0);
		if (!SUCCEEDED(hr)) return false;

		hr = FpConnectionPtr.CreateInstance(__uuidof(Connection));
		if (!SUCCEEDED(hr)) return false;

		hr = FpRecordsetPtr.CreateInstance(__uuidof(Recordset)/*"ADODB.Recordset"*/);
		if (!SUCCEEDED(hr)) return false;

		hr = FpCommandPtr.CreateInstance(__uuidof(Command)/*"ADODB.Command"*/);
		if (!SUCCEEDED(hr)) return false;

		FpRecordsetPtr->CursorLocation = adUseClient;		
		FpRecordsetPtr->CursorType = adOpenKeyset;
		FpRecordsetPtr->LockType = adLockOptimistic;
		Finit = true;
	}
	return Finit;
}
bool  AdoDbOper::BUnInit()
{	
	bool ret = false;
	if (Finit)
	{
		if (FpRecordsetPtr != NULL)
		{
			FpRecordsetPtr->Close();
			FpRecordsetPtr = NULL;
		}
		if (FpCommandPtr != NULL)
		{
			//FpCommandPtr->Close();
			FpCommandPtr = NULL;
		}
		VConnDisConnect();
		Finit = false;		
		ret = true;
	}
	return ret;
}
//conn
bool AdoDbOper::BConnSqlServerByStr()
{
	Fconnectstring += "Provider=SQLOLEDB;Data Source=";
	Fconnectstring += Fserver;
	Fconnectstring += ";Initial Catalog=";
	Fconnectstring += Fdatabase;
	Fconnectstring += ";User ID=";
	Fconnectstring += Fusername;
	Fconnectstring += ";Password=";
	Fconnectstring += Fpassword;
	Fconnectstring += ";";
	return BConnByStr();
}

bool AdoDbOper::BConnAccessByStr()
{
	Fconnectstring += "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=";
	Fconnectstring += Fdatabase;	
	if (Fpassword.empty())
	{
		Fconnectstring += ";Persist Security Info=False";
	}	
	else
	{   
		Fconnectstring += ";jet oledb:database password=";
		Fconnectstring += Fpassword;		
		Fconnectstring += ";Persist Security Info=false";
	}
	Fconnectstring += ";";
	return BConnByStr();	
}

bool AdoDbOper::BConnOracleByStr()
{
	Fconnectstring += "Provider=MSDAORA.1; Data Source=";
	Fconnectstring += Fserver;
	Fconnectstring += ";UID=";
	Fconnectstring += Fusername;
	Fconnectstring += ";PWD=";
	Fconnectstring += Fpassword;
	Fconnectstring += ";";
	return BConnByStr();
}
	

bool AdoDbOper::BConnByStr()
{
	if (!Finit) return false;
	if (BConnIsConnected()) VConnDisConnect();	
	if (FpConnectionPtr)
	{
		try
		{
			if (S_OK == FpConnectionPtr->Open(_bstr_t((Fconnectstring).c_str()), "", "", adModeUnknown))
			{}
		} catch (_com_error e)
		{			
			return false;
		} catch (...)
		{			
			return false;
		}
	}
	return (FpConnectionPtr->State == adStateOpen ? true : false);	
}

bool AdoDbOper::BConnIsConnected()
{
	return (FpConnectionPtr->State == adStateOpen ? true : false);
}

void AdoDbOper::VConnDisConnect()
{
	if (!BConnIsConnected()) return;
	VCloseSql();
	if (FpConnectionPtr != 0 && FpConnectionPtr->State == adStateOpen)
	{
		try 
		{
			FpConnectionPtr->Close();
			FpConnectionPtr->Release();
			FpConnectionPtr = NULL;
		}	catch (...)
		{
			return;
		}
	}
}

bool AdoDbOper::BConnRetryConnect()
{
	bool ret = false;
	if (!BUnInit()) return ret;
	if (!BInit()) return ret;	
	return BConnByStr();
}

bool AdoDbOper::BConnKeepAlive()
{
	bool ret = false;
	_variant_t var;
	try
	{
		FpConnectionPtr->Execute(_bstr_t("SELECT cur_time=GETDATE()"), &var, adCmdText);		
		ret = true;
	}
	catch (_com_error e)
	{
		FpConnectionPtr->Close();
	}
	catch (...)
	{		
		FpConnectionPtr->Close();		
	}
	if (!ret) ret = BConnRetryConnect();
	return ret;
}
//sqloper
void AdoDbOper::VCloseSql()
{
	if (!BConnIsConnected()) return;
	if (FpRecordsetPtr != 0 && FpRecordsetPtr->State == adStateOpen)
	{
		try
		{
			FpRecordsetPtr->Close();
		} catch (...)
		{
			return;
		}	
	}
}
bool AdoDbOper::BOpenFmtSql(const char* format, ...)
{
	if (!BConnIsConnected()) return false;
	char* sql = new char[strlen(format) + 2048];

	va_list args;
	va_start(args, format);
	_vsnprintf(sql, 2048, format, args);
	va_end(args);

	bool ret = BOpenSql(sql);
	delete[] sql;
	return ret;
}

bool AdoDbOper::BOpenSql(const char* sql)
{	
	if (!BConnIsConnected()) 
	{
		if (!BConnRetryConnect()) return false;
	}	
	try
	{
		VCloseSql();
		FpRecordsetPtr->Open(_bstr_t(sql), FpConnectionPtr.GetInterfacePtr(),
			adOpenKeyset, adLockOptimistic, adCmdText);		
	} catch (_com_error e)
	{		
		return false;
	} catch (...)
	{	
		return false;
	}
	return (FpRecordsetPtr->State == adStateOpen ? true : false);
}

bool AdoDbOper::BDoFmtSql(const char* format, ...)
{
	if (!BConnIsConnected()) return false;

	size_t len = strlen(format) + 2048;
	char* sql = new char[len];

	va_list args;
	va_start(args, format);
	_vsnprintf(sql, len, format, args);
	va_end(args);

	bool ret = BDoSql(sql);
	delete[] sql;
	return ret;

}
bool AdoDbOper::BDoSql(const char* sql)
{
	if (!BConnIsConnected()) 
	{
		if (!BConnRetryConnect()) return false;
	}
	_variant_t var;
	try
	{
		FpConnectionPtr->Execute(_bstr_t(sql), &var, adCmdText);
		return true;
	} catch (_com_error e)
	{		
		return false;
	} catch (...)
	{		
		return false;
	}	
}
//recordsetoper
bool AdoDbOper::BRecUpdate()
{
	try
	{
		FpRecordsetPtr->Update();
		return true;
	} catch (_com_error e)
	{	
		return false;
	} catch (...)
	{
		return false;
	}
}

bool AdoDbOper::BRecIsEof()
{
	if (FpRecordsetPtr != 0 && FpRecordsetPtr->State == adStateOpen)
	{
		return (FpRecordsetPtr->EndOfFile ? true : false);
	}
	return false;
};
bool AdoDbOper::BRecMoveToFirst()
{
	if (FpRecordsetPtr != 0 && FpRecordsetPtr->State == adStateOpen)
	{
		try
		{
			return (S_OK == FpRecordsetPtr->MoveFirst());
		} catch (_com_error e)
		{
			return false;
		} catch (...)
		{
			return false;
		}
	}
	return false;
};
	
bool AdoDbOper::BRecMoveToLast()
{
	if (FpRecordsetPtr != 0 && FpRecordsetPtr->State == adStateOpen)
	{
		try
		{
			return (S_OK == FpRecordsetPtr->MoveLast());
		} catch (_com_error e)
		{
			return false;
		} catch (...)
		{
			return false;
		}
	}
	return false;
};
	
bool AdoDbOper::BRecMoveToPrier()
{
	if (FpRecordsetPtr != 0 && FpRecordsetPtr->State == adStateOpen)
	{
		try
		{
			return (S_OK == FpRecordsetPtr->MovePrevious());
		} catch (_com_error e)
		{
			return false;
		} catch (...)
		{
			return false;
		}
	}
	return false;
};

bool AdoDbOper::BRecMoveToNext()
{
	if (FpRecordsetPtr != 0 && FpRecordsetPtr->State == adStateOpen)
	{
		try
		{
			return (S_OK == FpRecordsetPtr->MoveNext());
		} catch (_com_error e)
		{
			return false;
		} catch (...)
		{
			return false;
		}
	}
	return false;
};

bool AdoDbOper::BRecMoveTo(int index)
{
	if (FpRecordsetPtr != 0 && FpRecordsetPtr->State == adStateOpen)
	{
		try
		{
			FpRecordsetPtr->MoveFirst();
			return (S_OK == FpRecordsetPtr->Move(long(index)));
		} catch (_com_error e)
		{
			return false;
		} catch (...)
		{
			return false;
		}
	}
	return false;
};
int  AdoDbOper::IRecGetRecordCount()
{
	if (FpRecordsetPtr != 0 && FpRecordsetPtr->State == adStateOpen)
	{
		return FpRecordsetPtr->RecordCount;
	}
	return 0;
};
int  AdoDbOper::IRecGetFieldCount()
{
	if (FpRecordsetPtr != 0 && FpRecordsetPtr->State == adStateOpen)
	{
		return FpRecordsetPtr->Fields->Count;
	}
	return 0;
};
//fieldoper
bool  AdoDbOper::BFldIsValid(const char* field_name)
{
	try
	{
		return (FpRecordsetPtr->Fields->Item[field_name]->ActualSize > 0);
	} catch (_com_error e)
	{
		return false;
	} catch (...)
	{
		return false;
	}
};

bool  AdoDbOper::BFldIsValid(int field_index)
{
	if (field_index >= IRecGetFieldCount()) 
		return false;
	try
	{
		return (FpRecordsetPtr->Fields->Item[_variant_t((long)field_index)]->ActualSize > 0);
	} catch (_com_error e)
	{
		return false;
	} catch (...)
	{
		return false;
	}
};

string AdoDbOper::SFldGetBuffer(const char* field_name)
{
	return SFldItemToString(field_name);
};
string AdoDbOper::SFldGetBuffer(int field_index)
{
	return SFldItemToString(field_index);
};

int AdoDbOper::IFldGetLength(int field_index)
{
	try
	{
		return FpRecordsetPtr->Fields->Item[_variant_t((long)field_index)]->ActualSize;
	} catch (_com_error e)
	{		
		return 0;
	} catch (...)
	{
		return 0;
	}
	return 0;
};
int AdoDbOper::IFldGetLength(const char* field_name)
{
	try
	{
		return FpRecordsetPtr->Fields->Item[field_name]->ActualSize;
	} catch (_com_error e)
	{		
		return 0;
	} catch (...)
	{
		return 0;
	}
	return 0;
};

bool AdoDbOper::BFldItemToVariant(const char* field_name, _variant_t& var_ret)
{
	if (!BFldIsValid(field_name)) return false;
	try
	{	
		var_ret = FpRecordsetPtr->GetCollect(field_name);
		if(var_ret.vt == VT_NULL) return false;
	} catch (_com_error e)
	{		
		return false;
	} catch (...)
	{
		return false;
	}
	return true;
};

bool AdoDbOper::BFldItemToVariant(int field_index, _variant_t& var_ret)
{
	if (!BFldIsValid(field_index)) return false;
	try
	{
		var_ret = FpRecordsetPtr->Fields->Item[_variant_t((long)field_index)]->Value;
	} catch (_com_error e)
	{		
		return false;
	} catch (...)
	{		
		return false;
	}
	return true;
};

string AdoDbOper::SFldItemToString(const char* field_name)
{
	_variant_t var;
	if (BFldItemToVariant(field_name, var)) 
		return string(_bstr_t(var));
	return "";
};
string AdoDbOper::SFldItemToString(int field_index)
{
	_variant_t var;
	if (BFldItemToVariant(field_index, var)) 
		return string(_bstr_t(var));
	return "";
};

long AdoDbOper::LFldItemtoInteger(const char* field_name)
{
	_variant_t var;
	if (BFldItemToVariant(field_name, var)) return var;
	return 0;	
};

long AdoDbOper::LFldItemtoInteger(int field_index)
{
	_variant_t var;
	if (BFldItemToVariant(field_index, var)) return var;
	return 0;
};

short AdoDbOper::SFldItemtoSmallInteger(const char* field_name)
{
	_variant_t var;
	if (BFldItemToVariant(field_name, var)) return var;
	return 0;
};
short AdoDbOper::SFldItemtoSmallInteger(int field_index)
{
	_variant_t var;
	if (BFldItemToVariant(field_index, var)) return var;
	return 0;
};

unsigned char AdoDbOper::CFldItemtoTinyInteger(const char* field_name)
{
	_variant_t var;
	if (BFldItemToVariant(field_name, var)) return var;
	return 0;
};
unsigned char AdoDbOper::CFldItemtoTinyInteger(int field_index)
{
	_variant_t var;
	if (BFldItemToVariant(field_index, var)) return var;
	return 0;
};

int    AdoDbOper::IFldItemToBinary(const char* field_name, void* out_buf, int max_len)
{
	_variant_t var;
	if (BFldItemToVariant(field_name, var)) 
	{
		int rec_len = IFldGetLength(field_name);
		int cpy_len = (rec_len < max_len ? rec_len : max_len);
		memcpy(out_buf, (char*)((_bstr_t)var), cpy_len);
		return cpy_len;
	}
	return 0;
};
int    AdoDbOper::IFldItemToBinary(int field_index, void* out_buf, int max_len)
{
	_variant_t var;
	if (BFldItemToVariant(field_index, var)) 
	{
		int rec_len = IFldGetLength(field_index);
		int cpy_len = (rec_len < max_len ? rec_len : max_len);
		memcpy(out_buf, (char*)((_bstr_t)var), cpy_len);
		return cpy_len;
	}
	return 0;
};

bool   AdoDbOper::BFldItemToBool(const char* field_name)
{
	_variant_t var;
	if (BFldItemToVariant(field_name, var)) return (var.boolVal ? true : false);
	return false;
};
bool   AdoDbOper::BFldItemToBool(int field_index)
{
	_variant_t var;
	if (BFldItemToVariant(field_index, var)) return (var.boolVal ? true : false);
	return false;
};

bool AdoDbOper::BFldItemSetString(const char* field_name, const char* str)
{
	try
	{
		FpRecordsetPtr->PutCollect(field_name, str);
		return true;
	} catch (_com_error e)
	{		
		return false;
	} catch (...)
	{
		return false;
	}	
};


bool AdoDbOper::BFldItemSetInteger(const char* field_name, long value)
{
	try
	{
		FpRecordsetPtr->PutCollect(field_name, value);
		return true;
	} catch (_com_error e)
	{		
		return false;
	} catch (...)
	{
		return false;
	}	
};