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
{ PWaitAndSignal m(html_mutex);
  if(html_template_size <= 0) // count on zero initialization
  { FILE *fs;
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
    { fseek(fs, 0L, SEEK_END); html_template_size = ftell(fs); rewind(fs);
      html_template_buffer = new char[html_template_size + 1];
      if(html_template_size != fread(html_template_buffer, 1, html_template_size, fs)) html_template_size = -1;
      else html_template_buffer[html_template_size] = 0;
      fclose(fs);
    }
    else html_template_size = -1; // read error indicator
  }
  if(html_template_size <= 0) { cout << "Can't load HTML template!\n"; PTRACE(1,"WebCtrl\tCan't read HTML template from file"); return; }

  PString lang = PConfig(OPENMCU_PCONFIG_CONSTRUCTOR("Parameters")).GetString("Language").ToLower();

  PString html0(html_template_buffer); html0 = html0.Left(html0.Find("$BODY$"));
  html0.Replace("$LANG$",     lang,     TRUE, 0);
  html0.Replace("$PTITLE$",   ptitle,   TRUE, 0);
  html0.Replace("$TITLE$",    title,    TRUE, 0);
  html0.Replace("$QUOTE$",    quotekey, TRUE, 0);

  html << html0;
}

void EndPage (PStringStream &html, PString copyr) 
{
  PWaitAndSignal m(html_mutex);
  if (html_template_size <= 0) return;
  PString html0(html_template_buffer); html0 = html0.Mid(html0.Find("$BODY$")+6,P_MAX_INDEX);
  html0.Replace("$COPYRIGHT$", copyr,   TRUE, 0);
  html << html0;
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
