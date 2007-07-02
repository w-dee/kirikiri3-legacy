Script.require("../arith-matrix.rs");

// "8" = octet

var labels =  [    "V"   ,   "I"    ,   "R"    ,   "N"    ,   "S"    ,   "8"    ,   "T"    ,   "F"     ];
var values1 = [ void     ,    1     ,    1.0   ,    null  ,   "a"    , <%01%>   , true     ,  false    ];
var values2 = [ void     ,    2     ,    2.0   ,    null  ,   "b"    , <%02%>   , true     ,  false    ];

// E = Error

var expects =
[
//     ____V2____,____I2____,____R2____,____N2____,____S2____,____82____,____T2____,____F2____
/*V1*/[ void     ,    2     ,   2.0    ,    E     ,    "b"   ,<%02%>    ,    E     ,    E      ],
/*I1*/[ 1        ,    3     ,   3.0    ,    E     ,   "1b"   ,   E      ,    E     ,    E      ],
/*R1*/[ 1.0      ,    3.0   ,   3.0    ,    E     ,   "1b"   ,   E      ,    E     ,    E      ],
/*N1*/[    E     ,    E     ,    E     ,    E     ,    E     ,   E      ,    E     ,    E      ],
/*S1*/[ "a"      ,  "a2"    ,  "a2"    ,    E     ,   "ab"   ,   E      ,    E     ,    E      ],
/*81*/[ <%01%>   ,    E     ,    E     ,    E     ,    E     ,<%01 02%> ,    E     ,    E      ],
/*T1*/[    E     ,    E     ,    E     ,    E     ,    E     ,   E      ,    E     ,    E      ],
/*F1*/[    E     ,    E     ,    E     ,    E     ,    E     ,   E      ,    E     ,    E      ],
];

test(labels, values1, values2, expects) { |a,b| a + b };

//=> "ok"
