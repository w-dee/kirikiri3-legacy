Script::require("../arith-unary.rs");

var N = NoSuchMemberException;

var data =    [void,   0,   1,   0.0,   1.0,   -0.0,   +Infinity,   +NaN,   -Infinity,   -NaN, null, "", "1", <% %>, <% 00 %>,   false,   true, new Object()];
var expects = [  "", "0", "1", "0.0", "1.0", "-0.0", "+Infinity",  "NaN", "-Infinity",  "NaN",    N, "", "1",     N,        N, "false", "true",            N];

test(data, expects) { |v| (string)v } ; //=> "ok"
