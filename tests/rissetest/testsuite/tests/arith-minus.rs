Script::require("../arith-unary.rs");

var N = NoSuchMemberException;

var data =    [void, 0,  1, -1, -Infinity, 0.0,  1.0, -1.0, null, "", "a", <% %>, <% 00 %>, false, true, new Object()];
var expects = [   0, 0, -1,  1, +Infinity, 0.0, -1.0,  1.0,    N,  N,   N,     N,        N,     N,    N,            N];

test(data, expects) { |v| -v } ; //=> "ok"

