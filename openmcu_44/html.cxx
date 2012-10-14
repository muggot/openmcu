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

void BeginPage (PStringStream &html, const char *ptitle, const char *title, const char *quotekey) 
{
  long p_size;
  char * p_buffer;
  long q_size;
  char * q_buffer;

  FILE *fs;

  fs=fopen("template.html","r"); if(!fs) return;
  fseek(fs,0L,SEEK_END); p_size=ftell(fs); rewind(fs);
  p_buffer=new char[p_size+1];
  fread(p_buffer,1,p_size,fs); p_buffer[p_size]=0; 
  fclose(fs);

  fs=fopen("quote.txt","r"); if(!fs) { free(p_buffer); return; }
  fseek(fs,0L,SEEK_END); q_size=ftell(fs); rewind(fs);
  q_buffer=new char[q_size+1]; q_buffer[q_size]=0;
  fread(q_buffer,1,q_size,fs);
  fclose(fs);
  
  char *ptt=strstr(p_buffer,"$PTITLE$");
  if(ptt)
  {
   *ptt=0; ptt+=8;
   html << p_buffer << ptitle;
  }
  else ptt=p_buffer;

  char *tt=strstr(ptt,"$TITLE$");
  if(tt)
  {
   *tt=0; tt+=7;
   html << ptt << title;
  }
  else tt=ptt;

  char *quote_beg=strstr(q_buffer,quotekey);
  if(quote_beg)
  {
   quote_beg+=strlen(quotekey);
   char *quote_end=strstr(quote_beg,"$$");
   if(quote_end) *quote_end=0;
  }
  else quote_beg=q_buffer;
  
  char *qt=strstr(tt,"$QUOTE$");
  if(qt)
  {
   *qt=0; qt+=7;
   html << tt << quote_beg;
  }
  else qt=tt;
  
  char *bt=strstr(qt,"$BODY$");
  if(bt)
  {
   *bt=0;
   html << qt;
  }

  free(p_buffer);
  free(q_buffer);
}

void EndPage (PStringStream &html, PString copyr) 
{
  long p_size;
  char * p_buffer;
  FILE *fs;

  fs=fopen("template.html","r"); if(!fs) return;
  fseek(fs,0L,SEEK_END); p_size=ftell(fs); rewind(fs);
  p_buffer=new char[p_size];
  fread(p_buffer,1,p_size,fs); p_buffer[p_size]=0;
  fclose(fs);

  char *bt=strstr(p_buffer,"$BODY$");
  if(bt)
  {
   bt+=6;
   char *ct=strstr(bt,"$COPYRIGHT$");
   if(ct)
   {
    *ct=0; ct+=11;
    html << bt << copyr << ct;
   }
  }

  free(p_buffer);
}
