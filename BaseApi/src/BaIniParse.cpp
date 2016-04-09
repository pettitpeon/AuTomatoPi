/*------------------------------------------------------------------------------
    Includes
 -----------------------------------------------------------------------------*/
#include <map>
#include <string>
#include <sstream>
#include <string.h>
#include <algorithm>

#include "BaGenMacros.h"
#include "BaIniParse.h"
#include "BaUtils.hpp"

/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/
#define ASCIILINESZ  (1024)                // Line size for C functions
#define STRNOTFOUND  std::string::npos     // Return value for string not found
#define C_HDL_       ((CBaIniParser*) hdl) // Shortcut for C handle

/*------------------------------------------------------------------------------
    Type definitions
 -----------------------------------------------------------------------------*/
// This enum stores the status for each parsed line (internal use only).
typedef enum line_status {
    eLINE_UNPROCESSED,
    eLINE_ERROR,
    eLINE_EMPTY,
    eLINE_COMMENT,
    eLINE_SECTION,
    eLINE_VALUE
} line_status;

/*------------------------------------------------------------------------------
    Declarations
 -----------------------------------------------------------------------------*/
LOCAL const char* strlwc(const char *in, char *out, unsigned len);
LOCAL char* xstrdup(const char * s);
LOCAL unsigned strstrip(char *s);
LOCAL line_status iniparser_line(const char *input_line, char *section,
      char *key, char *value);


/*------------------------------------------------------------------------------
    C++ Interface
 -----------------------------------------------------------------------------*/
class CBaIniParser : public IBaIniParser {
public:

   //
   static IBaIniParser * Create(const char *file) {
      CBaIniParser *pIp = new CBaIniParser();
      if (!file) {
         return pIp;
      }
      FILE * in ;

      char line    [ASCIILINESZ+1];
      char section [ASCIILINESZ+1];
      char key     [ASCIILINESZ+1];
      char tmp     [(ASCIILINESZ * 2) + 1];
      char val     [ASCIILINESZ+1];

      int last = 0 ;
      int len;
      int lineno = 0 ;
      int errs = 0;


      if (!pIp || !(in = fopen(file, "r"))) {
         // todo: no printf
//         fprintf(stderr, "iniparser: cannot open %s\n", file);
         return 0;
      }

      memset(line,    0, ASCIILINESZ);
      memset(section, 0, ASCIILINESZ);
      memset(key,     0, ASCIILINESZ);
      memset(val,     0, ASCIILINESZ);
      last = 0;

      while (fgets(line+last, ASCIILINESZ-last, in) != NULL) {
         lineno++ ;
         len = (int)strlen(line)-1;
         if (len == 0) { continue; }
         /* Safety check against buffer overflows */
         if (line[len]!='\n' && !feof(in)) {
            // todo: no printf
//            fprintf(stderr, "iniparser: input line too long in %s (%d)\n",
//                  file, lineno);
            Destroy(pIp);
            fclose(in);
            return NULL ;
         }

         /* Get rid of \n and spaces at end of line */
         while ((len>=0) && ((line[len]=='\n') || (isspace(line[len])))) {
            line[len]=0 ;
            len-- ;
         }

         if (len < 0) { /* Line was entirely \n and/or spaces */
            len = 0;
         }

         /* Detect multi-line */
         if (line[len]=='\\') {
            /* Multi-line value */
            // Fixme: there is an issue with windows paths and multi-lines =(
            // This issue has to be looked at under windows
#ifdef __linux
            last=len ;
            continue ;
#endif
            last=0;
         } else {
            last=0 ;
         }

         switch (iniparser_line(line, section, key, val)) {
         case eLINE_EMPTY:
         case eLINE_COMMENT:
            break;
         case eLINE_SECTION:
            pIp->dic[section] = "";
            break;
         case eLINE_VALUE:
            sprintf(tmp, "%s:%s", section, key);
            pIp->dic[tmp] = val;
            break;
         case eLINE_ERROR:
            // TODO: either log all errors or brake loop!!
            errs++;
            break;
         default:
            break;
         }
         memset(line, 0, ASCIILINESZ);
         last=0;

      }

      if (errs) {
         Destroy(pIp);
         pIp = 0;
      }

      fclose(in);
      return pIp;
   };

   //
   static bool Destroy(IBaIniParser *pHdl) {
      CBaIniParser *p = dynamic_cast<CBaIniParser*>(pHdl);
      if (!p ) {
         return false;
      }
      delete p;
      return true;
   };

   //
   void Dump(FILE * f) {
      if (!f) {
         return;
      }

      for (auto kv : dic) {
         fprintf(f, "%s = %s\n", kv.first.c_str(), kv.second.c_str());
      }
   }

   //
   virtual void DumpIni(FILE *f) {
      if (!f) {
         return;
      }

      // Dump the section-less entries if exist
      DumpIniSecLess(f);

      // Find all entries that are section names and dump the sections
      for (auto kv : dic) {
         // If the key (first) does not have ':', it is a section name
         if (kv.first.find(':', 0) == STRNOTFOUND) {
            DumpIniSec(kv.first.c_str(), f);
         }
      }
      fprintf(f, "\n");

      return;

   }

   //
   virtual bool DumpIniSec(const char *sec, FILE *f) {

      // Check the arguments and allow nameless sections
      if (!sec || !f || (sec[0] && !Exists(sec))) {
         return false;
      }

      std::string keyt = sec;
      keyt.append(":");
      int secLen  = keyt.length();

      // Only print the section name if it has a section
      if (sec[0]) {
         fprintf(f, "\n[%s]\n", sec);
      }


      // Iterate all entries and dump only the ones in the section
      for (auto kv : dic) {
         if (!kv.first.compare(0, secLen, keyt)) {
            fprintf(f, "%s = %s\n", kv.first.c_str() + secLen, kv.second.c_str());
         }
      }

      fprintf(f, "\n");
      return true;
   }

   //
   virtual bool DumpIniSecLess(FILE *f) {

      return DumpIniSec("", f);
   }

   // Uses the general GetString() method and returns explicitly a copy of the
   // internal string to protect it
   virtual std::string GetString(const char *key, const char *def) {
      if (!def) {
         def = "";
      }

      if (!key) {
         return def;
      }

      // If key has no ':', prepend it. It is a section-less entry
      std::string tmpKey(key);
      std::string tmpDef(def);

      std::transform(tmpKey.begin(), tmpKey.end(), tmpKey.begin(), ::tolower);
      return GetString(tmpKey, tmpDef);
   };

   //
   virtual bool GetBool(const char *key, bool def) {
      std::string val = GetString(key, "!");

      const char *c = val.c_str();
      if (c[0]=='y' || c[0]=='Y' || c[0]=='1' || c[0]=='t' || c[0]=='T') {
          return true;
      } else if (c[0]=='!' || c[0]=='n' || c[0]=='N' || c[0]=='0' ||
                 c[0]=='f' || c[0]=='F') {
         return false;
      }

      return def;
   };

   //
   virtual int GetInt(const char *key, int def) {
      std::string val = GetString(key, "!");
      return BaToNumber(val.c_str(), (uint32_t)def);
   };

   //
   virtual double GetDouble(const char *key, double def) {
      std::string val = GetString(key, "!");
      return BaToNumber(val.c_str(), def);
   };

   //
   virtual bool Set(const char *key, const char *val) {
      if (!key) {
         return false;
      }

      dic[key] = val ? val : "";
      return true;
   }

   //
   virtual bool Reset(const char *key) {
      if (!key) {
         return false;
      }

      // TODO: Check why several elements could be erased
      return dic.erase(key);
   }

   //
   virtual bool Exists(const char *key) {
      if (!key) {
         return false;
      }

      return dic.find(key) != dic.end();
   };

   // This function has the logic of the GetString() method and returns a
   // reference to the internal string on purpose to be able to return the
   // internal C-string in the C-interface
   inline std::string & GetString(std::string &rKey, std::string &rDef) {

      // If key has no ':', prepend it. It is a section-less entry
      if (rKey.find(':') == STRNOTFOUND) {
         rKey = ":" + rKey;
      }

      // Find the entry
      auto it = dic.find(rKey);
      if (it != dic.end()) {
         return it->second;
      }

      // Entry not found
//      Dump(stdout); // For testing and debugging
      return rDef;
   }

private:
   // Members
   std::map<std::string, std::string> dic;
};

//
IBaIniParser * CBaIniParserCreate(const char *file) {
   return CBaIniParser::Create(file);
}

//
bool CBaIniParserDestroy(IBaIniParser *pHdl) {
   return CBaIniParser::Destroy(pHdl);
}

/*------------------------------------------------------------------------------
    C Interface
 -----------------------------------------------------------------------------*/
//
TBaIniParseHdl BaIniParseCreate(const char *file) {
   return CBaIniParserCreate(file);
}

//
TBaBoolRC BaIniParseDestroy(TBaIniParseHdl hdl) {
   return CBaIniParserDestroy(C_HDL_) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
const char* BaIniParseGetString(TBaIniParseHdl hdl, const char *key, const char *def) {
   if (!def) {
      def = "";
   }

   if (!hdl || !key) {
      return def;
   }

   CBaIniParser *p = C_HDL_;

   // If key has no ':', prepend it. It is a section-less entry
   std::string tmpKey(key);
   std::string tmpDef(def);

   // Returns a pointer to the internal C-string!!!
   std::string &rRet = p->GetString(tmpKey, tmpDef);
   return &rRet != &tmpDef ? rRet.c_str() : def;
}

//
int BaIniParseGetInt(TBaIniParseHdl hdl, const char *key, int def) {
   if (!hdl) {
      return def;
   }

   return C_HDL_->GetInt(key, def);
}

//
double BaIniParseGetDouble(TBaIniParseHdl hdl, const char *key, double def) {
   if (!hdl) {
      return def;
   }

   return C_HDL_->GetDouble(key, def);
}

//
TBaBool BaIniParseGetBool(TBaIniParseHdl hdl, const char *key, TBaBool def) {
   if (!hdl) {
      return def;
   }

   return C_HDL_->GetBool(key, def) ? eBaBool_true : eBaBool_false;
}

//
TBaBoolRC BaIniParseSet(TBaIniParseHdl hdl, const char *key, const char *val) {
   return C_HDL_->Set(key, val) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIniParseReset(TBaIniParseHdl hdl, const char *key) {
   return C_HDL_->Reset(key) ? eBaBoolRC_Success : eBaBoolRC_Error;
}

//
TBaBoolRC BaIniParseExists(TBaIniParseHdl hdl, const char *key) {
   return C_HDL_->Exists(key) ? eBaBool_true : eBaBool_false;
}

//
void BaIniParseDumpIni(TBaIniParseHdl hdl, FILE * f) {
   C_HDL_->DumpIni(f);
}

//
TBaBool BaIniParseDumpIniSec(TBaIniParseHdl hdl, const char *sec, FILE * f) {
   return C_HDL_->DumpIniSec(sec, f) ? eBaBool_true : eBaBool_false;
}

//
TBaBool BaIniParseDumpIniSecLess(TBaIniParseHdl hdl, FILE * f) {
   return C_HDL_->DumpIniSecLess(f) ? eBaBool_true : eBaBool_false;
}

//
void BaIniParseDump(TBaIniParseHdl hdl, FILE * f) {
   C_HDL_->Dump(f);
}



/*------------------------------------------------------------------------------
    Local functions
 -----------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*
  @brief    Convert a string to lowercase.
  @param    in   String to convert.
  @param    out Output buffer.
  @param    len Size of the out buffer.
  @return   ptr to the out buffer or NULL if an error occured.

  This function convert a string into lowercase.
  At most len - 1 elements of the input string will be converted.
 */
/*--------------------------------------------------------------------------*/
LOCAL const char *strlwc(const char *in, char *out, unsigned len) {
    unsigned i ;

    if (in==NULL || out == NULL || len==0) return NULL ;
    i=0 ;
    while (in[i] != '\0' && i < len-1) {
        out[i] = (char)tolower((int)in[i]);
        i++ ;
    }
    out[i] = '\0';
    return out ;
}

/*-------------------------------------------------------------------------*/
/*
  @brief    Duplicate a string
  @param    s String to duplicate
  @return   Pointer to a newly allocated string, to be freed with free()

  This is a replacement for strdup(). This implementation is provided
  for systems that do not have it.
 */
/*--------------------------------------------------------------------------*/
LOCAL char * xstrdup(const char * s) {
    char * t ;
    size_t len ;
    if (!s)
        return NULL ;

    len = strlen(s) + 1 ;
    t = (char*) malloc(len) ;
    if (t) {
        memcpy(t, s, len) ;
    }
    return t ;
}

/*-------------------------------------------------------------------------*/
/*
  @brief    Remove blanks at the beginning and the end of a string.
  @param    str  String to parse and alter.
  @return   unsigned New size of the string.
 */
/*--------------------------------------------------------------------------*/
LOCAL unsigned strstrip(char *s) {
    char *last = NULL ;
    char *dest = s;


    if (s==NULL) {
       return 0;
    }

    last = s + strlen(s);
    while (isspace((int)*s) && *s) s++;
    while (last > s) {
        if (!isspace((int)*(last-1)))
            break ;
        last -- ;
    }
    *last = (char)0;

    memmove(dest,s,last - s + 1);
    return last - s;
}

/*-------------------------------------------------------------------------*/
/*
  @brief    Load a single line from an INI file
  @param    input_line  Input line, may be concatenated multi-line input
  @param    section     Output space to store section
  @param    key         Output space to store key
  @param    value       Output space to store value
  @return   line_status value
 */
/*--------------------------------------------------------------------------*/
LOCAL line_status iniparser_line(const char *input_line, char *section,
    char *key, char *value
    ) {
    line_status sta ;
    char * line = NULL;
    size_t      len ;

    line = xstrdup(input_line);
    len = strstrip(line);

    sta = eLINE_UNPROCESSED ;
    if (len<1) {
        /* Empty line */
        sta = eLINE_EMPTY ;
    } else if (line[0]=='#' || line[0]==';') {
        /* Comment line */
        sta = eLINE_COMMENT ;
    } else if (line[0]=='[' && line[len-1]==']') {
        /* Section name */
        sscanf(line, "[%[^]]", section);
//        strcpy(section, line);
        strstrip(section);
        strlwc(section, section, len);
        sta = eLINE_SECTION ;
    } else if (sscanf (line, "%[^=] = \"%[^\"]\"", key, value) == 2
           ||  sscanf (line, "%[^=] = '%[^\']'",   key, value) == 2) {
        /* Usual key=value with quotes, with or without comments */
        strstrip(key);
        strlwc(key, key, len);
        /* Don't strip spaces from values surrounded with quotes */
        /*
         * sscanf cannot handle '' or "" as empty values
         * this is done here
         */
        if (!strcmp(value, "\"\"") || (!strcmp(value, "''"))) {
            value[0]=0 ;
        }
        sta = eLINE_VALUE ;
    } else if (sscanf (line, "%[^=] = %[^;#]", key, value) == 2) {
        /* Usual key=value without quotes, with or without comments */
        strstrip(key);
        strlwc(key, key, len);
        strstrip(value);

        sta = eLINE_VALUE ;
    } else if (sscanf(line, "%[^=] = %[;#]", key, value)==2
           ||  sscanf(line, "%[^=] %[=]", key, value) == 2) {
        /*
         * Special cases:
         * key=
         * key=;
         * key=#
         */
        strstrip(key);
        strlwc(key, key, len);
        value[0]=0 ;
        sta = eLINE_VALUE ;
    } else {
        /* Generate syntax error */
        sta = eLINE_ERROR ;
    }

    free(line);
    return sta ;
}

