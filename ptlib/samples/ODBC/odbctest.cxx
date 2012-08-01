/*
 * ODBCTest.cxx
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
 * The Original Code is derived from and used in conjunction with the 
 * pwlib Libaray of the OpenH323 Project (www.openh323.org/)
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte Ltd.
 *
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: odbctest.cxx,v $
 * Revision 1.5  2007/04/13 18:17:57  shorne
 * added ODBC support for linux thx Michal Z
 *
 * Revision 1.4  2007/03/21 15:32:04  shorne
 * Added link to podbc.h
 *
 * Revision 1.3  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.2  2005/08/09 09:08:13  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.1.2.1  2005/04/25 13:55:19  shorne
 * Initial version
 *
 *
*/


#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptclib/podbc.h>

class ODBCtest : public PProcess
{
  PCLASSINFO(ODBCtest, PProcess)
  public:
    void Main();
};


PCREATE_PROCESS(ODBCtest)

void ODBCtest::Main()
{

  cout << "ODBC Component for the Pwlib Library Test Program" << endl;
  cout << "=================================================" << endl;
  cout << endl;

  PODBC link;
  PODBC::ConnectData data;
  data.DBPath = "test.mdb";

  cout << "Open AccessDB " << data.DBPath << endl;

  if (!link.DataSource(PODBC::MSAccess,data))
   cout << "ODBC Error Link" << endl;
  else {

	  cout << "Connected Access Database" << endl;
	  cout << endl;

	  PINDEX i =0;  /// Table List
	  PINDEX j =0;	/// Columns
	  PINDEX k =0;  /// Rows
	  PINDEX l =0;  /// Fields

	/// Settings
	  link.SetPrecision(2);						/// Number of Decimal Places (def = 4)   
	  link.SetTimeFormat(PTime::ShortDate);		/// Set the Default Display Time

/// Enumerate Tables in Database
///+++++++++++++++++++++++++++++
/// You can also use the QUERY keyword to view Queries
	  cout << "Tables in Database" << endl;
	  PStringArray tables = link.TableList("TABLE");
	  for (i= 0; i < tables.GetSize(); i++) {
		  cout << tables[i] << endl;
      }

/// Viewing Database Contents
///++++++++++++++++++++++++++
/// Add Select SQL Statement
	  PString sql = "SELECT Clients.Ref, Calls.Date, Calls.CalledParty, Calls.Duration";
		sql = sql + " FROM Clients INNER JOIN Calls ON Clients.Ref = Calls.Ref WHERE";
		sql = sql + "(((Clients.Ref)=1))";
	  tables.Append(&sql);
	  
	  for (i= 0; i < tables.GetSize(); i++) {      
		  cout << "=================" << endl;
	  
		  PODBC::Table table(&link,tables[i]);
		  cout << "Query Table: " << tables[i] << endl;
		  cout << "Columns: " << table.Columns() << " Rows: " << table.Rows() << endl;
		   
		  cout << "ColumnNames : " << endl;
   	      PStringArray Names = table.ColumnNames();

		  for (j= 0; j < Names.GetSize(); j++)
			  cout << Names[j] << " ";
		  cout << endl;

    // Create a Field Array. The Fields are Bound to the
	// ODBC Driver so Row Navigation updates the Field Data.
		PArray<PODBC::Field> fields;
		  for (j=0; j < table.Columns(); j++)
			  fields.Append(&table.Column(j+1));

	// You can also Reference via Record Handler 
	//	  PODBC:: Row & row = table.RecordHandler();
	//	  PODBC::Field & f1 = row.Column(1);
	//		...etc...
	// or Access the Field directly via the Recordset 
	//	  PODBC::Field f1 = table(row.col) 
     
	// Display Table Contents
		  for (k= 0; k < table.Rows(); k++) {
		   
			table[k+1];  /// set the Record index to row j
			for (l =0; l < table.Columns(); l++) {
				cout << fields[l].AsString() << " ";
			}
			cout << endl;
		  }
	  }
	cout << endl;

/// Table Modification Examples 
/// +++++++++++++++++++++++++++
	cout << "Modify Table Calls" << endl;
	  PODBC::Table ntable(&link,"Calls");

	cout << endl;

/// Delete a Record (Directly Via RecordSet)
	cout << "Delete the Last Record #" << ntable.Rows() << endl;

	 if (ntable.DeleteRow(ntable.Rows()))
		 cout << "Last Record Deleted.." << endl;
	 else
		 cout << "Error Deleting Last Record" << endl;

	 cout << endl;

/// Update a Field (Using RecordHolder callRef is field 2)
	 cout << "Add 1 to the callRef field of the First Row" << endl;
		PODBC::Row handle = ntable[1];
		  int Num = handle[2].AsString().AsInteger();
		cout << "Old Value " << Num << " ";
		  handle[2].SetValue(Num+1);
		  handle[2].Post();
		cout << "New Value " << handle[2].AsString() << endl;

        cout << endl;

/// Adding a New Record (Using Column Names)
	 	cout << "Add New Record to Calls Table" << endl;
		  ntable.NewRow();
		    ntable.Column("CallRef").SetValue(1324);
		    ntable.Column("Ref").SetValue(2);
		    ntable.Column("Date").SetValue(PTime().AsString());
		    ntable.Column("Duration").SetValue("2.45");
		    ntable.Column("CalledParty").SetValue("Fred");
		  if (ntable.Post())
			  cout << "New Record Added!" << endl;

       cout << endl;

/// Display the RecordSet Contents thro' the RecordSet. (x,y)
	cout << "Display Table with new Record" << endl;

	  for (i=0; i< ntable.Rows(); i++)
	  {
		  for (j =0; j < ntable.Columns(); j++)
		    cout << ntable(i+1,j+1).AsString() << " ";

		  cout << endl;
	  }		
	cout << "Rows " << ntable.Rows() << endl;
  }
  link.Disconnect();

}
// End of netif.cxx

