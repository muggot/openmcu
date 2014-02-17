/*
 * podbc.h
 *
 * Virteos ODBC Implementation for PWLib Library.
 *
 * Virteos is a Trade Mark of ISVO (Asia) Pte Ltd.
 *
 * Copyright (c) 2005 ISVO (Asia) Pte Ltd. All Rights Reserved.
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
 * $Log: podbc.h,v $
 * Revision 1.4  2007/08/25 06:37:39  csoutheren
 * Fixed compilation on 64bit Linux
 *
 * Revision 1.3  2007/04/30 00:07:26  csoutheren
 * Fix problems with PODBC on Windows
 *
 * Revision 1.2  2007/04/25 09:21:20  csoutheren
 * Move unixODBC includes to a seperate namespace to avoid namespace conflicts
 *
 * Revision 1.1  2007/04/13 18:17:57  shorne
 * added ODBC support for linux thx Michal Z
 *
 * Revision 1.3  2006/06/27 04:28:16  shorne
 * Removed debuging code
 *
 * Revision 1.2  2006/06/25 11:20:24  csoutheren
 * Add detection of ODBC on Windows
 *
 * Revision 1.1  2006/01/27 06:35:30  shorne
 * added ODBC support
 *
 *
*/

/**
	ODBC Support for PWLIB

  Class Description
	 PODBC				:  Main DataBase Connection Class (Derive Class for Error Handling)
	 PODBC::ConnectData :  Class used to store information for Connecting to DataSource
	 PODBC::Table		:  Retrieved Data Structure (RecordSet) from Table or Select SQL Query
	 PODBC::Row			:  Record Pointer Class for the PODBC::Table (PArray of Fields)
	 PODBC::Field		:  Database Field Information (Field Structure & Data(Bound))
	 PODBC::Field:Bind	:  DataTypes Bound to the RecordSet (change with Record Navigation)
	 PDSNConnection		:  Derived Class of PODBC for ODBC Configured Connections
	 PODBCStmt			:  Wrapper for RecordSet (Internal)
	 PODBCRecord		:  Handle Retrieve/Post/Bind Data from RecordSet (Internal)

\begin{verbatim}
  Example of Use

  PODBC link;
  PODBC::ConnectData data;
  data.DBPath = "test.mdb";

  if (link.DataSource(PODBC::MSAccess,data)) {
    // Load a Database Table (could also be a SELECT Query)
			PODBC::Table table(&link,"FooTable");
	// Bind to Column 1 
			PODBC::Field & field = table.Column(1):
    // Display Contents
			cout << " Value " << field.AsString(); << endl;
	// Move to Record 2 of fooTable
			table[2];  
	// Display contents of Record 2 Column 1
			cout << " Value " << field.AsString(); << endl;
	// Set New Value for Record 2 Field 1 of FooTable
			field.SetValue("NewValue");
	// Send Update to Database.
			field.Post();

    // To Add New Record.(with Default Values)
			table.NewRow();
	// Alter the Value of field 1
			field.SetValue("Somethng");
	// Post the New Field to the Database
			field.Post();

    // Run General Query;
	    PString SQLStmt = "INSERT foo into [FooTable] ..."
		Link.Query(SQLStmt);
  }
// Disconnect from ODBC Source
  link.Disconnect();

\end{verbatim}
*/
//--

#if !defined(PODBC_H)
#define PODBC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#ifdef P_ODBC

namespace PTODBC {

#include <sql.h> 
#include <sqlext.h>
#include <odbcinst.h>

#ifdef _MSC_VER
 #include <tchar.h>
 #pragma comment(lib,"odbc32.lib")
 #pragma comment(lib,"odbcCP32.lib")
 typedef ::LPCTSTR LPCTSTR;
 typedef ::LPVOID LPVOID;
 typedef ::DWORD DWORD;
 typedef ::LONG LONG;
 typedef ::ULONG ULONG;
 typedef ::USHORT USHORT;
#else
 typedef const char *LPCTSTR;
 typedef void *LPVOID;
 typedef long long _int64;
 typedef long LONG;
 #define _T(x) x
#endif // _MSC_VER


// Max SQL String Data Length
#define MAX_DATA_LEN 1024

/** PODBC Statement Class 
	This class is use to parse store queries and Fetch data
	It is not designed to process the actual data only access it.
	PODBC::Record is used to Bind/Retrieve/Store Data Elements. .
*/

}; // PTODBC

class PODBC;
class PODBCRecord;
class PODBCStmt : public PObject
{
  PCLASSINFO(PODBCStmt, PObject);

PTODBC::HSTMT m_hStmt;

public:

  /**@name Constructor/Deconstructor */
  //@{
	/** Constructor PODBC (Datasources call) or thro' DSNConnection (Connection call). 
	    In General this class is constructed within the PODBC::Table Class.
	*/
	  PODBCStmt(PODBC * odbc);

	/** Deconstructor. This Class should be available for the duration of which
		a specific query/table is required and be deconstructed at the time of
		the PODBC::Table deconstruction.
	*/
	  ~PODBCStmt();
  //@}

 /**@name Handles */
  //@{  
	  /** Statement Handle Created by the Query Function.
	  */
	  operator PTODBC::HSTMT() { return m_hStmt; };
  //@}


 /**@name Data Management */
  //@{ 
	  /** IsValid Checks to ensure a Handle has been allocated and
			is effective.
	  */
	  BOOL IsValid();

	  /** GetChangedRowCount retreives the number of rows updated/altered by
			UPDATE/INSERT statements.
	  */
	  PTODBC::DWORD GetChangedRowCount(void);

	  /** Query function is the Main function to pass SQL statements to retreive/
			add/Modify database data. It accepts generally acceptable SQL Statements.
			  ie. Select * from [table-x]
	  */
	  BOOL Query(PString strSQL);
  //@}

  /**@name Data Retrieval */
  //@{  
	  /** Fetch General call to retreive the next row of data. 
	  */
	  BOOL Fetch();

	  /** FetchRow More detailed fetching of Rows. This allows you to fetch an
			Absolute row or a row relative to the current row fetched.
	  */
	  BOOL FetchRow(PINDEX nRow,BOOL Absolute=1);

	  /** FetchPrevious Fetch the previous Row from current row.
	  */
	  BOOL FetchPrevious();

	  /** FetchNext: Fetch the Next row. 
	  */
	  BOOL FetchNext();

	  /** FetchFirst Fetch the First row in the RecordSet 
	  */
	  BOOL FetchFirst();

	  /** FetchLast Fetch the Last row in the RecordSet
	  */
	  BOOL FetchLast();

	  /** Cancel the Current Statement
	  */
	  BOOL Cancel();
  //@}

  /**@name Utilities */
  //@{ 
	  /** Retreive the List of Tables from the current Datasource
			The option field can be used to specify the Table Types
			ie "TABLE" for Tables or "VIEW" for preconfigured datasource
			queries. *Further investigation is required*
	  */
	  PStringArray TableList(PString option = "");


	  /** Is the SQL Instruction OK
			If an Error is detected then GetLastError is called
			to Retrieve the SQL Error Information and Returns FALSE
	  */
	  BOOL SQL_OK(PTODBC::SQLRETURN res);

	  /** Get the Last Error 
			This returns the Error ID & String to PODBC::OnSQLError
	  */
	  void GetLastError();
  //@}

	  PODBC * odbclink;		/// Reference to the PODBC Class

	  int dbase; /// Database Type connecting to

};



/** PODBC Class
	The Main ODBC class. This Class should be used in the there is 
	not a preconfigured DSN setup in the MDAC. This class will use
	the applicable ODBC drivers to connect to a compliant Datasource.
	It Supports a wide variety of Datasources but others can added 
	by simply creating your custom connection string and then calling
	PODBC::Connect. For Defined sources the PODBC::DataSource function
	should be used.
*/

class PODBCRecord;
class PODBC  : public PObject
{
  PCLASSINFO(PODBC, PObject);

public:

  /**@name Constructor/Deconstructor */
  //@{
	/** Constructor
	*/
	PODBC();

	/** Deconstructor
	*/
	~PODBC();
  //@}

  /**@name Enumerators */
  //@{
	/** Raw SQL data type codes Refer <sql.h> SQL_*
		This list is not inclusive. If an item 
		is not listed or Unknown it is treated 
		as a character string.

	*/
	  enum FieldTypes
	  {
		LongVarChar     =-1,	 
		Binary			=-2,
		VarBinary       =-3,
		LongVarBinary	=-4,
		BigInt			=-5,
		TinyInt		    =-6,
		Bit             =-7,	 /// Boolean
		Guid			=-11,
		Unknown			= 0,
		Char			= 1,
		Numeric			= 2,
		Decimal			= 3,
		Integer			= 4,
		SmallInt		= 5,
		Float			= 6,
		Real			= 7,
		Double			= 8,
		DateTime		= 9,
		VarChar			=12,		
		Date			=91,		/// Structure   
		Time			=92,		/// Structure
		TimeStamp		=93			/// Structure	PTime		
	  };

	/** Converted Pwlib Field Types.	
		Data is stored as a PString
		and the pwType enumerator indicates
		the conversion required on the PODBC::Field.
	*/
	enum PwType
	  { 
		oPString,	// String Value
		oBOOL,		// Boolean 
		ochar,		// Character
		oshort,		// Short	
		oint,		// Integer  use .AsInteger()
		olong,		// long		
		odouble,	// Double   use .AsReal()
		oPBYTEArray,// Binary Data
		oPInt64,	// BigInt	use .AsInt64()
		oPTime,		// Time		use  PTime( "Value" ) 
		oPGUID		// GUID		use  PGUID( "Value" ) To Be Implemented...?
	};

	/** Datasources that are supported by this implementation
		used in the PODBC::DataSource Function.
	*/

	enum DataSources
	{
		mySQL,			
		MSSQL,			
		Oracle,			
		IBM_DB2,
		DBASE,
		Paradox,
		Excel,
		Ascii,
		Foxpro,
		MSAccess,
		postgreSQL
	};

	/** MSSQL protocols.If your interested?
	*/
	enum MSSQLProtocols
	{
	   MSSQLNamedPipes,
	   MSSQLWinSock,
	   MSSQLIPX,
	   MSSQLBanyan,
	   MSSQLRPC
	 };

  //@}

  /**@name Connection Class */
  //@{
	/** This class is a multipurpose use
		class for storing parameters when
		initiating connection to DataSource.
		Not all field are required. By default
		all non-essential params are set to a 
		datasource specific default value.
	*/
	 class ConnectData
	  {
		public:
		  PFilePath DBPath;    /// Database file Path (not Oracle,xxSQL)
		  PString DefDir;	   /// Used with Paradox/DBase/Excel (& mySQL db)
		  PString User;		   /// UserName
		  PString Pass;		   /// Password
		  BOOL Excl_Trust;	   /// Whether Datasource is locked or Trusted.
		  PString Host;		   /// URL for Host Datasouce xxSQL
		  int Port;			   /// Port to connect to mySQL
		  int opt;			   /// General Option Value.mySQL & Paradox
	  };
  //@}


  /**@name Database Field Class */
  //@{
	 /** Class for Field Data
	 */
	  class Row;
	  class Field : public PObject
	  {
		  PCLASSINFO(Field, PObject);
	  public:

		  /** SQL compliant Bound DataTypes.
				The appropriate Field is bound
				to the SQL Driver and alters 
				when a new record is fetched.
		  */
		  class Bind
		  {
		  public:
			   PString				sbin;		/// Strings & Binary Data
			   PString				sbinlong;   /// Long Data
			   short int			ssint;		/// Short Integer		SQLSMALLINT
			   long int				slint;		/// Integer				SQLINTEGER
			   double				sdoub;		/// Double				SQLDOUBLE
			   unsigned char		sbit;		/// Bit					SQLCHAR
			   unsigned char *		suchar;		/// Unsigned char		SQLCHAR *
#if _WIN32
			   _int64				sbint;		/// Bit Integer			SQLBIGINT
#else
         PTODBC::_int64				sbint;		/// Bit Integer			SQLBIGINT
#endif
			   PTODBC::DATE_STRUCT			date;	    /// Date Structure	
			   PTODBC::TIME_STRUCT			time;		/// Time Structure
			   PTODBC::TIMESTAMP_STRUCT		timestamp;	/// TimeStamp Structure
			   PTODBC::SQLGUID				guid;		/// GUID Structure (not Fully Supported)
			   PTODBC::SQLINTEGER			dataLen;	/// DataLength pointer (StrLen_or_Ind for Col Bind)
		  };

		/** Post the Changes back to the Database
		*/
		  BOOL Post();

		/** Returns a String representation of the field.
		*/
		  PString operator=(const PString & str);

		/** Display the Field Data as String
		*/
		  PString AsString();	

		/** Set the Field Data. Note a Post() must be called
			to post the changes back to the database.
		*/
		  void SetValue(PString value);  /// Set the Value

		/** Initialise/Set the Default values for Field of New Record
		*/
		  void SetDefaultValues();

		/** DataFragment Data is broken into fragment to be passed
			to the Database
		*/
		  BOOL DataFragment(PString & Buffer ,PINDEX & fragment, PTODBC::SQLINTEGER & size);

		/** Settings
		*/
		 /// Data
			  Bind	Data;			 /// Data Field to ODBC Bind to
			  PwType  Type;			 /// pwlib Type for conversion
			  FieldTypes ODBCType;	 /// ODBC Type (For saving/Conversion)

		 /// Column
			  PString Name;			 /// Column Name
			  PINDEX col;			 /// Column Number (For Saving/Conversion)

         /// Column Attributes
			  BOOL isReadOnly;		 /// Is Field Readonly
			  BOOL isNullable;		 /// Allows Nulls
			  BOOL isAutoInc;		 /// Field AutoIncrements
			  int Decimals;			 /// Number of decimal places to Round
			  BOOL LongData;		 /// LongData Length is Required

		 /// RecordHolder Reference
			  Row * row;			 /// Back Reference to the Row
	  };
  //@}


   /**@name Database Row Class */
  //@{ 
	  /** This class functions as a simple wrapper 
			of the PODBCStmt class to fetch/Save
			data to the Datasource. Data is fetched
			on a need to basis and not cached except
			to create a new row.
	  */
	  class Row : public PObject
	  {
	  public:

		/** Constructor
			Create a Dummy row of data to act as a 
			Record Marker. Template Field are created
			and Stored in a PARRAY.
		*/
		  Row(PODBCStmt * stmt);

		/** Retrieve Field Data given the specifed column.
			Note: Columns atart at 1 and not exceed PODBCStmt::GetColumnCount()
		*/
		  Field & Column(PINDEX col);

		/** Retreive Field Data given the Column Name
		*/
		  Field & Column(PString name);

		/** Retrieve the Column Names
		*/
		  PStringArray ColumnNames();

		/** Columns. The Number of Columns in the RecordSet
		*/
		  PINDEX Columns();

		/** Rows	The Number of Rows 
		*/
		  PINDEX Rows();

		/** Retrieve Field Data given specified column
		*/
		  Field & operator[] (PINDEX col);

		/** Retrieve Field Data given the column Name.
		*/
		  Field & operator[] (PString col);

		/** Navigate to Specified Row
		*/
		  BOOL Navigate(PINDEX row);

		/** SetNewRow Set New Row for input
		*/
		  void SetNewRow();

		/** Post the Row back to the Database.
			When Row::NewRow is true the data
			can be posted back to the Database;
			If Edit Invoked then releasea the
			RowHandler for Navigation.
		*/
		  BOOL Post();

		/** Delete the Current Record from the
			RecordSet
		*/
		  BOOL Delete(PINDEX row =0);

		  PODBCRecord * rec;      /// Record Structure

		  PINDEX CurRow;          /// Current Row
		  BOOL NewRow;			  /// Flag to Indicate New Row (requires either Post or Delete)
		  PINDEX RowCount;		  /// Number of Rows.

	  protected:
		  PArray<Field> Fields;	  /// PODBC::Field Array Cache (Used for New Row)
	  };
  //@}

   /** PODBC::Table
		This is the main Class to access Data returned by a Select Query.
		The Table does not actually create the RecordSet but acts as a wrapper 
		to the driver to access the cached data in the Driver.
   */
  class Table : public PObject
  {
  public:

   /**@name Constructor/Deconstructor */
  //@{ 
	  /** Constructor
			Using the HDBC and TableName/Select SQL Query 
			creates a virtual Table in the OBDC driver.
	  */
	  Table(PODBC * odbc, PString Query);

	  /** Deconstructor
	  */
	  ~Table();
  //@}

   /**@name Data Storage */
  //@{
    /** Add New Row
	*/
	  Row NewRow();

	/** Delete Row 0 indicates Current Row
	*/
	  BOOL DeleteRow(PINDEX row = 0);

	/** Post Update back to Database
	*/
	  BOOL Post();
  //@}

   /**@name Utilities */
  //@{
	  /** Rows. Returns the Number of Rows in the Resultant RecordSet
	  */
	  PINDEX Rows();

	  /** Columns. Returns the Number of Columns in the Resultant RecordSet
	  */
	  PINDEX Columns();

	  /** ColumnNames. Return the list of column Names of the Resultant RecordSet
	  */
	  PStringArray ColumnNames();

	  /** Obtain the Record Handler. This can be used as a Template to obtain
			Record Information. A call to tablename[i] will update the recordHandler
			with the Values contained in i Record. 
	  */
	  Row & RecordHandler();

	  /** Row return the fetched row in the Cached RecordSet. An Array of PODBC::Field
	  */
	  Row & operator[] (PINDEX row);

	  /** Returns the Field data at a predetermined position in the Resultant
			RecordSet. It Fetches the Row than isolates the Column from the fetched 
			data.
	  */
	  Field & operator() (PINDEX row, PINDEX col);

	  /** Returns the indicated Column Holder for the RecordSet,
			This can be used for iterative Row calls.
	  */
	  Field & Column(PINDEX col);

	  /** Returns the indicated Column Holder Name for the RecordSet,	
	  */
	  Field & Column(PString Name);
  //@}

  protected:
	  PODBCStmt stmt;			/// ODBC Fetched Statement Info
	  PString tableName;		/// Name of the Fetched Table (if used in Constructor)
	  Row * RowHandler;			/// row Handler
  };

   /**@name Data Queries */
  //@{ 
	  /** Load a specified Table/Stored Query or 
			General 'SELECT' SQL Query.
			This function will return a PODBC::Table for
			further analysis. Do Not Use this Function for
			any other SQL statements other than SELECT.
	  */
	  Table LoadTable(PString table);

	  /** Added Information to the DataSource. Use this 
			function if you just want to use a SQL statement
			to add data to a datasource without retreiving the
			data itself. ie "UPDATE" "APPEND" "INSERT" queries.
	  */
	  BOOL Query(PString Query);
  //@}


   /**@name DataSource Access */
  //@{ 
	 /** DataSource
			This is the main function to call to contact a
			DataSource. Source specifies the Type of DataSource
			to contact and the Data parameter contain the relevent
			connection information. You can choose to call this function
			or use the specific Connection function.
	  */
	  BOOL DataSource(DataSources Source, ConnectData Data);
  
	  /** General Connect Function
			Custom connection strings should call this 
			to connect Don't ask why its LPCTSTR!
	  */
	  virtual BOOL Connect(PTODBC::LPCTSTR svSource);

	  /** Connect to IBM DB2 DataSource
	  */
	  BOOL Connect_DB2(PFilePath DBPath);

	  /** Connect to MS Office excel spreadsheet
	  */
	  BOOL Connect_XLS(PFilePath XLSPath,PString DefDir = "");

	  /** Connect to an ascii text or cvs file
	  */
	  BOOL Connect_TXT(PFilePath TXTPath);

	  /** Connect to a Foxpro dataSource
	  */
	  BOOL Connect_FOX(PFilePath DBPath,PString User = "",
			  PString Pass = "",PString Type= "DBF",
			  BOOL Exclusive=FALSE);

	  /** Connect to a MS Access *.mdb DataSource.
	  */
	  BOOL Connect_MDB(PFilePath MDBPath,PString User ="",
				PString Pass = "",BOOL Exclusive=FALSE);

	  /** Connect to a paradox database datastore
	  */
	  BOOL Connect_PDOX(PDirectory DBPath,PDirectory DefaultDir,
				int version =5);

	  /** Connect to an Oracle Datasource
	  */
	  BOOL Connect_Oracle(PString Server,PString User="", PString Pass="");

	  /** Connect to a DBase DataStore
	  */
	  BOOL Connect_DBASE(PDirectory DBPath);

	  /** Connect to a MS SQL Server
	  */
	  BOOL Connect_MSSQL(PString User="",PString Pass="", 
			 PString Host ="(local)",BOOL Trusted = TRUE, 
			 MSSQLProtocols Proto=MSSQLNamedPipes);

	  /** Connect to a mySQL Server
	  */
	  BOOL Connect_mySQL(PString User="",PString Pass="",
			 PString Host= "localhost",
			 int Port=3306,int Option=0);

	  /** Connect to a mySQL Server's specified DataBase.
	  */
	  BOOL ConnectDB_mySQL(PString DB,PString User="",
			PString Pass="",PString Host= "localhost",
			int Port=3306,int Option=0);

	  /** Connect to a postgreSQL Server
	  */
	  BOOL Connect_postgreSQL(PString DB,PString User,
        PString Pass,PString Host, int Port=5432,int Option=0);

	  /** General Disconnect from DataSource.
	  */
	  void Disconnect();
  //@}

   /**@name Utilities */
  //@{ 
	  /** Retrieve a List of Tables in the Datasource
			use the option field to specify the type of
			data to access. ie "TABLE" or "VIEW" (further dev req'd)
	  */
	  PStringArray TableList(PString option = "");

	  /** Check whether their is a limit to Datalength
			when obtaining Long Data
	  */
	  BOOL NeedLongDataLen();

	  /** OnSQL Error
	  */
	  virtual void OnSQLError(PString RetCode, PString RetString) {};


	  /** Set the Number of Decimal places to
			round to By Default it is 4. However if the field
			decimal places is less then Precision Value the 
			field rounding will be used. This must be set prior
			to calling LoadTable()
	  */
	  void SetPrecision(int Digit);

	  /** Set the Time Display Format
	  */
	  void SetTimeFormat(PTime::TimeFormat tformat);

	  /** Operator Handle DataBase Connection
	  */
	  operator PTODBC::HDBC() { return m_hDBC; };
  //@}
	  	
	 PODBC::DataSources  dbase; /// Database Type connected to

protected:

  PTODBC::SQLRETURN       m_nReturn;      // Internal SQL Error code
  PTODBC::HENV            m_hEnv;         // Handle to environment
  PTODBC::HDBC            m_hDBC;         // Handle to database connection
};

/**		
	DSN (Data Source Name) Connection. The connection settings 
	have been preconfiured in the MDAC (Microsoft Data Access Component)
	and is called using those Preset Settings. Calling the PDSNConnection::Connect
	has the same effect and is a replaceable for PODBC::DataSource,
 */


class PDSNConnection : public PODBC
{
  PCLASSINFO(PDSNConnection, PODBC);

public:

  /**@name Constructor/Deconstructor */
  //@{
	  PDSNConnection();
	  ~PDSNConnection();
  //@}

  /**@name Connection/Disconnect */
  //@{
	  /** Connect to the MDAC using a pre-existing MDAC Defined DataSource
			This is different than calling PODBC::DataSource in that the 
			Data Source is known defined externally within MDAC,
	  */
	  BOOL Connect( PString Source ,PString Username, PString Password);

};

 //--
/** PODBCRecord
		This Class is used to analyse the fetched data and handles
		Data Conversion/Read Write operations. It is used in conjuction
		with the PODBCStmt Class
*/

class PODBCRecord : public PObject
{
  
  PCLASSINFO(PODBCRecord, PObject);

PTODBC::HSTMT m_hStmt;

public:

   /**@name Constructor/Deconstructor */
  //@{ 
	  /** Constructor 
	  */
	   PODBCRecord(PODBCStmt * hStmt);

	  /** Deconstructor
	  */
	  ~PODBCRecord(){};
  //@}

   /**@name Data Collection/Saving */
  //@{ 
	  /** Data: Main Call to retrieve and convert Field Data 
			and return the information in the PODBC::Field structure.
	  */
	  void Data(PINDEX Column, PODBC::Field & field);

	  /** InternalGetData is call when retrieving string or large binary 
			data where the size is indetermined. The Function can be iteratively
			called until the function returns FALSE.
	  */
	  BOOL InternalGetData(
                PTODBC::USHORT Column,
                PTODBC::LPVOID pBuffer, 
		PTODBC::ULONG pBufLen,
                PTODBC::SQLINTEGER * dataLen=NULL,
                int Type=SQL_C_DEFAULT
          );

	  /* Get Long Character Data. Long Data fields cannot be bound
			and Data must be Got from the RecordSet.
	  */
	  PString GetLongData(PINDEX Column);
  
	  /** Post the new record back to the RecordSet;
	  */
	  BOOL PostNew(PODBC::Row & rec);

	  /** Post the Updated record back to the RecordSet;
	  */
	  BOOL PostUpdate(PODBC::Row & rec);

	  /** Post a Delete command to the RecordSet; Default
		  1 Row is deleted.
	  */
	  BOOL PostDelete(PINDEX row= 1);

	  /** Check for and Save Long Data
	  */
	  BOOL InternalSaveLongData(PTODBC::SQLRETURN nRet,PODBC::Row & rec);

	  /** InternalBindColumn for Data input.
	  */
	  BOOL InternalBindColumn(
            PTODBC::USHORT Column,PTODBC::LPVOID pBuffer,
	    PTODBC::ULONG pBufferSize,
            PTODBC::LONG * pReturnedBufferSize=NULL,
	    PTODBC::USHORT nType=SQL_C_TCHAR
          );
  //@}

   /**@name Data Information */
  //@{ 
	  /** ColumnByName returns the column number of the column name
			If not found returns column value of 0;
	  */
	  PINDEX ColumnByName(PString Column);

	  /** ColumnCount No of columns
	  */
	  PINDEX ColumnCount();

	  /** ColumnTypes
	  */
	  PODBC::FieldTypes ColumnType(PINDEX Column );

	  /** Column Size
	  */
	  PTODBC::DWORD ColumnSize( PINDEX Column );

	  /** Column Scale
	  */
	  PTODBC::DWORD ColumnScale( PINDEX Column );

	  /** Column Name
	  */
	  PString ColumnName( PINDEX Column);

	  /** ColumnPrecision Get the Number of Decimal places
			if Precision is set the precision is set to the
			lessor of the Two.
	  */
	  unsigned int ColumnPrecision( PINDEX Column );

	  /** IsColumn Nullable. Accepts NULL value
	  */
	  BOOL IsColumnNullable( PINDEX Column );

	  /** IsColumn Updateable ie is not ReadOnly
	  */
	  BOOL IsColumnUpdatable( PINDEX Column );

	  /** IsColumnAutoIndex (ie don't give default Value)
	  */
	  BOOL IsColumnAutoIndex( PINDEX Column );

  //@}

   /**@name Data Conversion Settings */
  //@{ 
	  /** Conversion Settings
	  */
	  static unsigned int Precision;      /// Double Real Float Decimal digit rounding def= 4;
	  static int MaxCharSize;		      /// Long Data Limit KBytes def = 56; (56 Kbytes)
	  static PTime::TimeFormat TimeFormat;/// Time Format
  //@}

	  PODBCStmt * Stmt;					/// Statement Class
	  PODBC::DataSources dbase;			/// Database Type connecting to


};

#endif // PTODBC

#endif // !defined(PODBC_H)

