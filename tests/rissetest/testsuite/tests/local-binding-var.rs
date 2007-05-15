var a = 0;

(@).eval("var a = 1");


a; //=> 0

// eval on (@) makes variable local

