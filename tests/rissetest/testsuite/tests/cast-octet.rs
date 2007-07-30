// このスクリプトは UTF-8 文字列を含んでいます。
// This script contains UTF-8 strings.

var a1 = (octet)"" === <% %> ? "ok" : "ng";
var a2 = (octet)"abcあいう漢字" ===  <% 61 62 63 e3 81 82 e3 81 84 e3 81 86 e6 bc a2 e5 ad 97 %> ? "ok" : "ng";

return a1 + ":" + a2; //=> "ok:ok"


