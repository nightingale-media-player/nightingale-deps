/* Automatically generated.  Do not edit */
/* See the mkopcodeh.awk script for details */
#define OP_VRowid                               1
#define OP_VFilter                              2
#define OP_ContextPop                           3
#define OP_IntegrityCk                          4
#define OP_DropTrigger                          5
#define OP_DropIndex                            6
#define OP_IdxInsert                            7
#define OP_Delete                               8
#define OP_MoveGt                               9
#define OP_OpenEphemeral                       10
#define OP_VerifyCookie                        11
#define OP_Push                                12
#define OP_Dup                                 13
#define OP_Blob                                14
#define OP_FifoWrite                           15
#define OP_IdxGT                               17
#define OP_RowKey                              18
#define OP_IsUnique                            19
#define OP_SetNumColumns                       20
#define OP_Eq                                  68   /* same as TK_EQ       */
#define OP_VUpdate                             21
#define OP_Expire                              22
#define OP_NullRow                             23
#define OP_OpenPseudo                          24
#define OP_OpenWrite                           25
#define OP_OpenRead                            26
#define OP_Transaction                         27
#define OP_AutoCommit                          28
#define OP_Negative                            85   /* same as TK_UMINUS   */
#define OP_Pop                                 29
#define OP_Halt                                30
#define OP_Vacuum                              31
#define OP_IfMemNeg                            32
#define OP_RowData                             33
#define OP_NotExists                           34
#define OP_MoveLe                              35
#define OP_SetCookie                           36
#define OP_Variable                            37
#define OP_VNext                               38
#define OP_VDestroy                            39
#define OP_TableLock                           40
#define OP_MemMove                             41
#define OP_LoadAnalysis                        42
#define OP_IdxDelete                           43
#define OP_Sort                                44
#define OP_ResetCount                          45
#define OP_NotNull                             66   /* same as TK_NOTNULL  */
#define OP_Ge                                  72   /* same as TK_GE       */
#define OP_Remainder                           82   /* same as TK_REM      */
#define OP_Divide                              81   /* same as TK_SLASH    */
#define OP_Integer                             46
#define OP_AggStep                             47
#define OP_CreateIndex                         48
#define OP_NewRowid                            49
#define OP_MoveLt                              50
#define OP_Explain                             51
#define OP_And                                 61   /* same as TK_AND      */
#define OP_ShiftLeft                           76   /* same as TK_LSHIFT   */
#define OP_Real                               125   /* same as TK_FLOAT    */
#define OP_Return                              52
#define OP_MemLoad                             53
#define OP_IdxLT                               54
#define OP_Rewind                              55
#define OP_MakeIdxRec                          56
#define OP_Gt                                  69   /* same as TK_GT       */
#define OP_AddImm                              57
#define OP_Subtract                            79   /* same as TK_MINUS    */
#define OP_Null                                58
#define OP_VColumn                             59
#define OP_MemNull                             62
#define OP_MemIncr                             63
#define OP_Clear                               64
#define OP_IsNull                              65   /* same as TK_ISNULL   */
#define OP_If                                  73
#define OP_ToBlob                             139   /* same as TK_TO_BLOB  */
#define OP_RealAffinity                        84
#define OP_Callback                            86
#define OP_AggFinal                            89
#define OP_IfMemZero                           90
#define OP_Last                                91
#define OP_Rowid                               92
#define OP_Sequence                            93
#define OP_NotFound                            94
#define OP_MakeRecord                          95
#define OP_ToText                             138   /* same as TK_TO_TEXT  */
#define OP_BitAnd                              74   /* same as TK_BITAND   */
#define OP_Add                                 78   /* same as TK_PLUS     */
#define OP_HexBlob                            126   /* same as TK_BLOB     */
#define OP_String                              96
#define OP_Goto                                97
#define OP_VCreate                             98
#define OP_MemInt                              99
#define OP_IfMemPos                           100
#define OP_DropTable                          101
#define OP_IdxRowid                           102
#define OP_Insert                             103
#define OP_Column                             104
#define OP_Noop                               105
#define OP_Not                                 16   /* same as TK_NOT      */
#define OP_Le                                  70   /* same as TK_LE       */
#define OP_BitOr                               75   /* same as TK_BITOR    */
#define OP_Multiply                            80   /* same as TK_STAR     */
#define OP_String8                             88   /* same as TK_STRING   */
#define OP_VOpen                              106
#define OP_CreateTable                        107
#define OP_Found                              108
#define OP_Distinct                           109
#define OP_Close                              110
#define OP_Statement                          111
#define OP_IfNot                              112
#define OP_ToInt                              141   /* same as TK_TO_INT   */
#define OP_Pull                               113
#define OP_VBegin                             114
#define OP_MemMax                             115
#define OP_MemStore                           116
#define OP_Next                               117
#define OP_Prev                               118
#define OP_MoveGe                             119
#define OP_Lt                                  71   /* same as TK_LT       */
#define OP_Ne                                  67   /* same as TK_NE       */
#define OP_MustBeInt                          120
#define OP_ForceInt                           121
#define OP_ShiftRight                          77   /* same as TK_RSHIFT   */
#define OP_CollSeq                            122
#define OP_Gosub                              123
#define OP_ContextPush                        124
#define OP_FifoRead                           127
#define OP_ParseSchema                        128
#define OP_Destroy                            129
#define OP_IdxGE                              130
#define OP_ReadCookie                         131
#define OP_BitNot                              87   /* same as TK_BITNOT   */
#define OP_AbsValue                           132
#define OP_Or                                  60   /* same as TK_OR       */
#define OP_ToReal                             142   /* same as TK_TO_REAL  */
#define OP_ToNumeric                          140   /* same as TK_TO_NUMERIC*/
#define OP_Function                           133
#define OP_Concat                              83   /* same as TK_CONCAT   */
#define OP_Int64                              134

/* The following opcode values are never used */
#define OP_NotUsed_135                        135
#define OP_NotUsed_136                        136
#define OP_NotUsed_137                        137

/* Opcodes that are guaranteed to never push a value onto the stack
** contain a 1 their corresponding position of the following mask
** set.  See the opcodeNoPush() function in vdbeaux.c  */
#define NOPUSH_MASK_0 0x9fec
#define NOPUSH_MASK_1 0xfffb
#define NOPUSH_MASK_2 0xbddd
#define NOPUSH_MASK_3 0xb2d4
#define NOPUSH_MASK_4 0xffff
#define NOPUSH_MASK_5 0x4ef7
#define NOPUSH_MASK_6 0xf6b6
#define NOPUSH_MASK_7 0x1fff
#define NOPUSH_MASK_8 0x7c05
#define NOPUSH_MASK_9 0x0000
