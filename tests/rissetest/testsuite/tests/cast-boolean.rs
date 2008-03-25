import * in test.unary;

var data =    [void, 0, 1, 0.0, 1.0, null, "", "a", <% %>, <% 00 %>, false, true, new Object()];
var expects = [   F, F, T,   F,   T,    F,  F,   T,     F,        T,     F,    T,            T];

test(data, expects) { |v| (boolean)v } ; //=> "ok"
