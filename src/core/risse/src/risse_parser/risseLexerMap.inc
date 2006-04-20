static int RisseMapToken(const risse_char * & ptr, tRisseVariant &value)
{
 const risse_char * p = ptr;

 switch(p[0]) {
 case '0':
 case '1':
 case '2':
 case '3':
 case '4':
 case '5':
 case '6':
 case '7':
 case '8':
 case '9':
  p += 1;
  { ptr = p; return T_BEGIN_NUMBER; }
 case '"':
 case '\'':
  p += 1;
  { ptr = p; return T_BEGIN_STRING_LITERAL; }
 case ':':
  p += 1;
  { ptr = p; return T_COLON; }
 case ',':
  p += 1;
  { ptr = p; return T_COMMA; }
 case '$':
  p += 1;
  { ptr = p; return T_DOLLAR; }
 case '{':
  p += 1;
  { ptr = p; return T_LBRACE; }
 case '[':
  p += 1;
  { ptr = p; return T_LBRACKET; }
 case '(':
  p += 1;
  { ptr = p; return T_LPARENTHESIS; }
 case '?':
  p += 1;
  { ptr = p; return T_QUESTION; }
 case '}':
  p += 1;
  { ptr = p; return T_RBRACE; }
 case ']':
  p += 1;
  { ptr = p; return T_RBRACKET; }
 case ')':
  p += 1;
  { ptr = p; return T_RPARENTHESIS; }
 case ';':
  p += 1;
  { ptr = p; return T_SEMICOLON; }
 case '#':
  p += 1;
  { ptr = p; return T_SHARP; }
 case '~':
  p += 1;
  { ptr = p; return T_TILDE; }
 case 'N':
  switch(p[1]) {
  case 'a':
   switch(p[2]) {
   case 'N':
    p += 3;
    if(!Risse_iswordcha_nc(*p)) { ptr = p; value = RisseGetNaN(); return T_CONSTVAL; }
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'f':
  switch(p[1]) {
  case 'a':
   switch(p[2]) {
   case 'l':
    switch(p[3]) {
    case 's':
     switch(p[4]) {
     case 'e':
      p += 5;
      if(!Risse_iswordcha_nc(*p)) { ptr = p; value = false; return T_CONSTVAL; }
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'i':
   switch(p[2]) {
   case 'n':
    switch(p[3]) {
    case 'a':
     switch(p[4]) {
     case 'l':
      switch(p[5]) {
      default:
       p += 5;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_FINAL; }
       goto cut_word;
      case 'l':
       switch(p[6]) {
       case 'y':
        p += 7;
        if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_FINALLY; }
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
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
         p += 8;
         if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_FUNCTION; }
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'o':
   switch(p[2]) {
   case 'r':
    p += 3;
    if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_FOR; }
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'o':
  switch(p[1]) {
  case 'c':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 'e':
     switch(p[4]) {
     case 't':
      p += 5;
      if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_OCTET; }
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'r':
  switch(p[1]) {
  case 'e':
   switch(p[2]) {
   case 'a':
    switch(p[3]) {
    case 'l':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_REAL; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   case 't':
    switch(p[3]) {
    case 'u':
     switch(p[4]) {
     case 'r':
      switch(p[5]) {
      case 'n':
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_RETURN; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'i':
  switch(p[1]) {
  case 'f':
   p += 2;
   if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_IF; }
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
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_IMPORT; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
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
        p += 7;
        if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_ISVALID; }
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'n':
   switch(p[2]) {
   default:
    p += 2;
    if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_IN; }
    goto cut_word;
   case 't':
    p += 3;
    if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_INT; }
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
           p += 10;
           if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_INVALIDATE; }
           goto cut_word;
          }
          p += 9;
          goto cut_word;
         }
         p += 8;
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
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
            p += 11;
            if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_INCONTEXTOF; }
            goto cut_word;
           }
           p += 10;
           goto cut_word;
          }
          p += 9;
          goto cut_word;
         }
         p += 8;
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
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
           p += 10;
           if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_INSTANCEOF; }
           goto cut_word;
          }
          p += 9;
          goto cut_word;
         }
         p += 8;
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'w':
  switch(p[1]) {
  case 'h':
   switch(p[2]) {
   case 'i':
    switch(p[3]) {
    case 'l':
     switch(p[4]) {
     case 'e':
      p += 5;
      if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_WHILE; }
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'i':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 'h':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_WITH; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'c':
  switch(p[1]) {
  case 'l':
   switch(p[2]) {
   case 'a':
    switch(p[3]) {
    case 's':
     switch(p[4]) {
     case 's':
      p += 5;
      if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_CLASS; }
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
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
         p += 8;
         if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_CONTINUE; }
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    case 's':
     switch(p[4]) {
     case 't':
      p += 5;
      if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_CONST; }
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'a':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 'c':
     switch(p[4]) {
     case 'h':
      p += 5;
      if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_CATCH; }
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   case 's':
    switch(p[3]) {
    case 'e':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_CASE; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
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
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_GLOBAL; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'o':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 'o':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_GOTO; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'e':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'e':
      switch(p[5]) {
      case 'r':
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_GETTER; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'e':
  switch(p[1]) {
  case 'l':
   switch(p[2]) {
   case 's':
    switch(p[3]) {
    case 'e':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_ELSE; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
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
        p += 7;
        if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_EXTENDS; }
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   case 'p':
    switch(p[3]) {
    case 'o':
     switch(p[4]) {
     case 'r':
      switch(p[5]) {
      case 't':
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_EXPORT; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'n':
   switch(p[2]) {
   case 'u':
    switch(p[3]) {
    case 'm':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_ENUM; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
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
         p += 8;
         if(!Risse_iswordcha_nc(*p)) { ptr = p; value = RisseGetInf(); return T_CONSTVAL; }
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'd':
  switch(p[1]) {
  case 'o':
   p += 2;
   if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_DO; }
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
        p += 7;
        if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_DEFAULT; }
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   case 'l':
    switch(p[3]) {
    case 'e':
     switch(p[4]) {
     case 't':
      switch(p[5]) {
      case 'e':
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_DELETE; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
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
         p += 8;
         if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_DEBUGGER; }
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'v':
  switch(p[1]) {
  case 'o':
   switch(p[2]) {
   case 'i':
    switch(p[3]) {
    case 'd':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; value = tRisseVariant(); return T_CONSTVAL; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'a':
   switch(p[2]) {
   case 'r':
    p += 3;
    if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_VAR; }
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'b':
  switch(p[1]) {
  case 'r':
   switch(p[2]) {
   case 'e':
    switch(p[3]) {
    case 'a':
     switch(p[4]) {
     case 'k':
      p += 5;
      if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_BREAK; }
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
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
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_PUBLIC; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
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
          p += 9;
          if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_PROTECTED; }
          goto cut_word;
         }
         p += 8;
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    case 'p':
     switch(p[4]) {
     case 'e':
      switch(p[5]) {
      case 'r':
       switch(p[6]) {
       case 't':
        switch(p[7]) {
        case 'y':
         p += 8;
         if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_PROPERTY; }
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   case 'i':
    switch(p[3]) {
    case 'v':
     switch(p[4]) {
     case 'a':
      switch(p[5]) {
      case 't':
       switch(p[6]) {
       case 'e':
        p += 7;
        if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_PRIVATE; }
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case 'n':
  switch(p[1]) {
  case 'u':
   switch(p[2]) {
   case 'l':
    switch(p[3]) {
    case 'l':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; value = tRisseObject(); return T_CONSTVAL; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'e':
   switch(p[2]) {
   case 'w':
    p += 3;
    if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_NEW; }
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
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
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_SWITCH; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
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
             p += 12;
             if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_SYNCHRONIZED; }
             goto cut_word;
            }
            p += 11;
            goto cut_word;
           }
           p += 10;
           goto cut_word;
          }
          p += 9;
          goto cut_word;
         }
         p += 8;
         goto cut_word;
        }
        p += 7;
        goto cut_word;
       }
       p += 6;
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'u':
   switch(p[2]) {
   case 'p':
    switch(p[3]) {
    case 'e':
     switch(p[4]) {
     case 'r':
      p += 5;
      if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_SUPER; }
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 't':
   switch(p[2]) {
   case 'r':
    switch(p[3]) {
    case 'i':
     switch(p[4]) {
     case 'n':
      switch(p[5]) {
      case 'g':
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_STRING; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   case 'a':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'i':
      switch(p[5]) {
      case 'c':
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_STATIC; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'e':
   switch(p[2]) {
   case 't':
    switch(p[3]) {
    case 't':
     switch(p[4]) {
     case 'e':
      switch(p[5]) {
      case 'r':
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_SETTER; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
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
       p += 6;
       if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_TYPEOF; }
       goto cut_word;
      }
      p += 5;
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'h':
   switch(p[2]) {
   case 'r':
    switch(p[3]) {
    case 'o':
     switch(p[4]) {
     case 'w':
      p += 5;
      if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_THROW; }
      goto cut_word;
     }
     p += 4;
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   case 'i':
    switch(p[3]) {
    case 's':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_THIS; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  case 'r':
   switch(p[2]) {
   case 'y':
    p += 3;
    if(!Risse_iswordcha_nc(*p)) { ptr = p; return T_TRY; }
    goto cut_word;
   case 'u':
    switch(p[3]) {
    case 'e':
     p += 4;
     if(!Risse_iswordcha_nc(*p)) { ptr = p; value = true; return T_CONSTVAL; }
     goto cut_word;
    }
    p += 3;
    goto cut_word;
   }
   p += 2;
   goto cut_word;
  }
  p += 1;
  goto cut_word;
 case '&':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_AMPERSAND; }
  case '=':
   p += 2;
   { ptr = p; return T_AMPERSANDEQUAL; }
  case '&':
   switch(p[2]) {
   default:
    p += 2;
    { ptr = p; return T_LOGICALAND; }
   case '=':
    p += 3;
    { ptr = p; return T_LOGICALANDEQUAL; }
   }
  }
 case '*':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_ASTERISK; }
  case '=':
   p += 2;
   { ptr = p; return T_ASTERISKEQUAL; }
  }
 case '\\':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_BACKSLASH; }
  case '=':
   p += 2;
   { ptr = p; return T_BACKSLASHEQUAL; }
  }
 case '@':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_BEGIN_MACRO; }
  case '"':
  case '\'':
   p += 2;
   { ptr = p; return T_BEGIN_EMSTRING_LITERAL; }
  }
 case '^':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_CHEVRON; }
  case '=':
   p += 2;
   { ptr = p; return T_CHEVRONEQUAL; }
  }
 case '.':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_DOT; }
  case '.':
   switch(p[2]) {
   case '.':
    p += 3;
    { ptr = p; return T_OMIT; }
   }
  }
 case '=':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_EQUAL; }
  case '>':
   p += 2;
   { ptr = p; return T_COMMA; }
  case '=':
   switch(p[2]) {
   default:
    p += 2;
    { ptr = p; return T_EQUALEQUAL; }
   case '=':
    p += 3;
    { ptr = p; return T_DISCEQUAL; }
   }
  }
 case '!':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_EXCRAMATION; }
  case '=':
   switch(p[2]) {
   default:
    p += 2;
    { ptr = p; return T_NOTEQUAL; }
   case '=':
    p += 3;
    { ptr = p; return T_DISCNOTEQUAL; }
   }
  }
 case '>':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_GT; }
  case '=':
   p += 2;
   { ptr = p; return T_GTOREQUAL; }
  case '>':
   switch(p[2]) {
   default:
    p += 2;
    { ptr = p; return T_RARITHSHIFT; }
   case '=':
    p += 3;
    { ptr = p; return T_RARITHSHIFTEQUAL; }
   case '>':
    switch(p[3]) {
    default:
     p += 3;
     { ptr = p; return T_RBITSHIFT; }
    case '=':
     p += 4;
     { ptr = p; return T_RBITSHIFTEQUAL; }
    }
   }
  }
 case '<':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_LT; }
  case '%':
   p += 2;
   { ptr = p; return T_BEGIN_OCTET_LITERAL; }
  case '=':
   p += 2;
   { ptr = p; return T_LTOREQUAL; }
  case '-':
   switch(p[2]) {
   case '>':
    p += 3;
    { ptr = p; return T_SWAP; }
   }
  case '<':
   switch(p[2]) {
   default:
    p += 2;
    { ptr = p; return T_LARITHSHIFT; }
   case '=':
    p += 3;
    { ptr = p; return T_LARITHSHIFTEQUAL; }
   }
  }
 case '-':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_MINUS; }
  case '-':
   p += 2;
   { ptr = p; return T_DECREMENT; }
  case '=':
   p += 2;
   { ptr = p; return T_MINUSEQUAL; }
  }
 case '%':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_PERCENT; }
  case '=':
   p += 2;
   { ptr = p; return T_PERCENTEQUAL; }
  }
 case '+':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_PLUS; }
  case '+':
   p += 2;
   { ptr = p; return T_INCREMENT; }
  case '=':
   p += 2;
   { ptr = p; return T_PLUSEQUAL; }
  }
 case '/':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_SLASH; }
  case '*':
  case '/':
   p += 2;
   { ptr = p; return T_BEGIN_COMMENT; }
  case '=':
   p += 2;
   { ptr = p; return T_SLASHEQUAL; }
  }
 case '|':
  switch(p[1]) {
  default:
   p += 1;
   { ptr = p; return T_VERTLINE; }
  case '=':
   p += 2;
   { ptr = p; return T_VERTLINEEQUAL; }
  case '|':
   switch(p[2]) {
   default:
    p += 2;
    { ptr = p; return T_LOGICALOR; }
   case '=':
    p += 3;
    { ptr = p; return T_LOGICALOREQUAL; }
   }
  }
 default:
  if(Risse_iswalpha_nc(p[0])) { p++; goto cut_word; }
 }
 return 0;
cut_word:
 while(Risse_iswordcha_nc(*p)) p++;
 ptr = p;
 return T_SYMBOL;
}
