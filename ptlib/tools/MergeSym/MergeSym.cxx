/*
 * MergeSym.cxx
 *
 * Symbol merging utility for Windows DLL's.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: MergeSym.cxx,v $
 * Revision 1.20  2007/06/05 00:57:48  rjongbloed
 * Fixed output of separate DEF file to source file when no symbols change.
 *
 * Revision 1.19  2007/06/04 08:31:31  rjongbloed
 * Added ability for MergeSym to output new DEF file to different location, not changing the source DEF file.
 *
 * Revision 1.18  2006/07/09 09:46:25  csoutheren
 * Updated to work with VS 2005
 * Thanks to Martin Brown
 *
 * Revision 1.17  2006/06/26 05:22:52  shorne
 * Fixed compile issue
 *
 * Revision 1.16  2005/04/20 07:02:11  csoutheren
 * Changed handle "," in include paths
 *
 * Revision 1.15  2004/06/05 04:55:29  rjongbloed
 * Removed the unmangled versions of symbols to eliminate problems withthe line length
 *   exceeding MSVC linkers internal limits. Has added benefit of making files half the size.
 *
 * Revision 1.14  2004/05/22 07:41:32  rjongbloed
 * Added ability to specify command line override of the "dumpbin" program run
 *   to get library symbols, very useful for debugging.
 *
 * Revision 1.13  2004/04/09 07:03:23  rjongbloed
 * Added ability to get the output to DUMPBIN sent to a file.
 *
 * Revision 1.12  2004/04/03 06:54:32  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.11  2003/10/30 11:27:12  rjongbloed
 * Fixed ability to specify partial path for -x parameter.
 *
 * Revision 1.10  2003/02/11 07:00:17  robertj
 * Added copying def file to a backup version before alteration from read only.
 *
 * Revision 1.9  2002/06/13 05:51:01  robertj
 * Added ignore of some inherently private symbols exported by libraru eg debug
 *   line number info and real number constants.
 *
 * Revision 1.8  2000/12/18 07:31:10  robertj
 * Fixed minor irritation with DEF file being reordered when symbol added.
 *
 * Revision 1.7  2000/04/29 05:48:22  robertj
 * Added error and progress reporting in searching path for external DEF files.
 *
 * Revision 1.6  2000/04/29 05:01:49  robertj
 * Added multiple external DEF file capability (-x flag).
 * Added directory search path argument for external DEF files.
 * Fixed bug for symbols in external DEF file actively removed from merged DEF file.
 * Added copyright notice.
 *
 */

#include <ptlib.h>
#include <ptlib/pipechan.h>
#include <ptlib/pprocess.h>
void unsetenv(const char *);

PDECLARE_CLASS(Symbol, PCaselessString)
  public:
    Symbol(const PString & sym, const PString & cpp, PINDEX ord = 0, BOOL ext = FALSE)
      : PCaselessString(sym), unmangled(cpp) { ordinal = ord; external = ext; }

    void SetOrdinal(PINDEX ord) { ordinal = ord; }
    BOOL IsExternal() const { return external; }

    void PrintOn(ostream & s) const
    { s << "    " << theArray << " @" << ordinal << " NONAME\n"; }

  private:
    PString unmangled;
    PINDEX ordinal;
    BOOL external;
};

PSORTED_LIST(SortedSymbolList, Symbol);


PDECLARE_CLASS(MergeSym, PProcess)
  public:
    MergeSym();
    void Main();
};

PCREATE_PROCESS(MergeSym);


MergeSym::MergeSym()
  : PProcess("Equivalence", "MergeSym", 1, 5, ReleaseCode, 0)
{
}


void MergeSym::Main()
{
  cout << GetName() << " version " << GetVersion(TRUE)
       << " on " << GetOSClass() << ' ' << GetOSName()
       << " by " << GetManufacturer() << endl;

  PArgList & args = GetArguments();
  args.Parse("vsd:x:I:");

  PFilePath lib_filename, def_filename, out_filename;

  switch (args.GetCount()) {
    case 3 :
      out_filename = args[2];
      def_filename = args[1];
      lib_filename = args[0];
      break;

    case 2 :
      def_filename = out_filename = args[1];
      lib_filename = args[0];
      break;

    case 1 :
      lib_filename = def_filename = args[0];
      def_filename.SetType(".def");
      out_filename = def_filename;
      break;

    default :
      PError << "usage: MergeSym [ -v ] [ -s ] [ -d dumpbin ] [ -x deffile[.def] ] [-I deffilepath ] libfile[.lib] [ deffile[.def] [ outfile[.def] ] ]";
      SetTerminationValue(1);
      return;
  }

  if (lib_filename.GetType().IsEmpty())
    lib_filename.SetType(".lib");

  if (!PFile::Exists(lib_filename)) {
    PError << "MergeSym: library file " << lib_filename << " does not exist.\n";
    SetTerminationValue(1);
    return;
  }

  if (def_filename.GetType().IsEmpty())
    def_filename.SetType(".def");

  if (out_filename.GetType().IsEmpty())
    out_filename.SetType(".def");

  SortedSymbolList def_symbols;

  if (args.HasOption('x')) {
    PStringArray include_path;
    if (args.HasOption('I')) {
      PString includes = args.GetOptionString('I');
      if (includes.Find(';') == P_MAX_INDEX)
        include_path = includes.Tokenise(',', FALSE);
      else
        include_path = includes.Tokenise(';', FALSE);
    }
    include_path.InsertAt(0, new PString());
    PStringArray file_list = args.GetOptionString('x').Lines();
    for (PINDEX ext_index = 0; ext_index < file_list.GetSize(); ext_index++) {
      PString base_ext_filename = file_list[ext_index];
      PString ext_filename = base_ext_filename;
      if (PFilePath(ext_filename).GetType().IsEmpty())
        ext_filename += ".def";

      PINDEX previous_def_symbols_size = def_symbols.GetSize();
	  PINDEX inc_index = 0;
      for (inc_index = 0; inc_index < include_path.GetSize(); inc_index++) {
        PString trial_filename = PDirectory(include_path[inc_index]) + ext_filename;
        if (args.HasOption('v'))
          cout << "\nTrying " << trial_filename << " ..." << flush;
        PTextFile ext;
        if (ext.Open(trial_filename, PFile::ReadOnly)) {
          if (args.HasOption('v'))
            cout << "\nReading external symbols from " << ext.GetFilePath() << " ..." << flush;
          BOOL prefix = TRUE;
          while (!ext.eof()) {
            PCaselessString line;
            ext >> line;
            if (prefix)
              prefix = line.Find("EXPORTS") == P_MAX_INDEX;
            else {
              PINDEX start = 0;
              while (isspace(line[start]))
                start++;
              PINDEX end = start;
              while (line[end] != '\0' && !isspace(line[end]))
                end++;
              def_symbols.Append(new Symbol(line(start, end-1), "", 0, TRUE));
              if (args.HasOption('v') && def_symbols.GetSize()%100 == 0)
                cout << '.' << flush;
            }
          }
          break;
        }
      }
      if (inc_index >= include_path.GetSize())
        PError << "MergeSym: external symbol file \"" << base_ext_filename << "\" not found.\n";
      if (args.HasOption('v'))
        cout << '\n' << (def_symbols.GetSize() - previous_def_symbols_size)
             << " symbols read." << endl;
    }
  }

  PStringList def_file_lines;
  PINDEX max_ordinal = 0;
  PINDEX removed = 0;

  PTextFile def;
  if (def.Open(def_filename, PFile::ReadOnly)) {
    if (args.HasOption('v'))
      cout << "Reading existing ordinals..." << flush;
    BOOL prefix = TRUE;
    while (!def.eof()) {
      PCaselessString line;
      def >> line;
      if (prefix) {
        def_file_lines.AppendString(line);
        if (line.Find("EXPORTS") != P_MAX_INDEX)
          prefix = FALSE;
      }
      else {
        PINDEX start = 0;
        while (isspace(line[start]))
          start++;
        PINDEX end = start;
        while (line[end] != '\0' && !isspace(line[end]))
          end++;
        PINDEX ordpos = line.Find('@', end);
        if (ordpos != P_MAX_INDEX) {
          PINDEX ordinal = line.Mid(ordpos+1).AsInteger();
          if (ordinal > max_ordinal)
            max_ordinal = ordinal;
          PINDEX unmanglepos = line.Find(';', ordpos);
          if (unmanglepos != P_MAX_INDEX)
            unmanglepos++;
          Symbol sym(line(start, end-1), line.Mid(unmanglepos), ordinal);
          if (def_symbols.GetValuesIndex(sym) == P_MAX_INDEX)
            def_symbols.Append(new Symbol(sym));
          removed++;
          if (args.HasOption('v') && def_symbols.GetSize()%100 == 0)
            cout << '.' << flush;
        }
      }
    }
    def.Close();
    if (args.HasOption('v'))
      cout << '\n' << removed << " symbols read." << endl;
  }
  else {
    def_file_lines.AppendString("LIBRARY " + def_filename.GetTitle());
    def_file_lines.AppendString("EXPORTS");
  }

  if (args.HasOption('v'))
    cout << "Reading library symbols..." << flush;

  unsetenv("VS_UNICODE_OUTPUT");

  PINDEX linecount = 0;
  SortedSymbolList lib_symbols;
  PString dumpbin = args.GetOptionString('d', "dumpbin");
  PPipeChannel pipe(dumpbin + " /symbols '" + lib_filename + "'", PPipeChannel::ReadOnly);
  if (!pipe.IsOpen()) {
    PError << "\nMergeSym: could not run \"" << dumpbin << "\".\n";
    SetTerminationValue(2);
    return;
  }

  PTextFile symfile;
 // if (args.HasOption('s')) {
    PFilePath sym_filename = out_filename;
    sym_filename.SetType(".sym");
    if (!symfile.Open(sym_filename, PFile::WriteOnly))
      cerr << "Could not open symbol file " << sym_filename << endl;
 // }

  while (!pipe.eof()) {
    PString line;
    pipe >> line;
    symfile << line;

    char * namepos = (char *)strchr(line, '|');
    if (namepos != NULL) {
      *namepos = '\0';
      while (*++namepos == ' ');
      if (strstr(line, " UNDEF ") == NULL &&
          strstr(line, " External ") != NULL &&
          strstr(namepos, "deleting destructor") == NULL) {
        int namelen = strcspn(namepos, "\r\n\t ");
        PString name(namepos, namelen);
        if (strncmp(name, "??_C@_", 6) != 0 &&
            strncmp(name, "__real@", 7) != 0 &&
            strncmp(name, "?__LINE__Var@", 13) != 0 &&
            lib_symbols.GetValuesIndex(name) == P_MAX_INDEX) {
          const char * unmangled = strchr(namepos+namelen, '(');
          if (unmangled == NULL)
            unmangled = name;
          else {
            unmangled++;
            char * endunmangle = (char *)strrchr(unmangled, ')');
            if (endunmangle != NULL)
              *endunmangle = '\0';
          }
          lib_symbols.Append(new Symbol(name, unmangled));
        }
      }
    }
    if (args.HasOption('v') && linecount%500 == 0)
      cout << '.' << flush;
    linecount++;
  }

  if (args.HasOption('v'))
    cout << '\n' << lib_symbols.GetSize() << " symbols read.\n"
            "Sorting symbols... " << flush;

  PINDEX i;
  for (i = 0; i < def_symbols.GetSize(); i++) {
    if (lib_symbols.GetValuesIndex(def_symbols[i]) != P_MAX_INDEX &&
        !def_symbols[i].IsExternal())
      removed--;
  }

  PINDEX added = 0;
  for (i = 0; i < lib_symbols.GetSize(); i++) {
    if (def_symbols.GetValuesIndex(lib_symbols[i]) == P_MAX_INDEX) {
      lib_symbols[i].SetOrdinal(++max_ordinal);
      added++;
    }
  }

  if (added == 0 && removed == 0)
    cout << "\nNo changes to symbols.\n";
  else
    cout << "\nSymbols merged: " << added << " added, " << removed << " removed.\n";

  if (added != 0 || removed != 0 || def_filename != out_filename) {
    if (args.HasOption('v'))
      cout << "Writing .DEF file..." << flush;

    // If file is read/only, set it to read/write
    PFileInfo info;
    if (PFile::GetInfo(out_filename, info)) {
      if ((info.permissions&PFileInfo::UserWrite) == 0) {
        PFile::SetPermissions(out_filename, info.permissions|PFileInfo::UserWrite);
        cout << "Setting \"" << out_filename << "\" to read/write mode." << flush;
        PFile::Copy(out_filename, out_filename+".original");
      }
    }

    if (def.Open(out_filename, PFile::WriteOnly)) {
      SortedSymbolList merged_symbols;
      merged_symbols.DisallowDeleteObjects();

      for (i = 0; i < def_symbols.GetSize(); i++) {
        if (lib_symbols.GetValuesIndex(def_symbols[i]) != P_MAX_INDEX &&
            !def_symbols[i].IsExternal()) {
          merged_symbols.Append(&def_symbols[i]);
        }
        if (args.HasOption('v') && i%100 == 0)
          cout << '.' << flush;
      }
      for (i = 0; i < lib_symbols.GetSize(); i++) {
        if (def_symbols.GetValuesIndex(lib_symbols[i]) == P_MAX_INDEX)
          merged_symbols.Append(&lib_symbols[i]);
        if (args.HasOption('v') && i%100 == 0)
          cout << '.' << flush;
      }

      for (i = 0; i < def_file_lines.GetSize(); i++)
        def << def_file_lines[i] << '\n';
      for (i = 0; i < merged_symbols.GetSize(); i++)
        def << merged_symbols[i];

      if (args.HasOption('v'))
        cout << merged_symbols.GetSize() << " symbols written." << endl;
    }
    else {
      PError << "Could not create file " << out_filename << ':' << def.GetErrorText() << endl;
      SetTerminationValue(1);
    }
  }

} 
// End MergeSym.cxx

 void  unsetenv(const char *name) 
 { 
     char       *envstr; 
  
     if (getenv(name) == NULL) 
         return;                 /* no work */ 
  
     /* 
      * The technique embodied here works if libc follows the Single Unix Spec 
      * and actually uses the storage passed to putenv() to hold the environ 
      * entry.  When we clobber the entry in the second step we are ensuring 
      * that we zap the actual environ member.  However, there are some libc 
      * implementations (notably recent BSDs) that do not obey SUS but copy the 
      * presented string.  This method fails on such platforms.  Hopefully all 
      * such platforms have unsetenv() and thus won't be using this hack. 
      * 
      * Note that repeatedly setting and unsetting a var using this code will 
      * leak memory. 
      */ 
  
     envstr = (char *) malloc(strlen(name) + 2); 
     if (!envstr)                /* not much we can do if no memory */ 
         return; 
  
     /* Override the existing setting by forcibly defining the var */ 
     sprintf(envstr, "%s=", name); 
     _putenv(envstr); 
  
     /* Now we can clobber the variable definition this way: */ 
     strcpy(envstr, "="); 
  
     /* 
      * This last putenv cleans up if we have multiple zero-length names as a 
      * result of unsetting multiple things. 
      */ 
     _putenv(envstr); 
 }

