/*---------------------------------------------------------------------------*/
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*
	Date/time string parser lexical analyzer word cutter.

	This file is always generated from syntax/dp_wordtable.txt by
	syntax/create_word_map.pl. Modification by hand will be lost.

*/

switch(InputPointer[0])
{
case TJS_WC('a'):
case TJS_WC('A'):
 switch(InputPointer[1])
 {
 case TJS_WC('c'):
 case TJS_WC('C'):
  switch(InputPointer[2])
  {
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('s'):
   case TJS_WC('S'):
    switch(InputPointer[4])
    {
    case TJS_WC('t'):
    case TJS_WC('T'):
      if(!TJS_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 1030; return DP_TZ; }
     break;
    }
    break;
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 930; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -300; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('e'):
 case TJS_WC('E'):
  switch(InputPointer[2])
  {
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('s'):
   case TJS_WC('S'):
    switch(InputPointer[4])
    {
    case TJS_WC('t'):
    case TJS_WC('T'):
      if(!TJS_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 1100; return DP_TZ; }
     break;
    }
    break;
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1000; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case TJS_WC('h'):
 case TJS_WC('H'):
  switch(InputPointer[2])
  {
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = -1000; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case TJS_WC('m'):
 case TJS_WC('M'):
   if(!TJS_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 0; return DP_AM; }
  break;
 case TJS_WC('p'):
 case TJS_WC('P'):
  switch(InputPointer[2])
  {
  case TJS_WC('r'):
  case TJS_WC('R'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 3; return DP_MONTH; }
    break;
   case TJS_WC('i'):
   case TJS_WC('I'):
    switch(InputPointer[4])
    {
    case TJS_WC('l'):
    case TJS_WC('L'):
      if(!TJS_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 3; return DP_MONTH; }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 3; return DP_MONTH; }
   }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -400; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('u'):
 case TJS_WC('U'):
  switch(InputPointer[2])
  {
  case TJS_WC('g'):
  case TJS_WC('G'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 7; return DP_MONTH; }
    break;
   case TJS_WC('u'):
   case TJS_WC('U'):
    switch(InputPointer[4])
    {
    case TJS_WC('s'):
    case TJS_WC('S'):
     switch(InputPointer[5])
     {
     case TJS_WC('t'):
     case TJS_WC('T'):
       if(!TJS_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 7; return DP_MONTH; }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 7; return DP_MONTH; }
   }
   break;
  }
  break;
 case TJS_WC('w'):
 case TJS_WC('W'):
  switch(InputPointer[2])
  {
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('s'):
   case TJS_WC('S'):
    switch(InputPointer[4])
    {
    case TJS_WC('t'):
    case TJS_WC('T'):
      if(!TJS_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 900; return DP_TZ; }
     break;
    }
    break;
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 800; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 default:
  if(!TJS_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = -100; return DP_TZ; }
 }
 break;
case TJS_WC('b'):
case TJS_WC('B'):
 switch(InputPointer[1])
 {
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('t'):
 case TJS_WC('T'):
   if(!TJS_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 300; return DP_TZ; }
  break;
 }
 break;
case TJS_WC('c'):
case TJS_WC('C'):
 switch(InputPointer[1])
 {
 case TJS_WC('a'):
 case TJS_WC('A'):
  switch(InputPointer[2])
  {
  case TJS_WC('d'):
  case TJS_WC('D'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1030; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 930; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -1000; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('c'):
 case TJS_WC('C'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 800; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -500; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('e'):
 case TJS_WC('E'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
   switch(InputPointer[3])
   {
   case TJS_WC('d'):
   case TJS_WC('D'):
    switch(InputPointer[4])
    {
    case TJS_WC('s'):
    case TJS_WC('S'):
     switch(InputPointer[5])
     {
     case TJS_WC('t'):
     case TJS_WC('T'):
       if(!TJS_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 200; return DP_TZ; }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -600; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('d'):
case TJS_WC('D'):
 switch(InputPointer[1])
 {
 case TJS_WC('e'):
 case TJS_WC('E'):
  switch(InputPointer[2])
  {
  case TJS_WC('c'):
  case TJS_WC('C'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 11; return DP_MONTH; }
    break;
   case TJS_WC('e'):
   case TJS_WC('E'):
    switch(InputPointer[4])
    {
    case TJS_WC('m'):
    case TJS_WC('M'):
     switch(InputPointer[5])
     {
     case TJS_WC('b'):
     case TJS_WC('B'):
      switch(InputPointer[6])
      {
      case TJS_WC('e'):
      case TJS_WC('E'):
       switch(InputPointer[7])
       {
       case TJS_WC('r'):
       case TJS_WC('R'):
         if(!TJS_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 11; return DP_MONTH; }
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
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 11; return DP_MONTH; }
   }
   break;
  }
  break;
 case TJS_WC('n'):
 case TJS_WC('N'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('e'):
case TJS_WC('E'):
 switch(InputPointer[1])
 {
 case TJS_WC('a'):
 case TJS_WC('A'):
  switch(InputPointer[2])
  {
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1000; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -400; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('e'):
 case TJS_WC('E'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
   switch(InputPointer[3])
   {
   case TJS_WC('d'):
   case TJS_WC('D'):
    switch(InputPointer[4])
    {
    case TJS_WC('s'):
    case TJS_WC('S'):
     switch(InputPointer[5])
     {
     case TJS_WC('t'):
     case TJS_WC('T'):
       if(!TJS_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 300; return DP_TZ; }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 200; return DP_TZ; }
   }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -500; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('f'):
case TJS_WC('F'):
 switch(InputPointer[1])
 {
 case TJS_WC('e'):
 case TJS_WC('E'):
  switch(InputPointer[2])
  {
  case TJS_WC('b'):
  case TJS_WC('B'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1; return DP_MONTH; }
    break;
   case TJS_WC('r'):
   case TJS_WC('R'):
    switch(InputPointer[4])
    {
    case TJS_WC('u'):
    case TJS_WC('U'):
     switch(InputPointer[5])
     {
     case TJS_WC('a'):
     case TJS_WC('A'):
      switch(InputPointer[6])
      {
      case TJS_WC('r'):
      case TJS_WC('R'):
       switch(InputPointer[7])
       {
       case TJS_WC('y'):
       case TJS_WC('Y'):
         if(!TJS_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 1; return DP_MONTH; }
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
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 1; return DP_MONTH; }
   }
   break;
  }
  break;
 case TJS_WC('r'):
 case TJS_WC('R'):
  switch(InputPointer[2])
  {
  case TJS_WC('i'):
  case TJS_WC('I'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 5; return DP_WDAY; }
    break;
   case TJS_WC('d'):
   case TJS_WC('D'):
    switch(InputPointer[4])
    {
    case TJS_WC('a'):
    case TJS_WC('A'):
     switch(InputPointer[5])
     {
     case TJS_WC('y'):
     case TJS_WC('Y'):
       if(!TJS_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 5; return DP_WDAY; }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 5; return DP_WDAY; }
   }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('w'):
 case TJS_WC('W'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 200; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('g'):
case TJS_WC('G'):
 switch(InputPointer[1])
 {
 case TJS_WC('m'):
 case TJS_WC('M'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 1000; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('h'):
case TJS_WC('H'):
 switch(InputPointer[1])
 {
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -900; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('i'):
case TJS_WC('I'):
 switch(InputPointer[1])
 {
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('l'):
  case TJS_WC('L'):
   switch(InputPointer[3])
   {
   case TJS_WC('e'):
   case TJS_WC('E'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1200; return DP_TZ; }
    break;
   case TJS_WC('w'):
   case TJS_WC('W'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = -1200; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 200; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('t'):
 case TJS_WC('T'):
   if(!TJS_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 330; return DP_TZ; }
  break;
 }
 break;
case TJS_WC('j'):
case TJS_WC('J'):
 switch(InputPointer[1])
 {
 case TJS_WC('a'):
 case TJS_WC('A'):
  switch(InputPointer[2])
  {
  case TJS_WC('n'):
  case TJS_WC('N'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 0; return DP_MONTH; }
    break;
   case TJS_WC('u'):
   case TJS_WC('U'):
    switch(InputPointer[4])
    {
    case TJS_WC('a'):
    case TJS_WC('A'):
     switch(InputPointer[5])
     {
     case TJS_WC('r'):
     case TJS_WC('R'):
      switch(InputPointer[6])
      {
      case TJS_WC('y'):
      case TJS_WC('Y'):
        if(!TJS_iswalpha(InputPointer[7])) { InputPointer += 7; yylex->val = 0; return DP_MONTH; }
       break;
      }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_MONTH; }
   }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 900; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('t'):
 case TJS_WC('T'):
   if(!TJS_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 730; return DP_TZ; }
  break;
 case TJS_WC('u'):
 case TJS_WC('U'):
  switch(InputPointer[2])
  {
  case TJS_WC('.'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 5; return DP_MONTH; }
   break;
  case TJS_WC('l'):
  case TJS_WC('L'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 6; return DP_MONTH; }
    break;
   case TJS_WC('y'):
   case TJS_WC('Y'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 6; return DP_MONTH; }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 6; return DP_MONTH; }
   }
   break;
  case TJS_WC('n'):
  case TJS_WC('N'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 5; return DP_MONTH; }
    break;
   case TJS_WC('e'):
   case TJS_WC('E'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 5; return DP_MONTH; }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 5; return DP_MONTH; }
   }
   break;
  default:
   if(!TJS_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 5; return DP_MONTH; }
  }
  break;
 }
 break;
case TJS_WC('k'):
case TJS_WC('K'):
 switch(InputPointer[1])
 {
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 900; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('l'):
case TJS_WC('L'):
 switch(InputPointer[1])
 {
 case TJS_WC('i'):
 case TJS_WC('I'):
  switch(InputPointer[2])
  {
  case TJS_WC('g'):
  case TJS_WC('G'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1000; return DP_TZ; }
    break;
   }
   break;
  }
  break;
 }
 break;
case TJS_WC('m'):
case TJS_WC('M'):
 switch(InputPointer[1])
 {
 case TJS_WC('a'):
 case TJS_WC('A'):
  switch(InputPointer[2])
  {
  case TJS_WC('r'):
  case TJS_WC('R'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 2; return DP_MONTH; }
    break;
   case TJS_WC('c'):
   case TJS_WC('C'):
    switch(InputPointer[4])
    {
    case TJS_WC('h'):
    case TJS_WC('H'):
      if(!TJS_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 2; return DP_MONTH; }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 2; return DP_MONTH; }
   }
   break;
  case TJS_WC('y'):
  case TJS_WC('Y'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 4; return DP_MONTH; }
   break;
  }
  break;
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -600; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('e'):
 case TJS_WC('E'):
  switch(InputPointer[2])
  {
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 200; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('t'):
  case TJS_WC('T'):
   switch(InputPointer[3])
   {
   case TJS_WC('d'):
   case TJS_WC('D'):
    switch(InputPointer[4])
    {
    case TJS_WC('s'):
    case TJS_WC('S'):
     switch(InputPointer[5])
     {
     case TJS_WC('t'):
     case TJS_WC('T'):
       if(!TJS_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 200; return DP_TZ; }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   }
   break;
  case TJS_WC('w'):
  case TJS_WC('W'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 100; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('z'):
  case TJS_WC('Z'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('o'):
 case TJS_WC('O'):
  switch(InputPointer[2])
  {
  case TJS_WC('n'):
  case TJS_WC('N'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1; return DP_WDAY; }
    break;
   case TJS_WC('d'):
   case TJS_WC('D'):
    switch(InputPointer[4])
    {
    case TJS_WC('a'):
    case TJS_WC('A'):
     switch(InputPointer[5])
     {
     case TJS_WC('y'):
     case TJS_WC('Y'):
       if(!TJS_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 1; return DP_WDAY; }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 1; return DP_WDAY; }
   }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -700; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('t'):
 case TJS_WC('T'):
   if(!TJS_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 830; return DP_TZ; }
  break;
 default:
  if(!TJS_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = -1200; return DP_TZ; }
 }
 break;
case TJS_WC('n'):
case TJS_WC('N'):
 switch(InputPointer[1])
 {
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -230; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('f'):
 case TJS_WC('F'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -330; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('o'):
 case TJS_WC('O'):
  switch(InputPointer[2])
  {
  case TJS_WC('r'):
  case TJS_WC('R'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  case TJS_WC('v'):
  case TJS_WC('V'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 10; return DP_MONTH; }
    break;
   case TJS_WC('e'):
   case TJS_WC('E'):
    switch(InputPointer[4])
    {
    case TJS_WC('m'):
    case TJS_WC('M'):
     switch(InputPointer[5])
     {
     case TJS_WC('b'):
     case TJS_WC('B'):
      switch(InputPointer[6])
      {
      case TJS_WC('e'):
      case TJS_WC('E'):
       switch(InputPointer[7])
       {
       case TJS_WC('r'):
       case TJS_WC('R'):
         if(!TJS_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 10; return DP_MONTH; }
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
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 10; return DP_MONTH; }
   }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -330; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('t'):
 case TJS_WC('T'):
   if(!TJS_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = -1100; return DP_TZ; }
  break;
 case TJS_WC('z'):
 case TJS_WC('Z'):
  switch(InputPointer[2])
  {
  case TJS_WC('d'):
  case TJS_WC('D'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1300; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1200; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 1200; return DP_TZ; }
   break;
  }
  break;
 default:
  if(!TJS_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = 100; return DP_TZ; }
 }
 break;
case TJS_WC('o'):
case TJS_WC('O'):
 switch(InputPointer[1])
 {
 case TJS_WC('c'):
 case TJS_WC('C'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 9; return DP_MONTH; }
    break;
   case TJS_WC('o'):
   case TJS_WC('O'):
    switch(InputPointer[4])
    {
    case TJS_WC('b'):
    case TJS_WC('B'):
     switch(InputPointer[5])
     {
     case TJS_WC('e'):
     case TJS_WC('E'):
      switch(InputPointer[6])
      {
      case TJS_WC('r'):
      case TJS_WC('R'):
        if(!TJS_iswalpha(InputPointer[7])) { InputPointer += 7; yylex->val = 9; return DP_MONTH; }
       break;
      }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 9; return DP_MONTH; }
   }
   break;
  }
  break;
 }
 break;
case TJS_WC('p'):
case TJS_WC('P'):
 switch(InputPointer[1])
 {
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -700; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('m'):
 case TJS_WC('M'):
   if(!TJS_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 0; return DP_PM; }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -800; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('s'):
case TJS_WC('S'):
 switch(InputPointer[1])
 {
 case TJS_WC('a'):
 case TJS_WC('A'):
  switch(InputPointer[2])
  {
  case TJS_WC('d'):
  case TJS_WC('D'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 1030; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 930; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('t'):
  case TJS_WC('T'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 6; return DP_WDAY; }
    break;
   case TJS_WC('u'):
   case TJS_WC('U'):
    switch(InputPointer[4])
    {
    case TJS_WC('r'):
    case TJS_WC('R'):
     switch(InputPointer[5])
     {
     case TJS_WC('d'):
     case TJS_WC('D'):
      switch(InputPointer[6])
      {
      case TJS_WC('a'):
      case TJS_WC('A'):
       switch(InputPointer[7])
       {
       case TJS_WC('y'):
       case TJS_WC('Y'):
         if(!TJS_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 6; return DP_WDAY; }
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
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 6; return DP_WDAY; }
   }
   break;
  }
  break;
 case TJS_WC('e'):
 case TJS_WC('E'):
  switch(InputPointer[2])
  {
  case TJS_WC('p'):
  case TJS_WC('P'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 8; return DP_MONTH; }
    break;
   case TJS_WC('t'):
   case TJS_WC('T'):
    switch(InputPointer[4])
    {
    case TJS_WC('.'):
      if(!TJS_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 8; return DP_MONTH; }
     break;
    case TJS_WC('e'):
    case TJS_WC('E'):
     switch(InputPointer[5])
     {
     case TJS_WC('m'):
     case TJS_WC('M'):
      switch(InputPointer[6])
      {
      case TJS_WC('b'):
      case TJS_WC('B'):
       switch(InputPointer[7])
       {
       case TJS_WC('e'):
       case TJS_WC('E'):
        switch(InputPointer[8])
        {
        case TJS_WC('r'):
        case TJS_WC('R'):
          if(!TJS_iswalpha(InputPointer[9])) { InputPointer += 9; yylex->val = 8; return DP_MONTH; }
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
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 8; return DP_MONTH; }
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 8; return DP_MONTH; }
   }
   break;
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 200; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('u'):
 case TJS_WC('U'):
  switch(InputPointer[2])
  {
  case TJS_WC('n'):
  case TJS_WC('N'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 0; return DP_WDAY; }
    break;
   case TJS_WC('d'):
   case TJS_WC('D'):
    switch(InputPointer[4])
    {
    case TJS_WC('a'):
    case TJS_WC('A'):
     switch(InputPointer[5])
     {
     case TJS_WC('y'):
     case TJS_WC('Y'):
       if(!TJS_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 0; return DP_WDAY; }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_WDAY; }
   }
   break;
  }
  break;
 case TJS_WC('w'):
 case TJS_WC('W'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 100; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('t'):
case TJS_WC('T'):
 switch(InputPointer[1])
 {
 case TJS_WC('h'):
 case TJS_WC('H'):
  switch(InputPointer[2])
  {
  case TJS_WC('u'):
  case TJS_WC('U'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 4; return DP_WDAY; }
    break;
   case TJS_WC('r'):
   case TJS_WC('R'):
    switch(InputPointer[4])
    {
    case TJS_WC('s'):
    case TJS_WC('S'):
     switch(InputPointer[5])
     {
     case TJS_WC('.'):
       if(!TJS_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 4; return DP_WDAY; }
      break;
     case TJS_WC('d'):
     case TJS_WC('D'):
      switch(InputPointer[6])
      {
      case TJS_WC('a'):
      case TJS_WC('A'):
       switch(InputPointer[7])
       {
       case TJS_WC('y'):
       case TJS_WC('Y'):
         if(!TJS_iswalpha(InputPointer[8])) { InputPointer += 8; yylex->val = 4; return DP_WDAY; }
        break;
       }
       break;
      }
      break;
     default:
      if(!TJS_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 4; return DP_WDAY; }
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 4; return DP_WDAY; }
   }
   break;
  }
  break;
 case TJS_WC('u'):
 case TJS_WC('U'):
  switch(InputPointer[2])
  {
  case TJS_WC('e'):
  case TJS_WC('E'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 2; return DP_WDAY; }
    break;
   case TJS_WC('s'):
   case TJS_WC('S'):
    switch(InputPointer[4])
    {
    case TJS_WC('.'):
      if(!TJS_iswalpha(InputPointer[5])) { InputPointer += 5; yylex->val = 2; return DP_WDAY; }
     break;
    case TJS_WC('d'):
    case TJS_WC('D'):
     switch(InputPointer[5])
     {
     case TJS_WC('a'):
     case TJS_WC('A'):
      switch(InputPointer[6])
      {
      case TJS_WC('y'):
      case TJS_WC('Y'):
        if(!TJS_iswalpha(InputPointer[7])) { InputPointer += 7; yylex->val = 2; return DP_WDAY; }
       break;
      }
      break;
     }
     break;
    default:
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 2; return DP_WDAY; }
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 2; return DP_WDAY; }
   }
   break;
  }
  break;
 }
 break;
case TJS_WC('u'):
case TJS_WC('U'):
 switch(InputPointer[1])
 {
 case TJS_WC('t'):
 case TJS_WC('T'):
  switch(InputPointer[2])
  {
  case TJS_WC('c'):
  case TJS_WC('C'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_TZ; }
   break;
  default:
   if(!TJS_iswalpha(InputPointer[2])) { InputPointer += 2; yylex->val = 0; return DP_TZ; }
  }
  break;
 }
 break;
case TJS_WC('w'):
case TJS_WC('W'):
 switch(InputPointer[1])
 {
 case TJS_WC('a'):
 case TJS_WC('A'):
  switch(InputPointer[2])
  {
  case TJS_WC('d'):
  case TJS_WC('D'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 800; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('s'):
  case TJS_WC('S'):
   switch(InputPointer[3])
   {
   case TJS_WC('t'):
   case TJS_WC('T'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 700; return DP_TZ; }
    break;
   }
   break;
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -100; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 900; return DP_TZ; }
   break;
  }
  break;
 case TJS_WC('e'):
 case TJS_WC('E'):
  switch(InputPointer[2])
  {
  case TJS_WC('d'):
  case TJS_WC('D'):
   switch(InputPointer[3])
   {
   case TJS_WC('.'):
     if(!TJS_iswalpha(InputPointer[4])) { InputPointer += 4; yylex->val = 3; return DP_WDAY; }
    break;
   case TJS_WC('n'):
   case TJS_WC('N'):
    switch(InputPointer[4])
    {
    case TJS_WC('e'):
    case TJS_WC('E'):
     switch(InputPointer[5])
     {
     case TJS_WC('s'):
     case TJS_WC('S'):
      switch(InputPointer[6])
      {
      case TJS_WC('d'):
      case TJS_WC('D'):
       switch(InputPointer[7])
       {
       case TJS_WC('a'):
       case TJS_WC('A'):
        switch(InputPointer[8])
        {
        case TJS_WC('y'):
        case TJS_WC('Y'):
          if(!TJS_iswalpha(InputPointer[9])) { InputPointer += 9; yylex->val = 3; return DP_WDAY; }
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
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 3; return DP_WDAY; }
   }
   break;
  case TJS_WC('t'):
  case TJS_WC('T'):
   switch(InputPointer[3])
   {
   case TJS_WC('d'):
   case TJS_WC('D'):
    switch(InputPointer[4])
    {
    case TJS_WC('s'):
    case TJS_WC('S'):
     switch(InputPointer[5])
     {
     case TJS_WC('t'):
     case TJS_WC('T'):
       if(!TJS_iswalpha(InputPointer[6])) { InputPointer += 6; yylex->val = 100; return DP_TZ; }
      break;
     }
     break;
    }
    break;
   default:
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 0; return DP_TZ; }
   }
   break;
  }
  break;
 case TJS_WC('s'):
 case TJS_WC('S'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = 800; return DP_TZ; }
   break;
  }
  break;
 }
 break;
case TJS_WC('y'):
case TJS_WC('Y'):
 switch(InputPointer[1])
 {
 case TJS_WC('d'):
 case TJS_WC('D'):
  switch(InputPointer[2])
  {
  case TJS_WC('t'):
  case TJS_WC('T'):
    if(!TJS_iswalpha(InputPointer[3])) { InputPointer += 3; yylex->val = -800; return DP_TZ; }
   break;
  }
  break;
 default:
  if(!TJS_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = 1200; return DP_TZ; }
 }
 break;
case TJS_WC('z'):
case TJS_WC('Z'):
  if(!TJS_iswalpha(InputPointer[1])) { InputPointer += 1; yylex->val = 0; return DP_TZ; }
 break;
}
