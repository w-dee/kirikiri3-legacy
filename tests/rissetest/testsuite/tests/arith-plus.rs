Script::require("../arith-unary.rs");

var data =    [void, 0, 1, 0.0, 1.0, null, "", "a", "1", "1.0", "0xff", "077", <% %>, <% 00 %>, false, true, new Object()];
var expects = [   0, 0, 1, 0.0, 1.0,    I,  0,   0,   1,   1.0,    255,    63,     I,        I,     0,    1,            N];

test(data, expects) { |v| +v } ; //=> "ok"

