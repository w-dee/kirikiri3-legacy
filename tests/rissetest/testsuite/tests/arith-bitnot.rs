Script::require("../arith-unary.rs");

var data =    [void,  0,  1, 0.0, 1.0, null, "", "a", <% %>, <% 00 %>, <% f0 %>, false, true, new Object()];
var expects = [  -1, -1, -2,  -1,  -2,    I,  I,   I, <% %>, <% ff %>, <% 0f %>,     I,    I,            N];

test(data, expects) { |v| ~v } ; //=> "ok"
