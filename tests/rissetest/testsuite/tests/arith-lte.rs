Script::require("../arith-matrix.rs");

// "A" = any other instances

var labels =  [    "V"   ,   "i"    ,   "I"    ,   "r"    ,   "R"    ,   "N"    ,   "."    ,   "s"    ,   "S"    ,   "."    ,   "o"    ,   "O"    ,   "T"    ,   "F"   ,  "A"          ];
var values1 = [ void     ,    1     ,    2     ,    1.0   ,    2.0   ,    null  ,   ""     ,   "aa"   ,   "bb"   , <% %>    , <%01%>   , <%02%>   , true     ,  false  ,  new Object() ];
var values2 = [ void     ,    1     ,    2     ,    1.0   ,    2.0   ,    null  ,   ""     ,   "aa"   ,   "bb"   , <% %>    , <%01%>   , <%02%>   , true     ,  false  ,  new Object() ];

var eT = IllegalArgumentClassException;
var eM = NoSuchMemberException;

var expects =
[
//     ____V2____,____i2____,____I2____,____r2____,____R2____,____N2____,____.2____,____s2____,____S2____,____.2____,____o2____,____O2____,____T2____,____F2____,____A2____
/*V1*/[  true    ,  true    ,  true    ,  true    ,  true    ,    eT    ,  true    ,  true    ,  true    ,  true    ,  true    ,  true    ,   eT     ,   eT     ,   eT      ],
/*i1*/[  false   ,  true    ,  true    ,  true    ,  true    ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*I1*/[  false   ,  false   ,  true    ,  false   ,  true    ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*r1*/[  false   ,  true    ,  true    ,  true    ,  true    ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*R1*/[  false   ,  false   ,  true    ,  false   ,  true    ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*N1*/[   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*.1*/[  true    ,   eT     ,   eT     ,   eT     ,   eT     ,    eT    ,  true    ,  true    ,  true    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*s1*/[  false   ,   eT     ,   eT     ,   eT     ,   eT     ,    eT    ,  false   ,  true    ,  true    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*S1*/[  false   ,   eT     ,   eT     ,   eT     ,   eT     ,    eT    ,  false   ,  false   ,  true    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*.1*/[  true    ,   eT     ,   eT     ,   eT     ,   eT     ,    eT    ,   eT     ,   eT     ,   eT     ,  true    ,  true    ,  true    ,   eT     ,   eT     ,   eT      ],
/*o1*/[  false   ,   eT     ,   eT     ,   eT     ,   eT     ,    eT    ,   eT     ,   eT     ,   eT     ,  false   ,  true    ,  true    ,   eT     ,   eT     ,   eT      ],
/*O1*/[  false   ,   eT     ,   eT     ,   eT     ,   eT     ,    eT    ,   eT     ,   eT     ,   eT     ,  false   ,  false   ,  true    ,   eT     ,   eT     ,   eT      ],
/*T1*/[   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*F1*/[   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,    eT    ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT     ,   eT      ],
/*A1*/[   eM     ,   eM     ,   eM     ,   eM     ,   eM     ,    eM    ,   eM     ,   eM     ,   eM     ,   eM     ,   eM     ,   eM     ,   eM     ,   eM     ,   eM      ],
];

var a1 = test(labels, values1, values2, expects) { |a,b| a <= b };

// let all A2 column eM, A1 row (except for A1/A2) eT
for(var i = 0; i < expects.length; i++) expects[i][expects[i].length-1] = eM;
for(var i = 0; i < expects[expects.length-1].length - 1; i++) expects[expects.length-1][i] = eT;

var a2 = test(labels, values1, values2, expects) { |a,b| b >= a };

a1 + a2; //=> "okok"
