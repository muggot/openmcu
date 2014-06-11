#ifndef UTILS_H
#define UTILS_H

#define SPACE_CHARS " \t\r\n"

// from ffmpeg avutil/avstring.h
static inline int av_toupper(int c)
{
  if(c >= 'a' && c <= 'z')
    c ^= 0x20;
  return c;
}

// from ffmpeg libavformat/internal.h
static inline int ff_hex_to_data(uint8_t *data, const char *p)
{
  int c, len, v;

  len = 0;
  v = 1;
  for(;;)
  {
    p += strspn(p, SPACE_CHARS);
    if(*p == '\0')
      break;
    c = av_toupper((unsigned char) *p++);
    if(c >= '0' && c <= '9')
      c = c - '0';
    else if (c >= 'A' && c <= 'F')
      c = c - 'A' + 10;
    else
      break;
    v = (v << 4) | c;
    if(v & 0x100)
    {
      if(data)
        data[len] = v;
      len++;
      v = 1;
    }
  }
  return len;
}

#endif // UTILS_H
