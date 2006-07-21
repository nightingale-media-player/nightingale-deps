
/* Generated data (by glib-mkenums) */

#include "totem-pl-parser.h"
#include "totem-pl-parser-builtins.h"

/* enumerations from "totem-pl-parser.h" */
GType
totem_pl_parser_result_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { TOTEM_PL_PARSER_RESULT_UNHANDLED, "TOTEM_PL_PARSER_RESULT_UNHANDLED", "unhandled" },
      { TOTEM_PL_PARSER_RESULT_ERROR, "TOTEM_PL_PARSER_RESULT_ERROR", "error" },
      { TOTEM_PL_PARSER_RESULT_SUCCESS, "TOTEM_PL_PARSER_RESULT_SUCCESS", "success" },
      { TOTEM_PL_PARSER_RESULT_IGNORED, "TOTEM_PL_PARSER_RESULT_IGNORED", "ignored" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("TotemPlParserResult", values);
  }
  return etype;
}
GType
totem_pl_parser_type_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { TOTEM_PL_PARSER_PLS, "TOTEM_PL_PARSER_PLS", "pls" },
      { TOTEM_PL_PARSER_M3U, "TOTEM_PL_PARSER_M3U", "m3u" },
      { TOTEM_PL_PARSER_M3U_DOS, "TOTEM_PL_PARSER_M3U_DOS", "m3u-dos" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("TotemPlParserType", values);
  }
  return etype;
}
GType
totem_pl_parser_error_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { TOTEM_PL_PARSER_ERROR_VFS_OPEN, "TOTEM_PL_PARSER_ERROR_VFS_OPEN", "open" },
      { TOTEM_PL_PARSER_ERROR_VFS_WRITE, "TOTEM_PL_PARSER_ERROR_VFS_WRITE", "write" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("TotemPlParserError", values);
  }
  return etype;
}

/* Generated data ends here */

