Script::require("../arith-unary.rs");

var N = NoSuchMemberException;

var data =    [void,   0,   1, 0.0, 1.5, null,  "", "1.4", <% %>, <% 00 %>, false, true, new Object()];
var expects = [ 0.0, 0.0, 1.0, 0.0, 1.5,    N, 0.0,   1.4,     N,        N,   0.0,  1.0,            N];

test(data, expects) { |v| (real)v } ; //=> "ok"
