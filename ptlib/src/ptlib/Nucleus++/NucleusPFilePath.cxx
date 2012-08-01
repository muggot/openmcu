#include <ptlib.h>

extern char *  mktemp (char *);

static PString CanonicaliseDirectory (const PString & path)

{
#ifdef WOT_NO_FILESYSTEM
  return PString(path);
#else
  PString canonical_path;
  PString slash("/");

  // if the path does not start with a slash, then the current directory
  // must be prepended
  if (path.IsEmpty() || path[0] != '/')  {
    char *p = getcwd(canonical_path.GetPointer(P_MAX_PATH), P_MAX_PATH);
    PAssertOS (p != NULL);
  }

  // if the path doesn't end in a slash, add one
  if (canonical_path[canonical_path.GetLength()-1] != '/')
    canonical_path += slash;

  const char * ptr = path;
  const char * end;

  for (;;) {
    // ignore slashes
    while (*ptr == '/' && *ptr != '\0')
      ptr++;

    // finished if end of string
    if (*ptr == '\0')
      break;

    // collect non-slash characters
    end = ptr;
    while (*end != '/' && *end != '\0')
      end++;

    // make a string out of the element
    PString element(ptr, end - ptr);
    
    if (element == "..") {
      PINDEX last_char = canonical_path.GetLength()-1;
      if (last_char > 0)
        canonical_path = canonical_path.Left(canonical_path.FindLast('/', last_char-1)+1);
    } else if (element == "." || element == "") {
    } else {
      canonical_path += element;
      canonical_path += slash;
    }
    ptr = end;
  }

  return canonical_path;
#endif // WOT_NO_FILESYSTEM
}

static PString CanonicaliseFilename(const PString & filename)

{
  PINDEX p;
  PString dirname;

  // if there is a slash in the string, extract the dirname
  if ((p = filename.FindLast('/')) != P_MAX_INDEX) {
    dirname = filename(0,p);
    while (filename[p] == '/')
      p++;
  } else
    p = 0;

  return CanonicaliseDirectory(dirname) + filename(p, P_MAX_INDEX);
}

///////////////////////////////////////////////////////////////////////////////
// PFilePath

PFilePath::PFilePath(const PString & str)
  : PString(CanonicaliseFilename(str))
{
#ifdef __NUCLEUS_PLUS__
  PAssertAlways("NoPFilePath under Nucleus");
#endif
}


PFilePath::PFilePath(const char * cstr)
  : PString(CanonicaliseFilename(cstr))
{
#ifdef __NUCLEUS_PLUS__
  PAssertAlways("NoPFilePath under Nucleus");
#endif
}


PFilePath::PFilePath(const char * prefix, const char * dir)
  : PString()
{
#ifdef __NUCLEUS_PLUS__
  PAssertAlways("NoPFilePath under Nucleus");
#else
  if (prefix == NULL)
    prefix = "tmp";
  
  char * n;
  if (dir == NULL) {
    n = tempnam(NULL, prefix);
    *this = CanonicaliseFilename(n);
    runtime_free (n);
  } else {
    PDirectory s(dir);
    PString p = s + prefix + "XXXXXX";
    if (mktemp(p.GetPointer()) == 0L) {
      char extra = 'a';
      do 
        p = s + prefix + extra++ + "XXXXXX";
      while (mktemp(p.GetPointer()) == 0L && extra <= 'z');
    }
    *this = PString(p);
  }
#endif
}


PFilePath & PFilePath::operator=(const PString & str)
{
#ifdef __NUCLEUS_PLUS__
//  PAssertAlways("NoPFilePath under Nucleus");
  PError << "NoPFilePath under Nucleus (in";
  PError << "PFilePath & PFilePath::operator=(const PString & str)";
  PError << ")\n";
#else
  PString::operator=(CanonicaliseFilename(str));
#endif
  return *this;
}


PString PFilePath::GetPath() const

{
#ifdef __NUCLEUS_PLUS__
  PAssertAlways("NoPFilePath under Nucleus");
  return "";
#else
  int i;

  PAssert((i = FindLast('/')) != P_MAX_INDEX, PInvalidArrayIndex);
  return Left(i+1);
#endif
}


PString PFilePath::GetTitle() const

{
  PString fn(GetFileName());
  return fn(0, fn.FindLast('.')-1);
}


PString PFilePath::GetType() const

{
#ifdef __NUCLEUS_PLUS__
  PAssertAlways("NoPFilePath under Nucleus");
  return "NoPFilePath under Nucleus";
#else
  int p = FindLast('.');
  int l = (p == P_MAX_INDEX) ? 0 : (GetLength() - p);

  if (p < 0 || l < 2)
    return PString("");
  else
    return (*this)(p, P_MAX_INDEX);
#endif
}


void PFilePath::SetType(const PString & type)
{
#ifdef __NUCLEUS_PLUS__
  PAssertAlways("NoPFilePath under Nucleus");
#else
  PINDEX dot = Find('.', FindLast('/'));
  if (dot != P_MAX_INDEX)
    Splice(type, dot, GetLength()-dot);
  else
    *this += type;
#endif
}


PString PFilePath::GetFileName() const

{
#ifdef __NUCLEUS_PLUS__
  PAssertAlways("NoPFilePath under Nucleus");
  return "";
#else
  int i;

  if ((i = FindLast('/')) == P_MAX_INDEX)
    return *this;
  else
    return Right(GetLength()-i-1);
#endif
}


PDirectory PFilePath::GetDirectory() const
{
#ifdef __NUCLEUS_PLUS__
  PAssertAlways("NoPFilePath under Nucleus");
  return "";
#else
  int i;

  if ((i = FindLast('/')) == P_MAX_INDEX)
    return "./";
  else
    return Left(i);
#endif
}


BOOL PFilePath::IsValid(char c)
{
  return c != '/';
}


BOOL PFilePath::IsValid(const PString & str)
{
  return str.Find('/') == P_MAX_INDEX;
}
