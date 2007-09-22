Script::require("../arith-unary.rs");

var data =    [void, 0,  1, -1, -Infinity, 0.0,  1.0, -1.0, null, "", "a", <% %>, <% 00 %>, false, true, new Object()];
var expects = [   0, 0, -1,  1, +Infinity, 0.0, -1.0,  1.0,    I,  I,   I,     I,        I,     I,    I,            N];

test(data, expects) { |v| -v } ; //=> "ok"

