#include <ptlib.h>
#include "mcu.h"
#include <stdio.h>
#include <string.h>

PString Browser_Language(PHTTPRequest & request)
{
  PMIMEInfo mime = request.server.GetConnectionInfo().GetMIME();
  if(mime.Contains("Accept-Language"))
    if(mime("Accept-Language").Find("ru-")==0) return "RU";
  return "EN";
}

static unsigned long html_template_size; // count on zero initialization
char * html_template_buffer;
static unsigned long html_quote_size; // count on zero initialization
char * html_quote_buffer;
PMutex html_mutex;

void BeginPage (PStringStream &html, const char *ptitle, const char *title, const char *quotekey)
{
  PWaitAndSignal m(html_mutex);

  if(html_template_size == 0) // count on zero initialization
  {
    FILE *fs;
#ifdef SYS_RESOURCE_DIR
#  ifdef _WIN32
    fs=fopen(SYS_RESOURCE_DIR+PString("\\template.html"), "r");
#  else
    fs=fopen(SYS_RESOURCE_DIR+PString("/template.html"), "r");
#  endif
#else
    fs=fopen("template.html", "r");
#endif
    if(fs)
    {
      fseek(fs, 0L, SEEK_END);
      html_template_size = ftell(fs);
      rewind(fs);
      html_template_buffer = new char[html_template_size + 1];
      if(html_template_size != fread(html_template_buffer, 1, html_template_size, fs))
      { cout << "Can't load HTML template!\n";
        PTRACE(1,"WebCtrl\tCan't read HTML template from file");
        html_template_size = -1;
      }
      else html_template_buffer[html_template_size] = 0;
      fclose(fs);
    }
    else
    {
      html_template_size = -1; // read error indicator
    }
  }

  if(html_quote_size == 0) // count on zero initialization
  {
    FILE *fs;
#ifdef SYS_RESOURCE_DIR
#  ifdef _WIN32
    fs=fopen(SYS_RESOURCE_DIR+PString("\\quote.txt"), "r");
#  else
    fs=fopen(SYS_RESOURCE_DIR+PString("/quote.txt"), "r");
#  endif
#else
    fs=fopen("quote.txt", "r");
#endif
    if(fs)
    {
      fseek(fs, 0L, SEEK_END);
      html_quote_size = ftell(fs);
      rewind(fs);
      html_quote_buffer = new char[html_quote_size + 1];
      if(html_quote_size != fread(html_quote_buffer, 1, html_quote_size, fs))
      { cout << "Can't load quote.txt!\n";
        PTRACE(1,"WebCtrl\tCan't read quote.txt");
        html_quote_size = -1;
      }
      else html_quote_buffer[html_quote_size] = 0;
      fclose(fs);
    }
    else
    {
      html_quote_size = -1; // read error indicator
    }
  }

  if ((html_template_size > 0) && (html_quote_size > 0))
  {
    PString lang = PConfig("Parameters").GetString("Language").ToLower();
    char *lng = strstr(html_template_buffer, "$LANG$");
    if(lng && lang != "")
    {
      *lng=0;
      html << html_template_buffer << lang;
      *lng='$';
      lng+=6;
    }
    else lng = html_template_buffer;

    char *ptt = strstr(lng, "$PTITLE$");
    if(ptt)
    {
      *ptt=0;
      html << lng << ptitle;
      *ptt='$';
      ptt+=8;
    }
    else ptt = lng;

    char *tt=strstr(ptt, "$TITLE$");
    if(tt)
    {
      *tt=0;
      html << ptt << title;
      *tt='$';
      tt+=7;
    }
    else tt=ptt;

    char *quote_beg = strstr(html_quote_buffer, quotekey); // searching key here
    char *quote_end = NULL;
    BOOL needs_restore = FALSE;
    if(quote_beg)
    {
      quote_beg += strlen(quotekey);
      quote_end = strstr(quote_beg, "$$");
      if(quote_end)
      {
        *quote_end=0;
        needs_restore = TRUE;
      }
    }
    else quote_beg = html_quote_buffer;

    char *qt = strstr(tt, "$QUOTE$");
    if(qt)
    {
      *qt=0;
      html << tt << quotekey;
      *qt='$';
      qt+=7;
    }
    else qt=tt;
    if(needs_restore) *quote_end='$';

    char *qt2 = strstr(qt, "$QUOTE2$");
    if(qt2)
    {
      *qt2=0;
      html << qt << quote_beg;
      *qt2='$';
      qt2+=8;
    }
    else qt2=qt;
    if(needs_restore) *quote_end='$';

    char *bt = strstr(qt2,"$BODY$");
    if(bt)
    {
      *bt=0;
      html << qt2;
      *bt='$';
    }
  } //  if ((html_template_size > 0) && (html_quote_size > 0))
}

void EndPage (PStringStream &html, PString copyr) 
{
  PWaitAndSignal m(html_mutex);

  if ((html_template_size > 0) && (html_quote_size > 0))
  {
    char *bt = strstr(html_template_buffer,"$BODY$");
    if(bt)
    {
      bt+=6;
      char *ct=strstr(bt,"$COPYRIGHT$");
      if(ct)
      {
        *ct=0;
        html << bt << copyr;
        *ct='$';
        ct+=11;
        html << ct;
      }
    }
  }
}

PString ErrorPage( //maybe ptlib could already create pages like this? for future: dig into http server part
  PString        ip,            // "192.168.1.1"
  unsigned short port,          // 1420
  unsigned       errorCode,     // 403
  PString        errorText,     // "Forbidden"
  PString        title,         // "Page you tried to access is forbidden, lol"
  PString        description    // detailed: "blablablablablabla \n blablablablablabla"
)
{
  PStringStream p;
  PTime now;
  p << "<html>";
    p << "<head>";
      p << "<title>" << errorCode << " " << errorText << "</title>";
    p << "</head>";
    p << "<body>";

    p << "<h1>" << errorText << "</h1>";
    p << "<h3>" << title << "</h3>";

    PStringArray text = description.Lines();
    for(PINDEX i=0;i<text.GetSize();i++) p << "<p>" << text[i] << "</p>";

    p << "<hr><i>"
      << OpenMCU::Current().GetName() << " "  << OpenMCU::Current().GetVersion(TRUE)
      << " (" << OpenMCU::Current().GetOSName() << " " << OpenMCU::Current().GetOSVersion() << ") "
      << " Server at "
      << ip << " Port " << port
      << ". Generated " << now.AsString(PTime::RFC1123, PTime::GMT)
      << "</i>";
    p << "</body>";
  p << "</html>";
  return p;
}
