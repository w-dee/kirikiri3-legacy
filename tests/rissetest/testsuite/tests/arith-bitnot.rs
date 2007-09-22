Script::require("../arith-unary.rs");

var N = NoSuchMemberException;

var data =    [void,  0,  1, 0.0, 1.0, null, "", "a", <% %>, <% 00 %>, <% f0 %>, false, true, new Object()];
var expects = [  -1, -1, -2,  -1,  -2,    N,  N,   N, <% %>, <% ff %>, <% 0f %>,     N,    N,            N];

test(data, expects) { |v| ~v } ; //=> "ok"
