Script::require("../arith-unary.rs");

var data =    [void,   0,   1, 0.0, 1.5, null,  "", "1.4", <% %>, <% 00 %>, false, true, new Object()];
var expects = [ 0.0, 0.0, 1.0, 0.0, 1.5,    I, 0.0,   1.4,     I,        I,   0.0,  1.0,            N];

test(data, expects) { |v| (real)v } ; //=> "ok"
