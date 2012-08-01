#include <ptlib.h>

void PDirectory::Construct ()

{
#ifdef __NUCLEUS_PLUS__
PAssertAlways("No PDirectories under Nucleus");
#else
  directory   = NULL;
  entryBuffer = NULL;
  entryInfo   = NULL;

  PString::operator =(CanonicaliseDirectory(*this));
#endif
}

void PDirectory::Close()
{
#ifdef __NUCLEUS_PLUS__
PAssertAlways("No PDirectories under Nucleus");
#else
  if (directory != NULL) {
    PAssert(closedir(directory) == 0, POperatingSystemError);
    directory = NULL;
  }

  if (entryBuffer != NULL) {
    free(entryBuffer);
    entryBuffer = NULL;
  }

  if (entryInfo != NULL) {
    delete entryInfo;
    entryInfo = NULL;
  }
#endif
}

void PDirectory::CopyContents(const PDirectory & d)
{
#ifdef __NUCLEUS_PLUS__
PAssertAlways("No PDirectories under Nucleus");
#else
  if (d.entryInfo == NULL)
    entryInfo = NULL;
  else {
    entryInfo  = new PFileInfo;
    *entryInfo = *d.entryInfo;
  }
  directory   = NULL;
  entryBuffer = NULL;
#endif
}

BOOL PDirectory::Create(const PString & p, int perm)
{
#ifdef __NUCLEUS_PLUS__
PAssertAlways("No PDirectories under Nucleus");
return TRUE;
#else
  PAssert(!p.IsEmpty(), "attempt to create dir with empty name");
  PString str = p.Left(p.GetLength()-1);
  return mkdir(str, perm) == 0;
#endif
}

BOOL PDirectory::Remove(const PString & p)
{
#ifdef __NUCLEUS_PLUS__
PAssertAlways("No PDirectories under Nucleus");
return TRUE;
#else
  PAssert(!p.IsEmpty(), "attempt to remove dir with empty name");
  PString str = p.Left(p.GetLength()-1);
  return rmdir(str) == 0;
#endif
}
