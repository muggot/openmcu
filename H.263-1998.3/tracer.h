#ifndef H263_1998_TRACE_H
#define H263_1998_TRACE_H

#if ! defined TRACE_FILE

#define	DECLARE_TRACER 
#define TRACE_AND_LOG(tracer, level, text) 
#define CODEC_TRACER(tracer, text) 

#else

#include <string>
#include <sstream>
#include <iostream>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TRACE_AND_LOG(tracer, level, text) \
    TRACE(level, tracer.GetPrefix() << "\t" << tracer.GetDirection() << "\t" << text); \
    tracer.Start() << text; tracer.End(); \

#define CODEC_TRACER(tracer, text) \
    tracer.Start() << text; tracer.End(); \

class Tracer
{
  public:
    Tracer(const char * prefix, bool encoder)
      : m_prefix(prefix), m_encoder(encoder)
    {
      file = -1;
      Open();
    }

    ~Tracer()
    {
      Close();
    }

    std::ostream & GetStream() 
    { return stream; }

    const char * GetPrefix() const
    { return m_prefix; }

    const char * GetDirection() const
    { return m_encoder ? "Encoder" : "Decoder"; }

    bool Open()
    {
      {
        WaitAndSignal m(mutex);
        if (file != -1) 
          return true;
      
        WaitAndSignal m2(seqMutex);
        if (sequence == 0) 
          baseTime = time(NULL);
        char name[100];
        sprintf(name, "h263_%u_%i.log", baseTime, ++sequence);
        file = creat(name, 0777);

        if (file == -1) {
std::cerr << "Code trace file " << name << " could not be opened" << std::endl;
          return false;
        }
std::cerr << "Code trace file " << name << " opened" << std::endl;
      }


      Start() << m_prefix << (m_encoder ? "En" : "De") << "coder log started";
      End();
      
      return true;
    }

    bool Close()
    {
      Start() << "Log ended";
      End();
      WaitAndSignal m(mutex);
      ::close(file);
      file = -1;
      return true;
    }

    std::ostream & Start()
    {
      time_t t = time(NULL);
      struct tm * tv = localtime(&t);
      char buffer[100];
      strftime(buffer, sizeof(buffer), "%H:%I:%S", tv);
      mutex.Wait();

      stream << buffer << " ";

      return stream;
    }

    void End()
    {
      stream << std::endl;
      std::string str = stream.str();
      ::write(file, str.c_str(), str.length());
      stream.clear();
      stream.str("");
      mutex.Signal();
    }

  protected:
    const char * m_prefix;
    bool m_encoder;
    int file;
    CriticalSection mutex;
    std::stringstream stream;
    static CriticalSection seqMutex;
    static int sequence;
    static time_t baseTime;
};

#define	DECLARE_TRACER \
  CriticalSection Tracer::seqMutex; \
  int Tracer::sequence = 0; \
  time_t Tracer::baseTime; \

#endif  // TRACE_FILE

#endif // H263_1998_TRACE_H
