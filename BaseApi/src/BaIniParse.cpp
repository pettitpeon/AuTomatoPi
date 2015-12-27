/*---------------------------- Includes ------------------------------------*/

#include <map>
#include <string>
#include <sstream>
#include <string.h>

#include "BaGenMacros.h"
#include "BaIniParse.h"
#include "BaUtils.hpp"


/*---------------------------- Defines -------------------------------------*/
#define ASCIILINESZ         (1024)
//#define INI_INVALID_KEY     "*Invalid*)"

/*---------------------------------------------------------------------------
                        Private to this module
 ---------------------------------------------------------------------------*/

// This enum stores the status for each parsed line (internal use only).
typedef enum line_status {
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} line_status;

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

    sta = LINE_UNPROCESSED ;
    if (len<1) {
        /* Empty line */
        sta = LINE_EMPTY ;
    } else if (line[0]=='#' || line[0]==';') {
        /* Comment line */
        sta = LINE_COMMENT ;
    } else if (line[0]=='[' && line[len-1]==']') {
        /* Section name */
        sscanf(line, "[%[^]]", section);
//        strcpy(section, line);
        strstrip(section);
        strlwc(section, section, len);
        sta = LINE_SECTION ;
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
        sta = LINE_VALUE ;
    } else if (sscanf (line, "%[^=] = %[^;#]", key, value) == 2) {
        /* Usual key=value without quotes, with or without comments */
        strstrip(key);
        strlwc(key, key, len);
        strstrip(value);
        
        sta = LINE_VALUE ;
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
        sta = LINE_VALUE ;
    } else {
        /* Generate syntax error */
        sta = LINE_ERROR ;
    }

    free(line);
    return sta ;
}



// /////////////////////////////////////////////////////////////////////////////

class CBaIniParser : public IBaIniParser {
public:

   //
   static IBaIniParser * Create(const char *file) {
      CBaIniParser *pIp = new CBaIniParser();
      FILE * in ;

      char line    [ASCIILINESZ+1] ;
      char section [ASCIILINESZ+1] ;
      char key     [ASCIILINESZ+1] ;
      char tmp     [(ASCIILINESZ * 2) + 1] ;
      char val     [ASCIILINESZ+1] ;

      int  last=0 ;
      int  len ;
      int  lineno=0 ;
      int  errs=0;


      if (!pIp || !(in = fopen(file, "r"))) {
         // todo: no printf
//         fprintf(stderr, "iniparser: cannot open %s\n", file);
         return 0;
      }

      memset(line,    0, ASCIILINESZ);
      memset(section, 0, ASCIILINESZ);
      memset(key,     0, ASCIILINESZ);
      memset(val,     0, ASCIILINESZ);
      last=0 ;

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
            last=len ;
            continue ;
         } else {
            last=0 ;
         }

         switch (iniparser_line(line, section, key, val)) {
         case LINE_EMPTY:
         case LINE_COMMENT:
            break ;

         case LINE_SECTION:
            // Todo: is there any other way to identify a section?
            pIp->dic[section] = "";
            break ;

         case LINE_VALUE:
            sprintf(tmp, "%s:%s", section, key);
            pIp->dic[tmp] = val;
            break ;

         case LINE_ERROR:
            // todo: no printf
//            fprintf(stderr, "iniparser: syntax error in %s (%d):\n", file, lineno);
//            fprintf(stderr, "-> %s\n", line);
            errs++ ;
            break;

         default:
            break ;
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

      DumpSecLess(f);
      for (auto kv : dic) {

         // find returns 0 if : is in position zero. This must be handled
         // TODO: find is broken in minGW check why
         auto rc = kv.first.find(':', 0);
         if (kv.first[0] != ':' && (!rc || rc == UINT32_MAX)) {
            DumpIniSec(kv.first.c_str(), f);
         }
      }
      fprintf(f, "\n");

      return ;

   }

   // TODO: check that empty section is allowed
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
   virtual bool DumpSecLess(FILE *f) {

      return DumpIniSec("", f);
   }

   //
   virtual std::string GetString(const char *key, const char *def) {
      auto it = dic.find(key);
      if (it != dic.end()) {
         return it->second;
      }

      return def;
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
      return BaToNumber(val.c_str(), def);
   };

   //
   virtual double GetDouble(const char *key, double def) {
      std::string val = GetString(key, "!");
      return BaToNumber(val.c_str(), def);
   };

   //
   virtual bool Set(const char *key, const char *val) {
      if (!key || !val) {
         return false;
      }

      dic[key] = val;
      return true;
   }

   //
   virtual bool Reset(const char *key) {
      if (!key) {
         return false;
      }

      // TODO: Check why several elemets could be erased
      return dic.erase(key);
   }

   //
   virtual bool Exists(const char *key) {
      if (!key) {
         return false;
      }

      return dic.find(key) != dic.end();
   };

   // Members
   std::map<std::string, std::string> dic;
};

IBaIniParser * BaIniParserCreate(const char *file) {
   return CBaIniParser::Create(file);
}

bool BaIniParserDestroy(IBaIniParser *pHdl) {
   return CBaIniParser::Destroy(pHdl);
}
