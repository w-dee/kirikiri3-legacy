Script::require("../arith-unary.rs");

var N = NoSuchMemberException;
var I = IllegalArgumentClassException;

var data =    [void,  0,  1, 0.0, 1.0, null, "", "a", <% %>, <% 00 %>, false, true, new Object()];
var expects = [   1,  1,  2, 1.0, 2.0,    I,  I,   I,     I,        I,     I,    I,            N];

test(data, expects) { |v| ++v } ; //=> "ok"
