var tz = Date::timezoneOffset;
var tz_neg = tz < 0;
if(tz < 0) tz = -tz;
var tz_h = tz \ 60;
tz_h = tz_h < 10 ? ("0" + (string) tz_h) : (string) tz_h;
var tz_m = tz % 60;
tz_m = tz_m < 10 ? ("0" + (string) tz_m) : (string) tz_m;
var tz = (tz_neg ? '-' : '+') + tz_h + tz_m;

var d = new Date("2006");
assert(d.toString() == "Sun, 01 Jan 2006 00:00:00 GMT\{tz}");

var d = new Date("2006/5");
assert(d.toString() == "Mon, 01 May 2006 00:00:00 GMT\{tz}");

var d = new Date("2006/5/2");
assert(d.toString() == "Tue, 02 May 2006 00:00:00 GMT\{tz}");

var d = new Date("20060502");
assert(d.toString() == "Tue, 02 May 2006 00:00:00 GMT\{tz}");


var d = new Date("5/2");
p(d.toString());
p("Tue, 02 May \{Date.new().year} 00:00:00 GMT\{tz}");
assert(d.toString() == "Tue, 02 May \{Date.new().year} 00:00:00 GMT\{tz}");


