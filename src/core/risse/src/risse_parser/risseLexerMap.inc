static int RisseMapToken(const risse_char * & ptr, tRisseVariant &value)
{
 const risse_char * p = ptr;

 switch(p[0]) {
 case '&':
  switch(p[1]) {
  case '&':
   switch(p[2]) {
   default:
    { ptr += 2; return T_LOGICALAND; }
   case '=':
    { ptr += 3; return T_LOGICALANDEQUAL; }
   }
  default:
   { ptr += 1; return T_AMPERSAND; }
  case '=':
   { ptr += 2; return T_AMPERSANDEQUAL; }
  }
 case '<':
  switch(p[1]) {
  case '-':
   switch(p[2]) {
   case '>':
    { ptr += 3; return T_SWAP; }
   }
  case '<':
   switch(p[2]) {
   default:
    { ptr += 2; return T_LARITHSHIFT; }
   case '=':
    { ptr += 3; return T_LARITHSHIFTEQUAL; }
   }
  default:
   { ptr += 1; return T_LT; }
  case '%':
   { ptr += 2; return T_BEGIN_OCTET_LITERAL; }
  case '=':
   { ptr += 2; return T_LTOREQUAL; }
  }
 case '.':
  switch(p[1]) {
  case '.':
   switch(p[2]) {
   case '.':
    { ptr += 3; return T_OMIT; }
   }
  default:
   { ptr += 1; return T_DOT; }
  }
 case '=':
  switch(p[1]) {
  case '=':
   switch(p[2]) {
   default:
    { ptr += 2; return T_EQUALEQUAL; }
   case '=':
    { ptr += 3; return T_DISCEQUAL; }
   }
  default:
   { ptr += 1; return T_EQUAL; }
  case '>':
   { ptr += 2; return T_COMMA; }
  }
 case '!':
  switch(p[1]) {
  case '=':
   switch(p[2]) {
   default:
    { ptr += 2; return T_NOTEQUAL; }
   case '=':
    { ptr += 3; return T_DISCNOTEQUAL; }
   }
  default:
   { ptr += 1; return T_EXCRAMATION; }
  }
 case '>':
  switch(p[1]) {
  case '>':
   switch(p[2]) {
   case '>':
    switch(p[3]) {
    default:
     { ptr += 3; return T_RBITSHIFT; }
    case '=':
     { ptr += 4; return T_RBITSHIFTEQUAL; }
    }
   default:
    { ptr += 2; return T_RARITHSHIFT; }
   case '=':
    { ptr += 3; return T_RARITHSHIFTEQUAL; }
   }
  default:
   { ptr += 1; return T_GT; }
  case '=':
   { ptr += 2; return T_GTOREQUAL; }
  }
 case 'N':
  switch(p[1]) {
  case 'a':
   switch(p[2]) {
   case 'N':
    if(!Risse_iswalnum_nc(p[3])) { ptr += 3; return T_NAN; }
    goto cut_word;
   }
  }
 case 'f':
  switch(p[1]) {
  case 'a':
   switch(p[2]) {
   case 'l':
    switch(p[3]) {
    case 's':
     switch(p[4]) {
     case 'e':
      if(!Risse_iswalnum_nc(p[5])) { ptr += 5; return T_FALSE; }
      goto cut_word;
     }
    }
   }
  case 'i':
   switch(p[2]) {
   case 'n':
    switch(p[3]) {
    case 'a':
     switch(p[4]) {
     case 'l':
      switch(p[5]) {
      default:
       if(!Risse_iswalnum_nc(p[6])) { ptr += 5; return T_FINAL; }
       goto cut_word;
      case 'l':
       switch(p[6]) {
       case 'y':
        if(!Risse_iswalnum_nc(p[7])) { ptr += 7; return T_FINALLY; }
        goto cut_word;
       }
      }
     }
    }
   }
  case 'u':
   switch(p[2]) {
   case 'n':
    switch(p[3]) {
    case 'c':
     switch(p[4]) {
     case 't':
      switch(p[5]) {
      case 'i':
       switch(p[6]) {
       case 'o':
        switch(p[7]) {
        case 'n':
         if(!Risse_iswalnum_nc(p[8])) { ptr += 8; return T_FUNCTION; }
         goto cut_word;
        }
       }
      }
     }
    }
   }
  case 'o':
   switch(p[2]) {
   case 'r':
    if(!Risse_iswalnum_nc(p[3])) { ptr += 3; return T_FOR; }
    goto cut_word;
   }
  }
 case 'o':
  switch(p[1]) {
  case 'c':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 'e':
     switch(p[4]) {
     case 't':
      if(!Risse_iswalnum_nc(p[5])) { ptr += 5; return T_OCTET; }
      goto cut_word;
     }
    }
   }
  }
 case 'r':
  switch(p[1]) {
  case 'e':
   switch(p[2]) {
   case 'a':
    switch(p[3]) {
    case 'l':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_REAL; }
     goto cut_word;
    }
   case 't':
    switch(p[3]) {
    case 'u':
     switch(p[4]) {
     case 'r':
      switch(p[5]) {
      case 'n':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_RETURN; }
       goto cut_word;
      }
     }
    }
   }
  }
 case 'i':
  switch(p[1]) {
  case 'f':
   if(!Risse_iswalnum_nc(p[2])) { ptr += 2; return T_IF; }
   goto cut_word;
  case 'm':
   switch(p[2]) {
   case 'p':
    switch(p[3]) {
    case 'o':
     switch(p[4]) {
     case 'r':
      switch(p[5]) {
      case 't':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_IMPORT; }
       goto cut_word;
      }
     }
    }
   }
  case 's':
   switch(p[2]) {
   case 'v':
    switch(p[3]) {
    case 'a':
     switch(p[4]) {
     case 'l':
      switch(p[5]) {
      case 'i':
       switch(p[6]) {
       case 'd':
        if(!Risse_iswalnum_nc(p[7])) { ptr += 7; return T_ISVALID; }
        goto cut_word;
       }
      }
     }
    }
   }
  case 'n':
   switch(p[2]) {
   default:
    if(!Risse_iswalnum_nc(p[3])) { ptr += 2; return T_IN; }
    goto cut_word;
   case 't':
    if(!Risse_iswalnum_nc(p[3])) { ptr += 3; return T_INT; }
    goto cut_word;
   case 'v':
    switch(p[3]) {
    case 'a':
     switch(p[4]) {
     case 'l':
      switch(p[5]) {
      case 'i':
       switch(p[6]) {
       case 'd':
        switch(p[7]) {
        case 'a':
         switch(p[8]) {
         case 't':
          switch(p[9]) {
          case 'e':
           if(!Risse_iswalnum_nc(p[10])) { ptr += 10; return T_INVALIDATE; }
           goto cut_word;
          }
         }
        }
       }
      }
     }
    }
   case 'c':
    switch(p[3]) {
    case 'o':
     switch(p[4]) {
     case 'n':
      switch(p[5]) {
      case 't':
       switch(p[6]) {
       case 'e':
        switch(p[7]) {
        case 'x':
         switch(p[8]) {
         case 't':
          switch(p[9]) {
          case 'o':
           switch(p[10]) {
           case 'f':
            if(!Risse_iswalnum_nc(p[11])) { ptr += 11; return T_INCONTEXTOF; }
            goto cut_word;
           }
          }
         }
        }
       }
      }
     }
    }
   case 's':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'a':
      switch(p[5]) {
      case 'n':
       switch(p[6]) {
       case 'c':
        switch(p[7]) {
        case 'e':
         switch(p[8]) {
         case 'o':
          switch(p[9]) {
          case 'f':
           if(!Risse_iswalnum_nc(p[10])) { ptr += 10; return T_INSTANCEOF; }
           goto cut_word;
          }
         }
        }
       }
      }
     }
    }
   }
  }
 case 'w':
  switch(p[1]) {
  case 'h':
   switch(p[2]) {
   case 'i':
    switch(p[3]) {
    case 'l':
     switch(p[4]) {
     case 'e':
      if(!Risse_iswalnum_nc(p[5])) { ptr += 5; return T_WHILE; }
      goto cut_word;
     }
    }
   }
  case 'i':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 'h':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_WITH; }
     goto cut_word;
    }
   }
  }
 case 'c':
  switch(p[1]) {
  case 'l':
   switch(p[2]) {
   case 'a':
    switch(p[3]) {
    case 's':
     switch(p[4]) {
     case 's':
      if(!Risse_iswalnum_nc(p[5])) { ptr += 5; return T_CLASS; }
      goto cut_word;
     }
    }
   }
  case 'o':
   switch(p[2]) {
   case 'n':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'i':
      switch(p[5]) {
      case 'n':
       switch(p[6]) {
       case 'u':
        switch(p[7]) {
        case 'e':
         if(!Risse_iswalnum_nc(p[8])) { ptr += 8; return T_CONTINUE; }
         goto cut_word;
        }
       }
      }
     }
    case 's':
     switch(p[4]) {
     case 't':
      if(!Risse_iswalnum_nc(p[5])) { ptr += 5; return T_CONST; }
      goto cut_word;
     }
    }
   }
  case 'a':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 'c':
     switch(p[4]) {
     case 'h':
      if(!Risse_iswalnum_nc(p[5])) { ptr += 5; return T_CATCH; }
      goto cut_word;
     }
    }
   case 's':
    switch(p[3]) {
    case 'e':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_CASE; }
     goto cut_word;
    }
   }
  }
 case 'g':
  switch(p[1]) {
  case 'l':
   switch(p[2]) {
   case 'o':
    switch(p[3]) {
    case 'b':
     switch(p[4]) {
     case 'a':
      switch(p[5]) {
      case 'l':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_GLOBAL; }
       goto cut_word;
      }
     }
    }
   }
  case 'o':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 'o':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_GOTO; }
     goto cut_word;
    }
   }
  case 'e':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'e':
      switch(p[5]) {
      case 'r':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_GETTER; }
       goto cut_word;
      }
     }
    }
   }
  }
 case 'e':
  switch(p[1]) {
  case 'l':
   switch(p[2]) {
   case 's':
    switch(p[3]) {
    case 'e':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_ELSE; }
     goto cut_word;
    }
   }
  case 'x':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 'e':
     switch(p[4]) {
     case 'n':
      switch(p[5]) {
      case 'd':
       switch(p[6]) {
       case 's':
        if(!Risse_iswalnum_nc(p[7])) { ptr += 7; return T_EXTENDS; }
        goto cut_word;
       }
      }
     }
    }
   case 'p':
    switch(p[3]) {
    case 'o':
     switch(p[4]) {
     case 'r':
      switch(p[5]) {
      case 't':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_EXPORT; }
       goto cut_word;
      }
     }
    }
   }
  case 'n':
   switch(p[2]) {
   case 'u':
    switch(p[3]) {
    case 'm':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_ENUM; }
     goto cut_word;
    }
   }
  }
 case 'I':
  switch(p[1]) {
  case 'n':
   switch(p[2]) {
   case 'f':
    switch(p[3]) {
    case 'i':
     switch(p[4]) {
     case 'n':
      switch(p[5]) {
      case 'i':
       switch(p[6]) {
       case 't':
        switch(p[7]) {
        case 'y':
         if(!Risse_iswalnum_nc(p[8])) { ptr += 8; return T_INFINITY; }
         goto cut_word;
        }
       }
      }
     }
    }
   }
  }
 case 'd':
  switch(p[1]) {
  case 'o':
   if(!Risse_iswalnum_nc(p[2])) { ptr += 2; return T_DO; }
   goto cut_word;
  case 'e':
   switch(p[2]) {
   case 'f':
    switch(p[3]) {
    case 'a':
     switch(p[4]) {
     case 'u':
      switch(p[5]) {
      case 'l':
       switch(p[6]) {
       case 't':
        if(!Risse_iswalnum_nc(p[7])) { ptr += 7; return T_DEFAULT; }
        goto cut_word;
       }
      }
     }
    }
   case 'l':
    switch(p[3]) {
    case 'e':
     switch(p[4]) {
     case 't':
      switch(p[5]) {
      case 'e':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_DELETE; }
       goto cut_word;
      }
     }
    }
   case 'b':
    switch(p[3]) {
    case 'u':
     switch(p[4]) {
     case 'g':
      switch(p[5]) {
      case 'g':
       switch(p[6]) {
       case 'e':
        switch(p[7]) {
        case 'r':
         if(!Risse_iswalnum_nc(p[8])) { ptr += 8; return T_DEBUGGER; }
         goto cut_word;
        }
       }
      }
     }
    }
   }
  }
 case 'v':
  switch(p[1]) {
  case 'o':
   switch(p[2]) {
   case 'i':
    switch(p[3]) {
    case 'd':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_VOID; }
     goto cut_word;
    }
   }
  case 'a':
   switch(p[2]) {
   case 'r':
    if(!Risse_iswalnum_nc(p[3])) { ptr += 3; return T_VAR; }
    goto cut_word;
   }
  }
 case 'b':
  switch(p[1]) {
  case 'r':
   switch(p[2]) {
   case 'e':
    switch(p[3]) {
    case 'a':
     switch(p[4]) {
     case 'k':
      if(!Risse_iswalnum_nc(p[5])) { ptr += 5; return T_BREAK; }
      goto cut_word;
     }
    }
   }
  }
 case 'p':
  switch(p[1]) {
  case 'u':
   switch(p[2]) {
   case 'b':
    switch(p[3]) {
    case 'l':
     switch(p[4]) {
     case 'i':
      switch(p[5]) {
      case 'c':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_PUBLIC; }
       goto cut_word;
      }
     }
    }
   }
  case 'r':
   switch(p[2]) {
   case 'o':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'e':
      switch(p[5]) {
      case 'c':
       switch(p[6]) {
       case 't':
        switch(p[7]) {
        case 'e':
         switch(p[8]) {
         case 'd':
          if(!Risse_iswalnum_nc(p[9])) { ptr += 9; return T_PROTECTED; }
          goto cut_word;
         }
        }
       }
      }
     }
    case 'p':
     switch(p[4]) {
     case 'e':
      switch(p[5]) {
      case 'r':
       switch(p[6]) {
       case 't':
        switch(p[7]) {
        case 'y':
         if(!Risse_iswalnum_nc(p[8])) { ptr += 8; return T_PROPERTY; }
         goto cut_word;
        }
       }
      }
     }
    }
   case 'i':
    switch(p[3]) {
    case 'v':
     switch(p[4]) {
     case 'a':
      switch(p[5]) {
      case 't':
       switch(p[6]) {
       case 'e':
        if(!Risse_iswalnum_nc(p[7])) { ptr += 7; return T_PRIVATE; }
        goto cut_word;
       }
      }
     }
    }
   }
  }
 case 'n':
  switch(p[1]) {
  case 'u':
   switch(p[2]) {
   case 'l':
    switch(p[3]) {
    case 'l':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_NULL; }
     goto cut_word;
    }
   }
  case 'e':
   switch(p[2]) {
   case 'w':
    if(!Risse_iswalnum_nc(p[3])) { ptr += 3; return T_NEW; }
    goto cut_word;
   }
  }
 case 's':
  switch(p[1]) {
  case 'w':
   switch(p[2]) {
   case 'i':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'c':
      switch(p[5]) {
      case 'h':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_SWITCH; }
       goto cut_word;
      }
     }
    }
   }
  case 'y':
   switch(p[2]) {
   case 'n':
    switch(p[3]) {
    case 'c':
     switch(p[4]) {
     case 'h':
      switch(p[5]) {
      case 'r':
       switch(p[6]) {
       case 'o':
        switch(p[7]) {
        case 'n':
         switch(p[8]) {
         case 'i':
          switch(p[9]) {
          case 'z':
           switch(p[10]) {
           case 'e':
            switch(p[11]) {
            case 'd':
             if(!Risse_iswalnum_nc(p[12])) { ptr += 12; return T_SYNCHRONIZED; }
             goto cut_word;
            }
           }
          }
         }
        }
       }
      }
     }
    }
   }
  case 'u':
   switch(p[2]) {
   case 'p':
    switch(p[3]) {
    case 'e':
     switch(p[4]) {
     case 'r':
      if(!Risse_iswalnum_nc(p[5])) { ptr += 5; return T_SUPER; }
      goto cut_word;
     }
    }
   }
  case 't':
   switch(p[2]) {
   case 'r':
    switch(p[3]) {
    case 'i':
     switch(p[4]) {
     case 'n':
      switch(p[5]) {
      case 'g':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_STRING; }
       goto cut_word;
      }
     }
    }
   case 'a':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'i':
      switch(p[5]) {
      case 'c':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_STATIC; }
       goto cut_word;
      }
     }
    }
   }
  case 'e':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'e':
      switch(p[5]) {
      case 'r':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_SETTER; }
       goto cut_word;
      }
     }
    }
   }
  }
 case 't':
  switch(p[1]) {
  case 'y':
   switch(p[2]) {
   case 'p':
    switch(p[3]) {
    case 'e':
     switch(p[4]) {
     case 'o':
      switch(p[5]) {
      case 'f':
       if(!Risse_iswalnum_nc(p[6])) { ptr += 6; return T_TYPEOF; }
       goto cut_word;
      }
     }
    }
   }
  case 'h':
   switch(p[2]) {
   case 'r':
    switch(p[3]) {
    case 'o':
     switch(p[4]) {
     case 'w':
      if(!Risse_iswalnum_nc(p[5])) { ptr += 5; return T_THROW; }
      goto cut_word;
     }
    }
   case 'i':
    switch(p[3]) {
    case 's':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_THIS; }
     goto cut_word;
    }
   }
  case 'r':
   switch(p[2]) {
   case 'y':
    if(!Risse_iswalnum_nc(p[3])) { ptr += 3; return T_TRY; }
    goto cut_word;
   case 'u':
    switch(p[3]) {
    case 'e':
     if(!Risse_iswalnum_nc(p[4])) { ptr += 4; return T_TRUE; }
     goto cut_word;
    }
   }
  }
 case '|':
  switch(p[1]) {
  case '|':
   switch(p[2]) {
   default:
    { ptr += 2; return T_LOGICALOR; }
   case '=':
    { ptr += 3; return T_LOGICALOREQUAL; }
   }
  default:
   { ptr += 1; return T_VERTLINE; }
  case '=':
   { ptr += 2; return T_VERTLINEEQUAL; }
  }
 case '*':
  switch(p[1]) {
  default:
   { ptr += 1; return T_ASTERISK; }
  case '=':
   { ptr += 2; return T_ASTERISKEQUAL; }
  }
 case '\\':
  switch(p[1]) {
  default:
   { ptr += 1; return T_BACKSLASH; }
  case '=':
   { ptr += 2; return T_BACKSLASHEQUAL; }
  }
 case '^':
  switch(p[1]) {
  default:
   { ptr += 1; return T_CHEVRON; }
  case '=':
   { ptr += 2; return T_CHEVRONEQUAL; }
  }
 case '-':
  switch(p[1]) {
  default:
   { ptr += 1; return T_MINUS; }
  case '-':
   { ptr += 2; return T_DECREMENT; }
  case '=':
   { ptr += 2; return T_MINUSEQUAL; }
  }
 case '%':
  switch(p[1]) {
  default:
   { ptr += 1; return T_PERCENT; }
  case '=':
   { ptr += 2; return T_PERCENTEQUAL; }
  }
 case '+':
  switch(p[1]) {
  default:
   { ptr += 1; return T_PLUS; }
  case '+':
   { ptr += 2; return T_INCREMENT; }
  case '=':
   { ptr += 2; return T_PLUSEQUAL; }
  }
 case '/':
  switch(p[1]) {
  default:
   { ptr += 1; return T_SLASH; }
  case '*':
   { ptr += 2; return T_BEGIN_COMMENT; }
  case '=':
   { ptr += 2; return T_SLASHEQUAL; }
  }
 case '@':
  { ptr += 1; return T_BEGIN_MACRO; }
 case '"':
 case '\'':
  { ptr += 1; return T_BEGIN_STRING_LITERAL; }
 case ':':
  { ptr += 1; return T_COLON; }
 case ',':
  { ptr += 1; return T_COMMA; }
 case '$':
  { ptr += 1; return T_DOLLAR; }
 case '{':
  { ptr += 1; return T_LBRACE; }
 case '[':
  { ptr += 1; return T_LBRACKET; }
 case '(':
  { ptr += 1; return T_LPARENTHESIS; }
 case '?':
  { ptr += 1; return T_QUESTION; }
 case '}':
  { ptr += 1; return T_RBRACE; }
 case ']':
  { ptr += 1; return T_RBRACKET; }
 case ')':
  { ptr += 1; return T_RPARENTHESIS; }
 case ';':
  { ptr += 1; return T_SEMICOLON; }
 case '~':
  { ptr += 1; return T_TILDE; }
 default:
  if(Risse_iswalpha_nc(p[0])) { p++; goto cut_word; }
 }
 return 0;
cut_word:
 while(Risse_iswalnum_nc(*p)) p++;
 ptr = p;
 return T_SYMBOL;
}
