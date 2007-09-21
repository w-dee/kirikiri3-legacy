Script::require("../arith-unary.rs");

var N = NoSuchMemberException;

var data =    [void, 0, 1, 0.0, 1.0, null, "", "1", <% %>, <% 00 %>, false, true, new Object()];
var expects = [   0, 0, 1,   0,   1,    N,  0,   1,     N,        N,     0,    1,            N];

test(data, expects) { |v| (integer)v } ; //=> "ok"
