#ifndef _PROJECTCONTEXT_H_
#define _PROJECTCONTEXT_H_

#include "wdltypes.h"
#include "wdlcstring.h"

class WDL_String;
class WDL_FastString;
class WDL_HeapBuf;
class WDL_FastQueue;

#ifndef _REAPER_PLUGIN_PROJECTSTATECONTEXT_DEFINED_
#define _WDL_PROJECTSTATECONTEXT_DEFINED_

class ProjectStateContext // this is also defined in reaper_plugin.h (keep them identical, thx)
{
public:
  virtual ~ProjectStateContext() {};

  virtual void WDL_VARARG_WARN(printf,2,3) AddLine(const char *fmt, ...) = 0;
  virtual int GetLine(char *buf, int buflen)=0; // returns -1 on eof

  virtual WDL_INT64 GetOutputSize()=0;

  virtual int GetTempFlag()=0;
  virtual void SetTempFlag(int flag)=0;
};

#endif

ProjectStateContext *ProjectCreateFileRead(const char *fn);
ProjectStateContext *ProjectCreateFileWrite(const char *fn);
ProjectStateContext *ProjectCreateMemCtx(WDL_HeapBuf *hb); // read or write (ugh, deprecated), be sure to delete it before accessing hb
ProjectStateContext *ProjectCreateMemCtx_Read(const WDL_HeapBuf *hb); // read only
ProjectStateContext *ProjectCreateMemCtx_Write(WDL_HeapBuf *hb); // write only, be sure to delete it before accessing hb
ProjectStateContext *ProjectCreateMemWriteFastQueue(WDL_FastQueue *fq); // only write! no need to do anything at all before accessing (can clear/reuse as necessary)


// helper functions
class LineParser;
bool ProjectContext_EatCurrentBlock(ProjectStateContext *ctx); // returns TRUE if got valid >, otherwise it means eof...
bool ProjectContext_GetNextLine(ProjectStateContext *ctx, LineParser *lpOut); // true if lpOut is valid


int cfg_decode_binary(ProjectStateContext *ctx, WDL_HeapBuf *hb); // 0 on success, doesnt clear hb
void cfg_encode_binary(ProjectStateContext *ctx, const void *ptr, int len);

int cfg_decode_textblock(ProjectStateContext *ctx, WDL_String *str); // 0 on success, appends to str
int cfg_decode_textblock(ProjectStateContext *ctx, WDL_FastString *str); // 0 on success, appends to str
void cfg_encode_textblock(ProjectStateContext *ctx, const char *text);

void makeEscapedConfigString(const char *in, WDL_String *out);


class ProjectStateContext_GenericRead : public ProjectStateContext
{
  const char *m_ptr;
  const char *m_endptr;
  int m_tmpflag;

public:
  ProjectStateContext_GenericRead(const void *buf, int sz) : m_tmpflag(0)
  {
    m_ptr = (const char *)buf;
    m_endptr = m_ptr ? m_ptr+sz : NULL;
  }
  virtual ~ProjectStateContext_GenericRead() {}

  virtual void WDL_VARARG_WARN(printf,2,3) AddLine(const char *fmt, ...) { }
  virtual int GetLine(char *buf, int buflen) // returns -1 on eof
  {
    while (m_ptr < m_endptr && (!*m_ptr || *m_ptr == '\t' || *m_ptr == '\r' || *m_ptr == '\n' || *m_ptr == ' ')) m_ptr++;
    if (m_ptr >= m_endptr) return -1;
    lstrcpyn_safe(buf,m_ptr,buflen);
    m_ptr += strlen(m_ptr)+1;
    return 0;
  }
  virtual int GetTempFlag() { return m_tmpflag; }
  virtual void SetTempFlag(int flag) { m_tmpflag=flag; }
  virtual WDL_INT64 GetOutputSize() { return 0; }
};


#endif//_PROJECTCONTEXT_H_
