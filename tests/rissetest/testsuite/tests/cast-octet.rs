// このスクリプトは UTF-8 文字列を含んでいます。
// This script contains UTF-8 strings.
import * in test.unary;

var data =    [ void, 0, 1, 0.0, 1.5, null,     "",           "1.4", <% %>,                                              "abcあいう漢字", false, true, new Object()];
var expects = [<% %>, I, I,   I,   I,    I,  <% %>, <% 31 2e 34 %> , <% %>,  <% 61 62 63 e3 81 82 e3 81 84 e3 81 86 e6 bc a2 e5 ad 97 %>,     I,    I,            N];

test(data, expects) { |v| (octet)v } ; //=> "ok"
