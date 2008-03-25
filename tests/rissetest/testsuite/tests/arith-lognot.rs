import * in test.unary;

var data =    [void, 0, 1, 0.0, 1.0, null, "", "a", <% %>, <% 00 %>, false, true, new Object()];
var expects = [   T, T, F,   T,   F,    T,  T,   F,     T,        F,     T,    F,            F];

test(data, expects) { |v| !v } ; //=> "ok"
