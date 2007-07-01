
var a1 = "" + void; // implicit toString
var a2 = void.toString();
var a3 = void.dump();

@"${a1}:${a2}:${a3}" //=> "::void"

