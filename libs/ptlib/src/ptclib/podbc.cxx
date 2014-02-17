/*
 * podbc.cxx
 *
 * Virteos ODBC Implementation for PWLib Library.
 *
 * Virteos is a Trade Mark of ISVO (Asia) Pte Ltd.
 *
 * Copyright (c) 2004 ISVO (Asia) Pte Ltd. All Rights Reserved.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 *
 * The Original Code is derived from and used in conjunction with the 
 * pwlib Libaray of the OpenH323 Project (www.openh323.org/)
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte Ltd.
 *
 *	 Portions: Simple ODBC Wrapper Article www.codeproject.com
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: podbc.cxx,v $
 * Revision 1.5  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.4  2007/08/25 06:37:39  csoutheren
 * Fixed compilation on 64bit Linux
 *
 * Revision 1.3  2007/04/30 00:07:38  csoutheren
 * Fix problems with PODBC on Windows
 *
 * Revision 1.2  2007/04/25 09:21:20  csoutheren
 * Move unixODBC includes to a seperate namespace to avoid namespace conflicts
 *
 * Revision 1.1  2007/04/13 18:17:57  shorne
 * added ODBC support for linux thx Michal Z
 *
 * Revision 1.2  2006/06/25 11:20:24  csoutheren
 * Add detection of ODBC on Windows
 *
 * Revision 1.1  2006/01/27 06:38:11  shorne
 * Rename ODBC file to lower case
 *
 * Revision 1.3  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.2  2005/08/09 09:08:12  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.1.2.1  2005/04/25 13:09:49  shorne
 * Initial version
 *
 *
*/

#include <ptlib.h>

#ifdef P_ODBC

#include <ptclib/podbc.h>
using namespace PTODBC;

#define new PNEW


#ifdef _MSC_VER
 #pragma warning(disable:4244)
 #pragma warning(disable:4100)
#endif

//// Utilities
//////////////////////////////////////////////////////////////////

static void AddField(PODBC::Field & newField, const PString FieldName, 
							PODBC::FieldTypes odbctype, PODBC::PwType pwtype)
{
  if (newField.Name.GetLength() == 0)   /// If not already set..
  {
	newField.ODBCType = odbctype;
	newField.Name = FieldName;
	newField.Type = pwtype;
  }
}

/// Simple converters
///////////////////////////////////////////////////////////////////////////////////////
/// To Bound Data

static void Convert(long int & data, PString field)
{
	data = field.AsInteger();
}

static void Convert(short int & data, PString field)
{
	data = field.AsInteger();
}

static void Convert(unsigned char * data, PString field)
{
    data = (unsigned char *)(const char *)field;
}


static void Convert(unsigned char & data, PString field)
{
	int f = field.AsInteger();
	data = (unsigned char)f;
}


static void Convert(_int64 & data, PString field)
{
	data = field.AsInt64();
}

static void Convert(double & data, PString field,int Precision = PODBCRecord::Precision)
{
	/// Reformat to the Required Decimal places
	data = PString(PString::Decimal,field.AsReal(),Precision).AsReal();
}

static void Convert(DATE_STRUCT & data, PString field)
{
	PTime t = PTime(field);

	data.day = t.GetDay();
	data.month = t.GetMonth();
	data.year = t.GetYear();
}

static void Convert(TIME_STRUCT & data, PString field)
{
	PTime t = PTime(field);

	data.second = t.GetSecond();
	data.minute = t.GetMinute();
	data.hour = t.GetHour();
}
			   		
static void Convert(TIMESTAMP_STRUCT & data, PString field)
{
	PTime t = PTime(field);

	data.day = t.GetDay();
	data.month = t.GetMonth();
	data.year = t.GetYear();
	data.second = t.GetSecond();
	data.minute = t.GetMinute();
	data.hour = t.GetHour();
}

static void Convert(SQLGUID & data, PString field)
{
// Yet To Be Implemented.
	field = PString();
}


/// To PString
///////////////////////////////////////////////////////////////////////////////////////
template <typename SQLField>
static PString Convert(SQLField field)
{
	return PString(field);
}

static PString Convert(double field,int Precision = PODBCRecord::Precision)
{
	return PString(PString::Decimal,field,Precision);
}

static PString Convert(unsigned char * field)
{
	return PString(*field);
}

static PString Convert(DATE_STRUCT date)
{
	return PTime(0,0,0,date.day,date.month,date.year).AsString(PODBCRecord::TimeFormat);
}

static PString Convert(TIME_STRUCT time)
{
	return PTime(time.second,time.minute,time.hour,0,0,0).AsString(PODBCRecord::TimeFormat);
}
			   		
static PString Convert(TIMESTAMP_STRUCT	timestamp)
{
	return PTime(timestamp.second,timestamp.minute,
		timestamp.hour,timestamp.day,timestamp.month,timestamp.year).AsString(PODBCRecord::TimeFormat);
}

static PString Convert(SQLGUID guid)
{
// To Be Implemented.
	return PString();
}


///PODBC
/////////////////////////////////////////////////////////////////


PODBC::PODBC()
{
	m_hDBC              = NULL;
	m_hEnv              = NULL;
	m_nReturn           = SQL_ERROR;
}

PODBC::~PODBC()
{
   if( m_hDBC != NULL ) {
    m_nReturn = SQLFreeHandle( SQL_HANDLE_DBC,  m_hDBC );
   }
   if( m_hEnv!=NULL )
    m_nReturn = SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
}


BOOL PODBC::Connect(PTODBC::LPCTSTR svSource)
{
   int nConnect = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv );
   if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
    nConnect = SQLSetEnvAttr( m_hEnv, 
                      SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 );
    if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
     nConnect = SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hDBC );
     if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
      SQLSetConnectOption( m_hDBC,SQL_LOGIN_TIMEOUT,5 );                
      short shortResult = 0;
      SQLTCHAR szOutConnectString[ 1024 ];
      nConnect = SQLDriverConnect( m_hDBC,	// Connection Handle
          NULL,                           // Window Handle
          (SQLTCHAR*)svSource,			  // InConnectionString
#ifdef _WIN32
          _tcslen(svSource),              // StringLength1
#else
          strlen(svSource),
#endif
          szOutConnectString,             // OutConnectionString
          sizeof( szOutConnectString ),   // Buffer length
          &shortResult,                   // StringLength2Ptr
          SQL_DRIVER_NOPROMPT );          // no User prompt
      return ((nConnect == SQL_SUCCESS) || (nConnect == SQL_SUCCESS_WITH_INFO));
     }
    }
   }
   if( m_hDBC != NULL ) {
    m_nReturn = SQLDisconnect( m_hDBC );
    m_nReturn = SQLFreeHandle( SQL_HANDLE_DBC,  m_hDBC );
   }
   if( m_hEnv!=NULL )
    m_nReturn = SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
   m_hDBC              = NULL;
   m_hEnv              = NULL;
   m_nReturn           = SQL_ERROR;
   return ((m_nReturn == SQL_SUCCESS) || (m_nReturn == SQL_SUCCESS_WITH_INFO));
}


void PODBC::Disconnect()
  {
   if( m_hDBC != NULL )
   {
    m_nReturn = SQLDisconnect( m_hDBC );
    m_hDBC = NULL;
   }
}


//--

BOOL PODBC::Connect_MSSQL(PString User,PString Pass, 
         PString Host,BOOL Trusted, 
         MSSQLProtocols Proto)
{  
	PString Network = PString();

        switch(Proto) {
	   case MSSQLNamedPipes:
		Network ="dbnmpntw";
		break;
	   case MSSQLWinSock:
		Network ="dbmssocn";
		break;
	   case MSSQLIPX:
		Network = "dbmsspxn";
		break;
	   case MSSQLBanyan:
		Network = "dbmsvinn";
		break;
	   case MSSQLRPC:
		Network = "dbmsrpcn";
		break;
	   default:
		Network = "dbmssocn";
		break;
	 }

	PString ConStr = "Driver={SQL Server};Server=" + Host + ";Uid=" + User + ";Pwd=" +
		 Pass +";Trusted_Connection=" + (Trusted ? "Yes" : "No") + ";Network=" + Network + ";";

   return PODBC::Connect(ConStr);
}


 //--
BOOL PODBC::Connect_DB2(PFilePath DBPath)
{
   PString ConStr ="Driver={Microsoft dBASE Driver (*.dbf)};DriverID=277;Dbq=" + DBPath + ";";
   return PODBC::Connect(ConStr);
}

 //--
BOOL PODBC::Connect_XLS(PFilePath XLSPath,PString DefDir)
{
   PString ConStr = "Driver={Microsoft Excel Driver (*.xls)};DriverId=790;bq="+ XLSPath 
						+ ";DefaultDir=" + DefDir + ";";
   return PODBC::Connect(ConStr);
}

 //--
BOOL PODBC::Connect_TXT(PFilePath TXTPath)
{
   PString ConStr = "Driver={Microsoft Text Driver (*.txt; *.csv)};Dbq="+ TXTPath 
						+ ";Extensions=asc,csv,tab,txt;";

   return PODBC::Connect(ConStr);
}

 //--
BOOL PODBC::Connect_FOX(PFilePath DBPath,PString User,
          PString Pass,PString Type,
          BOOL Exclusive)
{
   PString ConStr = "Driver={Microsoft Visual Foxpro Driver};Uid=" + User + ";Pwd=" + Pass 
				+";SourceDB=" + DBPath + ";SourceType=" + Type + ";Exclusive=" + (Exclusive ? "yes": "no") + ";";
   
   return PODBC::Connect(ConStr);
}

 //--
BOOL PODBC::Connect_MDB(PFilePath MDBPath,PString User,
            PString Pass,BOOL Exclusive)
{
   PString ConStr = "Driver={Microsoft Access Driver (*.mdb)};Dbq=" + MDBPath + ";Uid=" + User +
	   ";Pwd=" + Pass + ";Exclusive=" + (Exclusive ? "yes" : "no");

   return PODBC::Connect(ConStr);
}

 //--
BOOL PODBC::Connect_PDOX(PDirectory DBPath,PDirectory DefaultDir,int version)
{
   PString driver = "3.X";
  if (version == 4) 
    driver = "4.X";
  if (version > 4) 
    driver = "5.X";

   PString ConStr = "Driver={Microsoft Paradox Driver (*.db )};DriverID=538;Fil=Paradox " + 
						 driver + ";DefaultDir=" + DefaultDir +
						 "\\;Dbq=" + DBPath + "\\;CollatingSequence=ASCII;";

   return PODBC::Connect(ConStr);
}

 //--
BOOL PODBC::Connect_DBASE(PDirectory DBPath)
{
   PString ConStr = "Driver={Microsoft dBASE Driver (*.dbf)};DriverID=277;Dbq=" + DBPath + ";";
   return PODBC::Connect(ConStr);
}
 //--
BOOL PODBC::Connect_Oracle(PString Server,PString User, PString Pass)
{
  PString ConStr = "Driver={Microsoft ODBC for Oracle};Server=" + Server + 
					";Uid=" + User + ";Pwd=" + Pass + ";";

  return PODBC::Connect(ConStr);
}

 //--
BOOL PODBC::Connect_mySQL(PString User,PString Pass,PString Host, int Port,int Option)
{
   PString ConStr = "Driver={MySQL ODBC 3.51 Driver};Uid=" + User + ";Pwd=" + Pass
				 + ";Server=" + Host + ";Port=" + PString(Port) + ";";

   return PODBC::Connect(ConStr);
}

BOOL PODBC::ConnectDB_mySQL(PString DB,PString User,PString Pass,PString Host, int Port,int Option)
{
   PString ConStr = "Driver={MySQL ODBC 3.51 Driver};Database=" + DB + ";Uid=" + User
					+ ";Pwd=" + Pass +";Server=" + Host + ";Port=" + PString(Port) + ";";

   return PODBC::Connect(ConStr);
}

BOOL PODBC::Connect_postgreSQL(PString DB,PString User,
        PString Pass,PString Host, int Port,int Option)
{
   PString ConStr = "Driver={PostgreSQL};Database=" + DB + ";Uid=" + User
					+ ";Pwd=" + Pass +";Server=" + Host + ";Port=" + PString(Port) + ";";

   return PODBC::Connect(ConStr);
}

 //--
BOOL PODBC::DataSource(DataSources Source, ConnectData Data)
{
   dbase = Source;

   switch (Source)
   {
     case PODBC::mySQL:
       if (Data.Host.GetLength() == 0) 
         Data.Host = "localhost";
       if (Data.Port == 0) 
         Data.Port = 3306;
       if (Data.DefDir.GetLength() == 0) {
         return Connect_mySQL(Data.User,Data.Pass,Data.Host,Data.Port,Data.opt);
        } else {
          return ConnectDB_mySQL(Data.DefDir,Data.User,Data.Pass,Data.Host,Data.Port,Data.opt);
        }
     case PODBC::MSSQL:
       if (Data.Host.GetLength() == 0) 
         Data.Host = "(local)";
         return Connect_MSSQL(Data.User,Data.Pass,Data.Host,Data.Excl_Trust, (MSSQLProtocols)Data.opt);
     case PODBC::Oracle:
         return Connect_Oracle(Data.Host,Data.User, Data.Pass);
     case PODBC::IBM_DB2:
         return Connect_DB2(Data.DBPath);
     case PODBC::DBASE:
         return Connect_DBASE(Data.DBPath);
     case PODBC::Paradox:
         return Connect_PDOX(Data.DBPath,Data.DefDir,Data.opt);
     case PODBC::Excel:
         return Connect_XLS(Data.DBPath,Data.DefDir);
     case PODBC::Ascii:
         return Connect_TXT(Data.DBPath);
     case PODBC::Foxpro:
         return Connect_FOX(Data.DBPath,Data.User,Data.Pass,"DBF",Data.Excl_Trust);
     case PODBC::MSAccess:
         return Connect_MDB(Data.DBPath,Data.User,Data.Pass,Data.Excl_Trust);
     case PODBC::postgreSQL:
         return Connect_postgreSQL(Data.User,Data.Pass,Data.Host,Data.Port,Data.opt);
   };

	return FALSE;
}

PStringArray PODBC::TableList(PString option)
{
    PODBCStmt data(this);
	return data.TableList(option);
}

PODBC::Table PODBC::LoadTable(PString table)
{
  PODBC::Table newTable(this,table);
  return newTable;
}

BOOL PODBC::Query(PString Query)
{
  if (m_hDBC == NULL)
	  return FALSE;

   PODBCStmt stmt(this);
	return stmt.Query(Query);
}

void PODBC::SetPrecision(int Digit)
{
	PODBCRecord::Precision = Digit;
}

void PODBC::SetTimeFormat(PTime::TimeFormat tformat)
{
	PODBCRecord::TimeFormat = tformat;
}

BOOL PODBC::NeedLongDataLen()
{
  
  PString  f;

  SQLGetInfo(m_hDBC,SQL_NEED_LONG_DATA_LEN,
   f.GetPointer(2),2, NULL);

	if (f == "N")   
		  return FALSE;
	else                                 
		  return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// PODBC::Field

PString PODBC::Field::AsString()
{
Bind & b = Data;	   /// Bound Buffer container		
BOOL B = isReadOnly;   /// ReadOnly Columns are not Buffer Bound and have to get Data;
SQLINTEGER len = MAX_DATA_LEN;   

     switch (ODBCType) {
      case PODBC::BigInt:
        if (B) 
          SQLGetData(*row->rec->Stmt,col,ODBCType,&b.sbint,0,&b.dataLen);
          return Convert(b.sbint);

      case PODBC::TinyInt:
      case PODBC::Bit:
        if (B) 
          SQLGetData(*row->rec->Stmt,col,ODBCType,&b.sbit,0,&b.dataLen);
          return Convert(b.sbit);				
	
      case PODBC::Char:
        if (B) 
          SQLGetData(*row->rec->Stmt,col,ODBCType,&b.suchar,0,&b.dataLen);
          return Convert(b.suchar);		

      case PODBC::Integer:
        if (B) 
          SQLGetData(*row->rec->Stmt,col,ODBCType,&b.slint,0,&b.dataLen);
          return Convert(b.slint);
				
      case PODBC::SmallInt:
        if (B) 
          SQLGetData(*row->rec->Stmt,col,ODBCType,&b.ssint,0,&b.dataLen);
          return Convert(b.ssint);

      case PODBC::Numeric:
      case PODBC::Decimal:
      case PODBC::Float:
      case PODBC::Real:
      case PODBC::Double:
        if (B) 
          SQLGetData(*row->rec->Stmt,col,ODBCType,&b.sdoub,0,&b.dataLen);
          return Convert(b.sdoub,Decimals);

      /// Date Times
      case PODBC::Date: 
        if (B) 
          SQLGetData(*row->rec->Stmt,col,ODBCType,&b.date,0,&b.dataLen);
          return Convert(b.date);

      case PODBC::Time:
        if (B) 
          SQLGetData(*row->rec->Stmt,col,ODBCType,&b.time,0,&b.dataLen);
          return Convert(b.time);

      case PODBC::TimeStamp:
         if (B) 
           SQLGetData(*row->rec->Stmt,col,ODBCType,&b.timestamp,0,&b.dataLen);
           return Convert(b.timestamp);

      case PODBC::Guid:   
         if (B) 
           SQLGetData(*row->rec->Stmt,col,ODBCType,&b.guid,0,&b.dataLen);
           return Convert(b.guid);

      /// Binary/Long Data Cannot be Bound Need to get it!
      case PODBC::Binary:
      case PODBC::VarBinary:
      case PODBC::LongVarBinary:
      case PODBC::LongVarChar:		
        return row->rec->GetLongData(col);	
			
      /// Character 
      case PODBC::DateTime:
      case PODBC::Unknown:
      case PODBC::VarChar:
      default:
        if (B) 
          SQLGetData(*row->rec->Stmt,col,ODBCType,b.sbin.GetPointer(len),len,&b.dataLen);
          return b.sbin;
  }
}

void PODBC::Field::SetValue(PString value)
{
 
    Bind &b = Data;

   /// If Field is marked as ReadOnly Do not Update
   if (isReadOnly) 
        return;

   /// If Field is not Nullable and the New Value length =0 No Update.
   if ((!isNullable) && (value.GetLength() == 0)) 
    return;

	switch (ODBCType) {
		case PODBC::BigInt:
			Convert(b.sbint,value);
			break;

		case PODBC::TinyInt:
		case PODBC::Bit:
			 Convert(b.sbit,value);	
			 break;
	
		case PODBC::Char:
			 Convert(b.suchar,value);
			 break;

		case PODBC::Integer:
			 Convert(b.slint,value);
			 break;
				
		case PODBC::SmallInt:
			 Convert(b.ssint,value);
			 break;

		case PODBC::Numeric:
		case PODBC::Decimal:
		case PODBC::Float:
		case PODBC::Real:
		case PODBC::Double:
			 Convert(b.sdoub,value);
			 break;

	  /// Date Times
		case PODBC::Date:  
			 Convert(b.date,value);
			 break;

		case PODBC::Time:
			 Convert(b.time,value);
			 break;

		case PODBC::TimeStamp:
			 Convert(b.timestamp,value);
			 break;

		case PODBC::Guid:   
			 Convert(b.guid,value);

	  /// Binary Data
		case PODBC::Binary:
		case PODBC::VarBinary:
		case PODBC::LongVarBinary:
		case PODBC::LongVarChar:
			b.sbinlong = value;	
				if (row->rec->dbase == PODBC::MSAccess) {
					b.sbinlong.SetSize(MAX_DATA_LEN * PODBCRecord::MaxCharSize);
					b.dataLen = b.sbinlong.GetLength();
				}
			break;
			
	  /// Character 
		case PODBC::DateTime:
		case PODBC::Unknown:
		case PODBC::VarChar:
		default:
			b.sbin = value;
			b.sbin.SetSize(MAX_DATA_LEN);
			b.dataLen = b.sbin.GetLength();
	};
}

void PODBC::Field::SetDefaultValues() 
{
  if ((isReadOnly) || (isAutoInc))  
    return;
 
  /// Set Default Values
  switch (Type) {
    case oPString:	
    case ochar:	
     if (isNullable)
       SetValue(PString());
     else
       SetValue("?");
     break;
    case oPTime:
      SetValue(PTime().AsString());
      break;
    default:
      SetValue(0);
  } 
}

PString PODBC::Field::operator=(const PString & str)
{ 
	return AsString();
}


BOOL PODBC::Field::DataFragment(PString & Buffer,PINDEX & fragment, PTODBC::SQLINTEGER & size)
{
  PINDEX fragcount = PINDEX(Data.sbinlong.GetLength()/size);
// Value less than Buffer Size
  if (fragcount == 0) {
	  Buffer = Data.sbinlong;
	  return FALSE;
  }
// Buffer Fragment
  if (fragcount < fragment) {
	  Buffer.Splice(Data.sbinlong,(fragment * size)+1,size);
	  fragment = fragment++;
	  return TRUE;
  }
// Last Fragment
  PINDEX blen = Data.sbinlong.GetLength() - (fragment * size);
  Buffer = Data.sbinlong.Right(blen);
  size = blen;
  return FALSE; 
}

BOOL PODBC::Field::Post() 
{
	return row->Post();
}

/////////////////////////////////////////////////////////////////////////////
// PODBC::Table
	  
PODBC::Table::Table(PODBC * odbc, PString Query)
	:	stmt(PODBCStmt(odbc))
{
   PString query;
    tableName = PString();

// Do the Query
    if (Query.Trim().Left(6) == "SELECT") {		// Select Query
      query = Query;
    } else {									// Table Query
      tableName = Query;
      query = "SELECT * FROM [" + tableName + "];";
    }

    if (!stmt.Query(query))
      return;

    // Create the Row Handler
    RowHandler = new Row(&stmt);
}

PODBC::Table::~Table() 
{
}
     	  
PODBC::Row PODBC::Table::NewRow() 
{
      RowHandler->SetNewRow();
      return *RowHandler;
}

BOOL PODBC::Table::DeleteRow(PINDEX row)
{
      return RowHandler->Delete(row);
}


BOOL PODBC::Table::Post()
{
      return RowHandler->Post();
}

PINDEX PODBC::Table::Rows()  
{ 
      return RowHandler->RowCount; 
}

PINDEX PODBC::Table::Columns() 
{ 
     return RowHandler->Columns(); 
}
	  
PStringArray PODBC::Table::ColumnNames() 
{ 
     return RowHandler->ColumnNames(); 
}

PODBC::Row & PODBC::Table::operator[] (PINDEX row) 
{ 
   if (RowHandler->Navigate(row))
     return *RowHandler; 

   return *RowHandler;
}

PODBC::Row & PODBC::Table::RecordHandler()
{
     return *RowHandler;
}

PODBC::Field & PODBC::Table::operator() (PINDEX row, PINDEX col) 
{ 

     RowHandler->Navigate(row);
     return RowHandler->Column(col); 
}

PODBC::Field & PODBC::Table::Column(PINDEX col)
{
     return RowHandler->Column(col);
}

PODBC::Field & PODBC::Table::Column(PString Name)
{
	return RowHandler->Column(Name);
}
///////////////////////////////////////////////////////////////////
// PODBC::Row

PODBC::Row::Row(PODBCStmt * stmt)
	: rec(new PODBCRecord(stmt))	
{
  /// Create a blank Row (RecordHolder) with default Recordset Info.
	for (PINDEX i=0; i < rec->ColumnCount(); i++)
	{   
          Field * nfield = new Field;
          nfield->row = this;
          nfield->col = i+1;

	 // Default Attributes
         nfield->isReadOnly = FALSE;
         nfield->isNullable = TRUE;
         nfield->isAutoInc = FALSE;
         nfield->Decimals = 0;
         nfield->LongData = stmt->odbclink->NeedLongDataLen();

         rec->Data(i+1, *nfield);
	    		
        /// Append to PArray
        Fields.Append(nfield);
       }

   
   NewRow = FALSE;

// Attempt to get the first Record
  if (!stmt->FetchFirst())   
    RowCount = 0; 
  else	  
    RowCount = 1;

// Get the Record Count  (Need a better way other than fetching! :-( )
   while(stmt->Fetch())
	RowCount++;
	 
  /// Put the RecordHolder to point to the First Row
  if (RowCount > 0) {
   stmt->FetchFirst();
   CurRow = 1;
  } else 
   CurRow = 0;
}


PODBC::Field & PODBC::Row::Column(PINDEX col) 
{ 
  /// Column = 0 return blank field
  if ((col == 0) || (col > Fields.GetSize()))    
      return *(new PODBC::Field());

   return Fields[col-1];
}

PODBC::Field & PODBC::Row::Column(PString name)
{
    PINDEX i = rec->ColumnByName(name);
    return Column(i);
}

PINDEX PODBC::Row::Columns()
{
    return rec->ColumnCount();
}

PStringArray PODBC::Row::ColumnNames()
{
    PStringArray Names;

    for (PINDEX i = 0; i < Fields.GetSize(); i++)
    {
      Names.AppendString(rec->ColumnName(i+1));
    }
    return Names;
}

PINDEX PODBC::Row::Rows()
{
   return RowCount;
}


PODBC::Field & PODBC::Row::operator[] (PINDEX col) 
{  
    return Column(col);
} 

PODBC::Field & PODBC::Row::operator[] (PString col) 
{  
     PINDEX i = rec->ColumnByName(col);
     return Column(i);
}

void PODBC::Row::SetNewRow()
{
  CurRow = RowCount+1;
  NewRow = TRUE;

  for (PINDEX i = 0; i < Fields.GetSize(); i++)
  {
     Column(i).SetDefaultValues();
  }
}

BOOL PODBC::Row::Navigate(PINDEX row)
{
   if ((row > 0) && (CurRow != row)) {
     if (!rec->Stmt->FetchRow(row,TRUE)) 
	return FALSE;

     CurRow = row;
   } 
  return TRUE;
}

BOOL PODBC::Row::Post()
{
  BOOL Success;

  if (NewRow) {
    NewRow = FALSE;
    RowCount++;
    CurRow = RowCount;
    Success = rec->PostNew(*this);
   } else {
    Success = rec->PostUpdate(*this);
   }

   return Success;
}



BOOL PODBC::Row::Delete(PINDEX row)
{
    if (row > 0) {
      if (!Navigate(row))
        return FALSE;
      }

    if (!rec->PostDelete())
     return FALSE;

    RowCount--;
    return TRUE;
}


// PDSNConnection
/////////////////////////////////////////////////////////////////////////////////////

PDSNConnection::PDSNConnection()
{

}

PDSNConnection::~PDSNConnection()
{

}


BOOL PDSNConnection::Connect( PString Source ,PString Username, PString Password)
{
   int nConnect = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv );

    if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
     nConnect = SQLSetEnvAttr( m_hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 );

      if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
        nConnect = SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hDBC );

	if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
	  SQLSetConnectOption( m_hDBC,SQL_LOGIN_TIMEOUT,5 );                
	  nConnect=SQLConnect( m_hDBC,
		 ( SQLTCHAR *)(const char *)Source,SQL_NTS, 
		 ( SQLTCHAR *)(const char *)Username,
		 SQL_NTS,
		 ( SQLTCHAR *)(const char *)Password,
		 SQL_NTS 
               );
	  return ((nConnect == SQL_SUCCESS) || (nConnect == SQL_SUCCESS_WITH_INFO));
	}
      }
    }

   if( m_hDBC != NULL ) {
    m_nReturn = SQLDisconnect( m_hDBC );
    m_nReturn = SQLFreeHandle( SQL_HANDLE_DBC,  m_hDBC );
   }
   if( m_hEnv!=NULL )
    m_nReturn = SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );

   m_hDBC              = NULL;
   m_hEnv              = NULL;
   m_nReturn           = SQL_ERROR;

   return FALSE;
}
 //--
PODBCStmt::PODBCStmt(PODBC * odbc)
	: odbclink(odbc)
{
   HDBC hDBCLink = *odbc;

   SQLRETURN m_nReturn;
   m_nReturn = SQLAllocHandle( SQL_HANDLE_STMT, hDBCLink, &m_hStmt );
   SQLSetStmtAttr(m_hStmt, SQL_ATTR_CONCURRENCY, 
                       (SQLPOINTER) SQL_CONCUR_ROWVER, 0);
   SQLSetStmtAttr(m_hStmt, SQL_ATTR_CURSOR_TYPE,
                       (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, 0);
   SQLSetStmtAttr(m_hStmt, SQL_ATTR_ROW_BIND_TYPE, 
					   (SQLPOINTER)SQL_BIND_BY_COLUMN, 0);
   SQLSetStmtAttr(m_hStmt, SQL_ATTR_ROW_ARRAY_SIZE, 
					   (SQLPOINTER)1, 0);
   SQLSetStmtAttr(m_hStmt, SQL_ATTR_ROW_STATUS_PTR, 
					   NULL, 0);

   dbase = odbc->dbase;

   if(!SQL_OK(m_nReturn))
    m_hStmt=SQL_NULL_HSTMT;
}


PODBCStmt::~PODBCStmt()
{
   SQLCloseCursor(m_hStmt);

   if(m_hStmt!=SQL_NULL_HSTMT)
    SQLFreeHandle(SQL_HANDLE_STMT,m_hStmt);
}

BOOL PODBCStmt::IsValid()
{
    return m_hStmt!=SQL_NULL_HSTMT;
}


PTODBC::DWORD PODBCStmt::GetChangedRowCount(void)
{
   SQLINTEGER nRows=0;
   if(!SQL_OK(SQLRowCount(m_hStmt,&nRows)))
    return 0;
   return nRows;
}

BOOL PODBCStmt::Query(PString strSQL)
{
   SQLRETURN nRet=SQLExecDirect( m_hStmt, (SQLTCHAR *)(const char *)strSQL, SQL_NTS );
    return SQL_OK( nRet );
}

BOOL PODBCStmt::Fetch()
{
   return SQL_OK(SQLFetch(m_hStmt));
}

BOOL PODBCStmt::FetchPrevious()
{
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,SQL_FETCH_PRIOR,0);
   return SQL_OK(nRet);
}

BOOL PODBCStmt::FetchNext()
{
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,SQL_FETCH_NEXT,0);
   return SQL_OK(nRet);
}

BOOL PODBCStmt::FetchRow(PINDEX nRow,BOOL Absolute)
{
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,
      (Absolute ? SQL_FETCH_ABSOLUTE : SQL_FETCH_RELATIVE),nRow);
   return SQL_OK(nRet);
}

BOOL PODBCStmt::FetchFirst()
{
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,SQL_FETCH_FIRST,0);
   return SQL_OK(nRet);
}

BOOL PODBCStmt::FetchLast()
{
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,SQL_FETCH_LAST,0);
   return SQL_OK(nRet);
}

BOOL PODBCStmt::Cancel()
{
   SQLRETURN nRet=SQLCancel(m_hStmt);
   return SQL_OK(nRet);
}

PStringArray PODBCStmt::TableList(PString option)
{
    PString list;
    PString entry;
    SQLINTEGER len = 129;
    SQLINTEGER cb = 0;
    SQLRETURN nRet;

/// This Statement will need reviewing as it
/// depends on the Database, Might work on some
/// but not on others

      nRet = SQLTables(m_hStmt, 0, SQL_NTS, 0, SQL_NTS,0, SQL_NTS,(unsigned char *)(const char *)option,option.GetLength());

	if (SQL_OK(nRet)) {
		SQLBindCol(m_hStmt, 3, SQL_C_CHAR, entry.GetPointer(len),129,&cb);

		while (Fetch())
		{
			SQLGetData(m_hStmt, 3, SQL_C_CHAR, entry.GetPointer(len), 129, &cb);
				if (entry.GetLength() > 0) {
					list = list + ":" + entry;
					entry.MakeEmpty();
				}
		}
	}

	return list.Tokenise(":",FALSE);
}

BOOL PODBCStmt::SQL_OK(SQLRETURN res) 
{
	if ((res==SQL_SUCCESS_WITH_INFO) || (res==SQL_SUCCESS))
		  return TRUE;

	if (res != SQL_NEED_DATA)	
			GetLastError();

	return FALSE;
}

void PODBCStmt::GetLastError()
{

  SQLRETURN   rc;
  SQLINTEGER    NativeError;
  int buflen = SQL_MAX_MESSAGE_LENGTH;
  SQLSMALLINT MsgLen, i = 1;
  PString ErrStr, Msg;

   while ((rc = SQLGetDiagRec(SQL_HANDLE_STMT, m_hStmt, i, (unsigned char *)ErrStr.GetPointer(6), 
	      &NativeError,(unsigned char *)Msg.GetPointer(buflen), buflen, &MsgLen)) != SQL_NO_DATA) 
   {
	     odbclink->OnSQLError(ErrStr, Msg);
      i++;
   }
}


//////////////////////////////////////////////////////////////////////////////
/// PODBCRecord

unsigned int PODBCRecord::Precision;
int PODBCRecord::MaxCharSize;
PTime::TimeFormat PODBCRecord::TimeFormat;

 //--
PODBCRecord::PODBCRecord(PODBCStmt * hStmt)
	: Stmt(hStmt)
{
	m_hStmt=*hStmt;
	dbase = (PODBC::DataSources)hStmt->dbase;  // Database name
	if (!Precision) 
          Precision = 4;
	if (!TimeFormat) 
          TimeFormat = PTime::RFC1123;
	MaxCharSize = 56;  //56 Kbytes (Stupid MSAccess)
}

PINDEX PODBCRecord::ColumnCount()
{
   short nCols=0;
   if(!Stmt->SQL_OK(SQLNumResultCols(m_hStmt,&nCols)))
    return 0;
   return nCols;
}


BOOL PODBCRecord::InternalBindColumn(::USHORT Column,PTODBC::LPVOID pBuffer,
        PTODBC::ULONG pBufferSize,PTODBC::LONG * pReturnedBufferSize,
        PTODBC::USHORT nType)
{
   SQLINTEGER pReturnedSize=0;

   SQLRETURN Ret=SQLBindCol(m_hStmt,Column,nType,
               pBuffer,pBufferSize,&pReturnedSize);
   if(*pReturnedBufferSize)
    *pReturnedBufferSize=pReturnedSize;
cout << pReturnedSize;
   return Stmt->SQL_OK(Ret);
}

PINDEX PODBCRecord::ColumnByName(PString Column)
{
   PINDEX nCols=ColumnCount();
   for(PINDEX i=1;i<(nCols+1);i++)
   {
    if(Column == ColumnName(i))
      return i;
   }
   return 0;
}

BOOL PODBCRecord::InternalGetData(PTODBC::USHORT Column, PTODBC::LPVOID pBuffer, 
    PTODBC::ULONG pBufLen, PTODBC::SQLINTEGER * dataLen, int Type)
{
   SQLINTEGER od=0;
   int Err=SQLGetData(m_hStmt,Column,Type,pBuffer,pBufLen,&od);

   if (!Stmt->SQL_OK(Err))
		return FALSE;
    
   if(dataLen)
    *dataLen=od;
   return TRUE;
}

PString PODBCRecord::GetLongData(PINDEX Column)
{
   PString sbin;
   PString Data;
   SQLINTEGER len = MAX_DATA_LEN;
   SQLINTEGER cb =0;

    while (InternalGetData((PTODBC::USHORT)Column,sbin.GetPointer(len + 1),len,&cb))
	{
		if (sbin.Right(1) == '\0')			// Remove Null Char
			Data = Data + sbin.Left(sbin.GetLength()-1);
		else
			Data = Data + sbin;
	}

	return Data;
}

void PODBCRecord::Data(PINDEX Column, PODBC::Field & field)
{
   SQLINTEGER len = MAX_DATA_LEN;
//   SQLINTEGER cb =0;
   BOOL B = TRUE;    /// Bind Switch (Readonly Fields are not Bound

   PODBC::Field::Bind & b = field.Data;
   PODBC::FieldTypes ctype = ColumnType(Column);

 /// Set the Attributes
   field.isReadOnly = !IsColumnUpdatable(Column);
   field.isNullable	= IsColumnNullable(Column);
   field.isAutoInc = IsColumnAutoIndex(Column);

 /// Mark Columns which ReadOnly to Be Ignored when Binding.
   if (field.isReadOnly) B = FALSE;

   switch (ctype) {

    /// Numeric Data
    case PODBC::BigInt:
       if (B) 
        SQLBindCol(m_hStmt, Column,SQL_C_SBIGINT, &b.sbint,0,&b.dataLen);
        AddField(field,ColumnName(Column),ctype, PODBC::oPInt64);
        break;

     case PODBC::TinyInt:
       if (B) 
        SQLBindCol(m_hStmt, Column, SQL_C_UTINYINT, &b.sbit, 0, &b.dataLen);
        AddField(field,ColumnName(Column),ctype, PODBC::oshort);
        break;
				
     case PODBC::Bit:
       if (B) 
        SQLBindCol(m_hStmt, Column, SQL_C_BIT, &b.sbit, 0, &b.dataLen);
        AddField(field,ColumnName(Column),ctype, PODBC::oBOOL);
        break;
	
     case PODBC::Char:
       if (B) 
        SQLBindCol(m_hStmt, Column, SQL_C_CHAR, &b.suchar, 0, &b.dataLen);
        AddField(field,ColumnName(Column),ctype,PODBC::ochar);
        break;

     case PODBC::Integer:
       if (B) 
        SQLBindCol(m_hStmt, Column, SQL_C_LONG, &b.slint, 0, &b.dataLen);
        AddField(field,ColumnName(Column),ctype,PODBC::olong);
        break;
				
     case PODBC::SmallInt:
       if (B) 
        SQLBindCol(m_hStmt, Column, SQL_C_SSHORT, &b.ssint, 0, &b.dataLen);
        AddField(field,ColumnName(Column),ctype, PODBC::oint);
        break;

     case PODBC::Numeric:
     case PODBC::Decimal:
     case PODBC::Float:
     case PODBC::Real:
     case PODBC::Double:
       field.Decimals = ColumnPrecision(Column);
       if (B) 
        SQLBindCol(m_hStmt, Column, SQL_C_DOUBLE, &b.sdoub, 0, &b.dataLen);
       AddField(field,ColumnName(Column),ctype,PODBC::odouble);
       break;

     // Data Structures
     case PODBC::Date:  
       if (B) 
        SQLBindCol(m_hStmt, Column, SQL_C_TYPE_DATE, &b.date, 0, &b.dataLen);
       AddField(field,ColumnName(Column),ctype,PODBC::oPTime);
       break;

     case PODBC::Time:
       if (B) 
        SQLBindCol(m_hStmt, Column, SQL_C_TYPE_TIME, &b.time, 0, &b.dataLen);
       AddField(field,ColumnName(Column),ctype,PODBC::oPTime);
       break;

     case PODBC::TimeStamp:
       if (B) 
         SQLBindCol(m_hStmt, Column, SQL_C_TYPE_TIMESTAMP, &b.timestamp, 0, &b.dataLen);
       AddField(field,ColumnName(Column),ctype,PODBC::oPTime);
       break;

     case PODBC::Guid:   
       if (B) 
        SQLBindCol(m_hStmt, Column, SQL_C_GUID, &b.guid, 0, &b.dataLen);
       AddField(field,ColumnName(Column),ctype,PODBC::oPGUID);
       break;

      /// Binary Data
      case PODBC::Binary:
      case PODBC::VarBinary:
      case PODBC::LongVarBinary:
      case PODBC::LongVarChar:
        if (dbase == PODBC::MSAccess)   /// Stupid Access Stuff!
          if (B) 
            SQLBindCol(m_hStmt, field.col, SQL_C_CHAR, b.sbinlong.GetPointer(len*MaxCharSize), len*MaxCharSize, &b.dataLen);
         else {
            if (B) 
              SQLBindCol(m_hStmt, Column, SQL_LONGVARCHAR, (SQLPOINTER)Column, 0, &b.dataLen);
            if (field.LongData) 
               b.dataLen = SQL_LEN_DATA_AT_EXEC(0);
            else
               b.dataLen = SQL_LEN_DATA_AT_EXEC(len);
            } 	
	    AddField(field,ColumnName(Column),ctype,PODBC::oPString);
            break;
			
      /// Character 
      case PODBC::DateTime:
      case PODBC::Unknown:
      case PODBC::VarChar:
      default:
        b.sbin.SetSize(len+1);
        AddField(field,ColumnName(Column),ctype,PODBC::oPString);
        if (B) 
         SQLBindCol(m_hStmt, Column, SQL_C_CHAR, b.sbin.GetPointer(len), len, &b.dataLen);
        break;
   };
}


BOOL PODBCRecord::PostNew(PODBC::Row & rec)
{
   SQLRETURN nRet;

   nRet = SQLBulkOperations(m_hStmt,			// Statement handle
			SQL_ADD								// Operation
   );

   return InternalSaveLongData(nRet,rec);
}


BOOL PODBCRecord::PostUpdate(PODBC::Row & rec)
{
   SQLRETURN nRet;

   nRet = SQLSetPos(m_hStmt,			// Statement handle
	   (unsigned short)1,				// RowNumber
	   SQL_UPDATE,						// Operation
	   SQL_LOCK_NO_CHANGE				// LockType
	   );

   return InternalSaveLongData(nRet,rec);
}


BOOL PODBCRecord::PostDelete(PINDEX row)
{
   SQLRETURN nRet;

   nRet = SQLSetPos(m_hStmt,			// Statement handle
     (unsigned short)row,				// RowNumber
     SQL_DELETE,						// Operation
     SQL_LOCK_NO_CHANGE				// LockType
     );

   return (Stmt->SQL_OK(nRet));
}

BOOL PODBCRecord::InternalSaveLongData(SQLRETURN nRet, PODBC::Row & rec)
{

   SQLPOINTER pToken;
   SQLINTEGER cbData;
   PString DataSlice;
   PINDEX frag, col=0;

 /// Everything OK but no Long Data
   if (Stmt->SQL_OK(nRet)) 
      return TRUE;
 /// Error Somewhere else.
   if (nRet != SQL_NEED_DATA) 
      return FALSE;
	
/// If More Data Required
   while (nRet == SQL_NEED_DATA) {
	 nRet = SQLParamData(m_hStmt,&pToken);

   if (col != *(PINDEX*)pToken) {
     col = *(PINDEX*)pToken;
     DataSlice = PString();
     frag = 0;
     cbData = MAX_DATA_LEN;
   }
	 
   if (nRet == SQL_NEED_DATA) {
     while (rec[col].DataFragment(DataSlice,frag, cbData))
      SQLPutData(m_hStmt,DataSlice.GetPointer(cbData),cbData);
   }
   }
   return TRUE;
}

PODBC::FieldTypes PODBCRecord::ColumnType( PINDEX Column )
{
   int nType=SQL_C_DEFAULT;
   SQLTCHAR svColName[ 256 ]=_T("");
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   UDWORD pcbColDef;
   SQLDescribeCol( m_hStmt,            // Statement handle
       Column,             // ColumnNumber
       svColName,          // ColumnName
       sizeof( svColName), // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr
   nType=(int)swType;
   return( (PODBC::FieldTypes)nType );
}

PTODBC::DWORD PODBCRecord::ColumnSize( PINDEX Column )
{
//   int nType=SQL_C_DEFAULT;
   SQLTCHAR svColName[ 256 ]=_T("");
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   PTODBC::DWORD pcbColDef=0;
   SQLDescribeCol( m_hStmt,            // Statement handle
       Column,             // ColumnNumber
       svColName,          // ColumnName
       sizeof( svColName), // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr
   return pcbColDef;
}

PTODBC::DWORD PODBCRecord::ColumnScale( PINDEX Column )
{
//   int nType=SQL_C_DEFAULT;
   SQLTCHAR svColName[ 256 ]=_T("");
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   PTODBC::DWORD pcbColDef=0;
   SQLDescribeCol( m_hStmt,            // Statement handle
       Column,             // ColumnNumber
       svColName,          // ColumnName
       sizeof( svColName), // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr
   return swScale;
}

PString PODBCRecord::ColumnName(PINDEX Column) //, PString Name, SHORT NameLen )
{
//   int nType=SQL_C_DEFAULT;
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   PTODBC::DWORD pcbColDef=0;
   TCHAR Name[256]=_T("");
   SQLRETURN Ret=
    SQLDescribeCol( m_hStmt,            // Statement handle
       Column,               // ColumnNumber
       (SQLTCHAR*)(LPTSTR)Name,     // ColumnName
       sizeof(Name),    // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr

   if (!Stmt->SQL_OK(Ret))
		return PString();

   return Name;
}

BOOL PODBCRecord::IsColumnNullable( PINDEX Column )
 {
//   int nType=SQL_C_DEFAULT;
   SQLTCHAR svColName[ 256 ]=_T("");
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   UDWORD pcbColDef;
   SQLDescribeCol( m_hStmt,            // Statement handle
       Column,             // ColumnNumber
       svColName,          // ColumnName
       sizeof( svColName), // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr
   return (swNull==SQL_NULLABLE);
}

BOOL PODBCRecord::IsColumnUpdatable(PINDEX Column )
{

   SWORD colUpdate=0;
   SQLColAttribute(m_hStmt,     // StatementHandle
        (SQLSMALLINT)(Column),  // ColumnNumber
        SQL_DESC_UPDATABLE,	// FieldIdentifier
        NULL,			// CharacterAttributePtr
        0,			// BufferLength
        NULL,			// StringLengthPtr
        &colUpdate);		// NumericAttributePtr
   return (colUpdate != SQL_ATTR_READONLY);
}

BOOL PODBCRecord::IsColumnAutoIndex(PINDEX Column )
{

   SWORD colIndex=0;
   SQLColAttribute(m_hStmt,	// StatementHandle
        (SQLSMALLINT)(Column),  // ColumnNumber
        SQL_DESC_AUTO_UNIQUE_VALUE,  // FieldIdentifier
        NULL,                    // CharacterAttributePtr
        0,                       // BufferLength
        NULL,                    // StringLengthPtr
        &colIndex);              // NumericAttributePtr
   return (colIndex == SQL_TRUE);
}

unsigned int PODBCRecord::ColumnPrecision(PINDEX Column )
{
   SWORD coldigits=0;
   SQLColAttribute(m_hStmt,      // StatementHandle
        (SQLSMALLINT)(Column),   // ColumnNumber
        SQL_DESC_PRECISION,      // FieldIdentifier
        NULL,                    // CharacterAttributePtr
        0,                       // BufferLength
        NULL,                    // StringLengthPtr
        &coldigits);             // NumericAttributePtr

   if (Precision < (unsigned)coldigits)
	   return Precision;
   else
	   return coldigits;
}

#ifdef _MSC_VER
 #pragma warning(default:4100)
 #pragma warning(default:4244)
#endif

#endif // P_ODBC

