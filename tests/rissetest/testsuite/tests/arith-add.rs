Script::require("../arith-matrix.rs");

// "A" = any other instances

var labels =  [    "V"   ,   "I"    ,   "R"    ,   "N"    ,   "S"    ,   "O"    ,   "T"    ,   "F"   ,  "A"          ];
var values1 = [ void     ,    1     ,    1.0   ,    null  ,   "a"    , <%01%>   , true     ,  false  ,  new Object() ];
var values2 = [ void     ,    2     ,    2.0   ,    null  ,   "b"    , <%02%>   , true     ,  false  ,  new Object() ];

var eT = IllegalArgumentTypeException;
var eM = NoSuchMemberException;

var expects =
[
//     ____V2____,____I2____,____R2____,____N2____,____S2____,____O2____,____T2____,____F2____,____A2____
/*V1*/[ void     ,    2     ,   2.0    ,    eT    ,    "b"   ,<%02%>    ,   eT     ,   eT     ,   eT     ],
/*I1*/[ 1        ,    3     ,   3.0    ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ],
/*R1*/[ 1.0      ,    3.0   ,   3.0    ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ],
/*N1*/[   eT     ,   eT     ,   eT     ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ],
/*S1*/[ "a"      ,   eT     ,   eT     ,    eT    ,   "ab"   ,   eT     ,   eT     ,   eT     ,   eT     ],
/*O1*/[ <%01%>   ,   eT     ,   eT     ,    eT    ,   eT     ,<%01 02%> ,   eT     ,   eT     ,   eT     ],
/*T1*/[   eT     ,   eT     ,   eT     ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ],
/*F1*/[   eT     ,   eT     ,   eT     ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ],
/*A1*/[   eM     ,   eM     ,   eM     ,    eM    ,   eM     ,   eM     ,   eM     ,   eM     ,   eM     ],
];

test(labels, values1, values2, expects) { |a,b| a + b };

//=> "ok"
