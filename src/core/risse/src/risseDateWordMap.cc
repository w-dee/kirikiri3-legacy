/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*!  日付文字列解析用 Lexer の実装 */
/*
	Date/time string parser lexical analyzer word cutter.

	This file is always generated from syntax/dp_wordtable.txt by
	syntax/create_word_map.pl. Modification by hand will be lost.

*/

switch(InputPointer[0])
{
case RISSE_WC('a'):
case RISSE_WC('A'):
 switch(InputPointer[1])
 {
 case RISSE_WC('c'):
 case RISSE_WC('C'):
  switch(InputPointer[2])
  {
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('s'):
   case RISSE_WC('S'):
    switch(InputPointer[4])
    {
    case RISSE_WC('t'):
    case RISSE_WC('T'):
      if(!Risse_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 1030; return DP_TZ; }
     break;
    }
    break;
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 930; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -300; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('e'):
 case RISSE_WC('E'):
  switch(InputPointer[2])
  {
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('s'):
   case RISSE_WC('S'):
    switch(InputPointer[4])
    {
    case RISSE_WC('t'):
    case RISSE_WC('T'):
      if(!Risse_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 1100; return DP_TZ; }
     break;
    }
    break;
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1000; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case RISSE_WC('h'):
 case RISSE_WC('H'):
  switch(InputPointer[2])
  {
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = -1000; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case RISSE_WC('m'):
 case RISSE_WC('M'):
   if(!Risse_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 0; return DP_AM; }
  break;
 case RISSE_WC('p'):
 case RISSE_WC('P'):
  switch(InputPointer[2])
  {
  case RISSE_WC('r'):
  case RISSE_WC('R'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 3; return DP_MONTH; }
    break;
   case RISSE_WC('i'):
   case RISSE_WC('I'):
    switch(InputPointer[4])
    {
    case RISSE_WC('l'):
    case RISSE_WC('L'):
      if(!Risse_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 3; return DP_MONTH; }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 3; return DP_MONTH; }
   }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -400; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('u'):
 case RISSE_WC('U'):
  switch(InputPointer[2])
  {
  case RISSE_WC('g'):
  case RISSE_WC('G'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 7; return DP_MONTH; }
    break;
   case RISSE_WC('u'):
   case RISSE_WC('U'):
    switch(InputPointer[4])
    {
    case RISSE_WC('s'):
    case RISSE_WC('S'):
     switch(InputPointer[5])
     {
     case RISSE_WC('t'):
     case RISSE_WC('T'):
       if(!Risse_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 7; return DP_MONTH; }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 7; return DP_MONTH; }
   }
   break;
  }
  break;
 case RISSE_WC('w'):
 case RISSE_WC('W'):
  switch(InputPointer[2])
  {
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('s'):
   case RISSE_WC('S'):
    switch(InputPointer[4])
    {
    case RISSE_WC('t'):
    case RISSE_WC('T'):
      if(!Risse_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 900; return DP_TZ; }
     break;
    }
    break;
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 800; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 default:
  if(!Risse_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = -100; return DP_TZ; }
 }
 break;
case RISSE_WC('b'):
case RISSE_WC('B'):
 switch(InputPointer[1])
 {
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('t'):
 case RISSE_WC('T'):
   if(!Risse_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 300; return DP_TZ; }
  break;
 }
 break;
case RISSE_WC('c'):
case RISSE_WC('C'):
 switch(InputPointer[1])
 {
 case RISSE_WC('a'):
 case RISSE_WC('A'):
  switch(InputPointer[2])
  {
  case RISSE_WC('d'):
  case RISSE_WC('D'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1030; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 930; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -1000; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('c'):
 case RISSE_WC('C'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 800; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -500; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('e'):
 case RISSE_WC('E'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
   switch(InputPointer[3])
   {
   case RISSE_WC('d'):
   case RISSE_WC('D'):
    switch(InputPointer[4])
    {
    case RISSE_WC('s'):
    case RISSE_WC('S'):
     switch(InputPointer[5])
     {
     case RISSE_WC('t'):
     case RISSE_WC('T'):
       if(!Risse_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 200; return DP_TZ; }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -600; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('d'):
case RISSE_WC('D'):
 switch(InputPointer[1])
 {
 case RISSE_WC('e'):
 case RISSE_WC('E'):
  switch(InputPointer[2])
  {
  case RISSE_WC('c'):
  case RISSE_WC('C'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 11; return DP_MONTH; }
    break;
   case RISSE_WC('e'):
   case RISSE_WC('E'):
    switch(InputPointer[4])
    {
    case RISSE_WC('m'):
    case RISSE_WC('M'):
     switch(InputPointer[5])
     {
     case RISSE_WC('b'):
     case RISSE_WC('B'):
      switch(InputPointer[6])
      {
      case RISSE_WC('e'):
      case RISSE_WC('E'):
       switch(InputPointer[7])
       {
       case RISSE_WC('r'):
       case RISSE_WC('R'):
         if(!Risse_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 11; return DP_MONTH; }
        break;
       }
       break;
      }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 11; return DP_MONTH; }
   }
   break;
  }
  break;
 case RISSE_WC('n'):
 case RISSE_WC('N'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('e'):
case RISSE_WC('E'):
 switch(InputPointer[1])
 {
 case RISSE_WC('a'):
 case RISSE_WC('A'):
  switch(InputPointer[2])
  {
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1000; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -400; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('e'):
 case RISSE_WC('E'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
   switch(InputPointer[3])
   {
   case RISSE_WC('d'):
   case RISSE_WC('D'):
    switch(InputPointer[4])
    {
    case RISSE_WC('s'):
    case RISSE_WC('S'):
     switch(InputPointer[5])
     {
     case RISSE_WC('t'):
     case RISSE_WC('T'):
       if(!Risse_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 300; return DP_TZ; }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 200; return DP_TZ; }
   }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -500; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('f'):
case RISSE_WC('F'):
 switch(InputPointer[1])
 {
 case RISSE_WC('e'):
 case RISSE_WC('E'):
  switch(InputPointer[2])
  {
  case RISSE_WC('b'):
  case RISSE_WC('B'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1; return DP_MONTH; }
    break;
   case RISSE_WC('r'):
   case RISSE_WC('R'):
    switch(InputPointer[4])
    {
    case RISSE_WC('u'):
    case RISSE_WC('U'):
     switch(InputPointer[5])
     {
     case RISSE_WC('a'):
     case RISSE_WC('A'):
      switch(InputPointer[6])
      {
      case RISSE_WC('r'):
      case RISSE_WC('R'):
       switch(InputPointer[7])
       {
       case RISSE_WC('y'):
       case RISSE_WC('Y'):
         if(!Risse_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 1; return DP_MONTH; }
        break;
       }
       break;
      }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 1; return DP_MONTH; }
   }
   break;
  }
  break;
 case RISSE_WC('r'):
 case RISSE_WC('R'):
  switch(InputPointer[2])
  {
  case RISSE_WC('i'):
  case RISSE_WC('I'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 5; return DP_WDAY; }
    break;
   case RISSE_WC('d'):
   case RISSE_WC('D'):
    switch(InputPointer[4])
    {
    case RISSE_WC('a'):
    case RISSE_WC('A'):
     switch(InputPointer[5])
     {
     case RISSE_WC('y'):
     case RISSE_WC('Y'):
       if(!Risse_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 5; return DP_WDAY; }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 5; return DP_WDAY; }
   }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('w'):
 case RISSE_WC('W'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 200; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('g'):
case RISSE_WC('G'):
 switch(InputPointer[1])
 {
 case RISSE_WC('m'):
 case RISSE_WC('M'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 1000; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('h'):
case RISSE_WC('H'):
 switch(InputPointer[1])
 {
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -900; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('i'):
case RISSE_WC('I'):
 switch(InputPointer[1])
 {
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('l'):
  case RISSE_WC('L'):
   switch(InputPointer[3])
   {
   case RISSE_WC('e'):
   case RISSE_WC('E'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1200; return DP_TZ; }
    break;
   case RISSE_WC('w'):
   case RISSE_WC('W'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = -1200; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 200; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('t'):
 case RISSE_WC('T'):
   if(!Risse_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 330; return DP_TZ; }
  break;
 }
 break;
case RISSE_WC('j'):
case RISSE_WC('J'):
 switch(InputPointer[1])
 {
 case RISSE_WC('a'):
 case RISSE_WC('A'):
  switch(InputPointer[2])
  {
  case RISSE_WC('n'):
  case RISSE_WC('N'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 0; return DP_MONTH; }
    break;
   case RISSE_WC('u'):
   case RISSE_WC('U'):
    switch(InputPointer[4])
    {
    case RISSE_WC('a'):
    case RISSE_WC('A'):
     switch(InputPointer[5])
     {
     case RISSE_WC('r'):
     case RISSE_WC('R'):
      switch(InputPointer[6])
      {
      case RISSE_WC('y'):
      case RISSE_WC('Y'):
        if(!Risse_iswalpha(InputPointer[7])) { InputPointer += 7; yylex->val = 0; return DP_MONTH; }
       break;
      }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_MONTH; }
   }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 900; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('t'):
 case RISSE_WC('T'):
   if(!Risse_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 730; return DP_TZ; }
  break;
 case RISSE_WC('u'):
 case RISSE_WC('U'):
  switch(InputPointer[2])
  {
  case RISSE_WC('.'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 5; return DP_MONTH; }
   break;
  case RISSE_WC('l'):
  case RISSE_WC('L'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 6; return DP_MONTH; }
    break;
   case RISSE_WC('y'):
   case RISSE_WC('Y'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 6; return DP_MONTH; }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 6; return DP_MONTH; }
   }
   break;
  case RISSE_WC('n'):
  case RISSE_WC('N'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 5; return DP_MONTH; }
    break;
   case RISSE_WC('e'):
   case RISSE_WC('E'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 5; return DP_MONTH; }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 5; return DP_MONTH; }
   }
   break;
  default:
   if(!Risse_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 5; return DP_MONTH; }
  }
  break;
 }
 break;
case RISSE_WC('k'):
case RISSE_WC('K'):
 switch(InputPointer[1])
 {
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 900; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('l'):
case RISSE_WC('L'):
 switch(InputPointer[1])
 {
 case RISSE_WC('i'):
 case RISSE_WC('I'):
  switch(InputPointer[2])
  {
  case RISSE_WC('g'):
  case RISSE_WC('G'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1000; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 }
 break;
case RISSE_WC('m'):
case RISSE_WC('M'):
 switch(InputPointer[1])
 {
 case RISSE_WC('a'):
 case RISSE_WC('A'):
  switch(InputPointer[2])
  {
  case RISSE_WC('r'):
  case RISSE_WC('R'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 2; return DP_MONTH; }
    break;
   case RISSE_WC('c'):
   case RISSE_WC('C'):
    switch(InputPointer[4])
    {
    case RISSE_WC('h'):
    case RISSE_WC('H'):
      if(!Risse_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 2; return DP_MONTH; }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 2; return DP_MONTH; }
   }
   break;
  case RISSE_WC('y'):
  case RISSE_WC('Y'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 4; return DP_MONTH; }
   break;
  }
  break;
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -600; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('e'):
 case RISSE_WC('E'):
  switch(InputPointer[2])
  {
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 200; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('t'):
  case RISSE_WC('T'):
   switch(InputPointer[3])
   {
   case RISSE_WC('d'):
   case RISSE_WC('D'):
    switch(InputPointer[4])
    {
    case RISSE_WC('s'):
    case RISSE_WC('S'):
     switch(InputPointer[5])
     {
     case RISSE_WC('t'):
     case RISSE_WC('T'):
       if(!Risse_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 200; return DP_TZ; }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   }
   break;
  case RISSE_WC('w'):
  case RISSE_WC('W'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 100; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('z'):
  case RISSE_WC('Z'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('o'):
 case RISSE_WC('O'):
  switch(InputPointer[2])
  {
  case RISSE_WC('n'):
  case RISSE_WC('N'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1; return DP_WDAY; }
    break;
   case RISSE_WC('d'):
   case RISSE_WC('D'):
    switch(InputPointer[4])
    {
    case RISSE_WC('a'):
    case RISSE_WC('A'):
     switch(InputPointer[5])
     {
     case RISSE_WC('y'):
     case RISSE_WC('Y'):
       if(!Risse_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 1; return DP_WDAY; }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 1; return DP_WDAY; }
   }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -700; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('t'):
 case RISSE_WC('T'):
   if(!Risse_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 830; return DP_TZ; }
  break;
 default:
  if(!Risse_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = -1200; return DP_TZ; }
 }
 break;
case RISSE_WC('n'):
case RISSE_WC('N'):
 switch(InputPointer[1])
 {
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -230; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('f'):
 case RISSE_WC('F'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -330; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('o'):
 case RISSE_WC('O'):
  switch(InputPointer[2])
  {
  case RISSE_WC('r'):
  case RISSE_WC('R'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  case RISSE_WC('v'):
  case RISSE_WC('V'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 10; return DP_MONTH; }
    break;
   case RISSE_WC('e'):
   case RISSE_WC('E'):
    switch(InputPointer[4])
    {
    case RISSE_WC('m'):
    case RISSE_WC('M'):
     switch(InputPointer[5])
     {
     case RISSE_WC('b'):
     case RISSE_WC('B'):
      switch(InputPointer[6])
      {
      case RISSE_WC('e'):
      case RISSE_WC('E'):
       switch(InputPointer[7])
       {
       case RISSE_WC('r'):
       case RISSE_WC('R'):
         if(!Risse_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 10; return DP_MONTH; }
        break;
       }
       break;
      }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 10; return DP_MONTH; }
   }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -330; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('t'):
 case RISSE_WC('T'):
   if(!Risse_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = -1100; return DP_TZ; }
  break;
 case RISSE_WC('z'):
 case RISSE_WC('Z'):
  switch(InputPointer[2])
  {
  case RISSE_WC('d'):
  case RISSE_WC('D'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1300; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1200; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 1200; return DP_TZ; }
   break;
  }
  break;
 default:
  if(!Risse_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = 100; return DP_TZ; }
 }
 break;
case RISSE_WC('o'):
case RISSE_WC('O'):
 switch(InputPointer[1])
 {
 case RISSE_WC('c'):
 case RISSE_WC('C'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 9; return DP_MONTH; }
    break;
   case RISSE_WC('o'):
   case RISSE_WC('O'):
    switch(InputPointer[4])
    {
    case RISSE_WC('b'):
    case RISSE_WC('B'):
     switch(InputPointer[5])
     {
     case RISSE_WC('e'):
     case RISSE_WC('E'):
      switch(InputPointer[6])
      {
      case RISSE_WC('r'):
      case RISSE_WC('R'):
        if(!Risse_iswalpha(InputPointer[7])) { InputPointer += 7; yylex->val = 9; return DP_MONTH; }
       break;
      }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 9; return DP_MONTH; }
   }
   break;
  }
  break;
 }
 break;
case RISSE_WC('p'):
case RISSE_WC('P'):
 switch(InputPointer[1])
 {
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -700; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('m'):
 case RISSE_WC('M'):
   if(!Risse_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 0; return DP_PM; }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -800; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('s'):
case RISSE_WC('S'):
 switch(InputPointer[1])
 {
 case RISSE_WC('a'):
 case RISSE_WC('A'):
  switch(InputPointer[2])
  {
  case RISSE_WC('d'):
  case RISSE_WC('D'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1030; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 930; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('t'):
  case RISSE_WC('T'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 6; return DP_WDAY; }
    break;
   case RISSE_WC('u'):
   case RISSE_WC('U'):
    switch(InputPointer[4])
    {
    case RISSE_WC('r'):
    case RISSE_WC('R'):
     switch(InputPointer[5])
     {
     case RISSE_WC('d'):
     case RISSE_WC('D'):
      switch(InputPointer[6])
      {
      case RISSE_WC('a'):
      case RISSE_WC('A'):
       switch(InputPointer[7])
       {
       case RISSE_WC('y'):
       case RISSE_WC('Y'):
         if(!Risse_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 6; return DP_WDAY; }
        break;
       }
       break;
      }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 6; return DP_WDAY; }
   }
   break;
  }
  break;
 case RISSE_WC('e'):
 case RISSE_WC('E'):
  switch(InputPointer[2])
  {
  case RISSE_WC('p'):
  case RISSE_WC('P'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 8; return DP_MONTH; }
    break;
   case RISSE_WC('t'):
   case RISSE_WC('T'):
    switch(InputPointer[4])
    {
    case RISSE_WC('.'):
      if(!Risse_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 8; return DP_MONTH; }
     break;
    case RISSE_WC('e'):
    case RISSE_WC('E'):
     switch(InputPointer[5])
     {
     case RISSE_WC('m'):
     case RISSE_WC('M'):
      switch(InputPointer[6])
      {
      case RISSE_WC('b'):
      case RISSE_WC('B'):
       switch(InputPointer[7])
       {
       case RISSE_WC('e'):
       case RISSE_WC('E'):
        switch(InputPointer[8])
        {
        case RISSE_WC('r'):
        case RISSE_WC('R'):
          if(!Risse_iswalpha(InputPointer[9])) { InputPointer += 9; yylex->val = 8; return DP_MONTH; }
         break;
        }
        break;
       }
       break;
      }
      break;
     }
     break;
    default:
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 8; return DP_MONTH; }
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 8; return DP_MONTH; }
   }
   break;
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 200; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('u'):
 case RISSE_WC('U'):
  switch(InputPointer[2])
  {
  case RISSE_WC('n'):
  case RISSE_WC('N'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 0; return DP_WDAY; }
    break;
   case RISSE_WC('d'):
   case RISSE_WC('D'):
    switch(InputPointer[4])
    {
    case RISSE_WC('a'):
    case RISSE_WC('A'):
     switch(InputPointer[5])
     {
     case RISSE_WC('y'):
     case RISSE_WC('Y'):
       if(!Risse_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 0; return DP_WDAY; }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_WDAY; }
   }
   break;
  }
  break;
 case RISSE_WC('w'):
 case RISSE_WC('W'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('t'):
case RISSE_WC('T'):
 switch(InputPointer[1])
 {
 case RISSE_WC('h'):
 case RISSE_WC('H'):
  switch(InputPointer[2])
  {
  case RISSE_WC('u'):
  case RISSE_WC('U'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 4; return DP_WDAY; }
    break;
   case RISSE_WC('r'):
   case RISSE_WC('R'):
    switch(InputPointer[4])
    {
    case RISSE_WC('s'):
    case RISSE_WC('S'):
     switch(InputPointer[5])
     {
     case RISSE_WC('.'):
       if(!Risse_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 4; return DP_WDAY; }
      break;
     case RISSE_WC('d'):
     case RISSE_WC('D'):
      switch(InputPointer[6])
      {
      case RISSE_WC('a'):
      case RISSE_WC('A'):
       switch(InputPointer[7])
       {
       case RISSE_WC('y'):
       case RISSE_WC('Y'):
         if(!Risse_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 4; return DP_WDAY; }
        break;
       }
       break;
      }
      break;
     default:
      if(!Risse_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 4; return DP_WDAY; }
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 4; return DP_WDAY; }
   }
   break;
  }
  break;
 case RISSE_WC('u'):
 case RISSE_WC('U'):
  switch(InputPointer[2])
  {
  case RISSE_WC('e'):
  case RISSE_WC('E'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 2; return DP_WDAY; }
    break;
   case RISSE_WC('s'):
   case RISSE_WC('S'):
    switch(InputPointer[4])
    {
    case RISSE_WC('.'):
      if(!Risse_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 2; return DP_WDAY; }
     break;
    case RISSE_WC('d'):
    case RISSE_WC('D'):
     switch(InputPointer[5])
     {
     case RISSE_WC('a'):
     case RISSE_WC('A'):
      switch(InputPointer[6])
      {
      case RISSE_WC('y'):
      case RISSE_WC('Y'):
        if(!Risse_iswalpha(InputPointer[7])) { InputPointer += 7; yylex->val = 2; return DP_WDAY; }
       break;
      }
      break;
     }
     break;
    default:
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 2; return DP_WDAY; }
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 2; return DP_WDAY; }
   }
   break;
  }
  break;
 }
 break;
case RISSE_WC('u'):
case RISSE_WC('U'):
 switch(InputPointer[1])
 {
 case RISSE_WC('t'):
 case RISSE_WC('T'):
  switch(InputPointer[2])
  {
  case RISSE_WC('c'):
  case RISSE_WC('C'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_TZ; }
   break;
  default:
   if(!Risse_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 0; return DP_TZ; }
  }
  break;
 }
 break;
case RISSE_WC('w'):
case RISSE_WC('W'):
 switch(InputPointer[1])
 {
 case RISSE_WC('a'):
 case RISSE_WC('A'):
  switch(InputPointer[2])
  {
  case RISSE_WC('d'):
  case RISSE_WC('D'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 800; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('s'):
  case RISSE_WC('S'):
   switch(InputPointer[3])
   {
   case RISSE_WC('t'):
   case RISSE_WC('T'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 700; return DP_TZ; }
    break;
   }
   break;
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -100; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 900; return DP_TZ; }
   break;
  }
  break;
 case RISSE_WC('e'):
 case RISSE_WC('E'):
  switch(InputPointer[2])
  {
  case RISSE_WC('d'):
  case RISSE_WC('D'):
   switch(InputPointer[3])
   {
   case RISSE_WC('.'):
     if(!Risse_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 3; return DP_WDAY; }
    break;
   case RISSE_WC('n'):
   case RISSE_WC('N'):
    switch(InputPointer[4])
    {
    case RISSE_WC('e'):
    case RISSE_WC('E'):
     switch(InputPointer[5])
     {
     case RISSE_WC('s'):
     case RISSE_WC('S'):
      switch(InputPointer[6])
      {
      case RISSE_WC('d'):
      case RISSE_WC('D'):
       switch(InputPointer[7])
       {
       case RISSE_WC('a'):
       case RISSE_WC('A'):
        switch(InputPointer[8])
        {
        case RISSE_WC('y'):
        case RISSE_WC('Y'):
          if(!Risse_iswalpha(InputPointer[9])) { InputPointer += 9; yylex->val = 3; return DP_WDAY; }
         break;
        }
        break;
       }
       break;
      }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 3; return DP_WDAY; }
   }
   break;
  case RISSE_WC('t'):
  case RISSE_WC('T'):
   switch(InputPointer[3])
   {
   case RISSE_WC('d'):
   case RISSE_WC('D'):
    switch(InputPointer[4])
    {
    case RISSE_WC('s'):
    case RISSE_WC('S'):
     switch(InputPointer[5])
     {
     case RISSE_WC('t'):
     case RISSE_WC('T'):
       if(!Risse_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 100; return DP_TZ; }
      break;
     }
     break;
    }
    break;
   default:
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_TZ; }
   }
   break;
  }
  break;
 case RISSE_WC('s'):
 case RISSE_WC('S'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 800; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case RISSE_WC('y'):
case RISSE_WC('Y'):
 switch(InputPointer[1])
 {
 case RISSE_WC('d'):
 case RISSE_WC('D'):
  switch(InputPointer[2])
  {
  case RISSE_WC('t'):
  case RISSE_WC('T'):
    if(!Risse_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -800; return DP_TZ; }
   break;
  }
  break;
 default:
  if(!Risse_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = 1200; return DP_TZ; }
 }
 break;
case RISSE_WC('z'):
case RISSE_WC('Z'):
  if(!Risse_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = 0; return DP_TZ; }
 break;
}
