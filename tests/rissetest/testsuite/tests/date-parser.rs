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

var d = new Date("2006-5");
assert(d.toString() == "Mon, 01 May 2006 00:00:00 GMT\{tz}");

var d = new Date("2006/May");
assert(d.toString() == "Mon, 01 May 2006 00:00:00 GMT\{tz}");

var d = new Date("May/2006");
assert(d.toString() == "Mon, 01 May 2006 00:00:00 GMT\{tz}");

var d = new Date("2006/5/2");
assert(d.toString() == "Tue, 02 May 2006 00:00:00 GMT\{tz}");

var d = new Date("2006-05-02");
assert(d.toString() == "Tue, 02 May 2006 00:00:00 GMT\{tz}");

var d = new Date("2006/May/2");
assert(d.toString() == "Tue, 02 May 2006 00:00:00 GMT\{tz}");

var d = new Date("2006/2/May");
assert(d.toString() == "Tue, 02 May 2006 00:00:00 GMT\{tz}");

var d = new Date("May/2/2006");
assert(d.toString() == "Tue, 02 May 2006 00:00:00 GMT\{tz}");

var d = new Date("2/May/2006");
assert(d.toString() == "Tue, 02 May 2006 00:00:00 GMT\{tz}");

var d = new Date("20060502");
assert(d.toString() == "Tue, 02 May 2006 00:00:00 GMT\{tz}");

var d = new Date("20060502T122334");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("2006-05-02T12:23");
assert(d.toString() == "Tue, 02 May 2006 12:23:00 GMT\{tz}");

var d = new Date("2006-05-02T12:23Z\{tz}");
assert(d.toString() == "Tue, 02 May 2006 12:23:00 GMT\{tz}");

var d = new Date("2006-05-02T12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("20060502T122334Z\{tz}");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("20060502T122334\{tz}");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Tue, 02 May 2006 12:23:34 GMT\{tz}");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Tue, 02 May 06 12:23:34 GMT\{tz}");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Tue, 02 May 106 12:23:34 GMT\{tz}");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Sun, 02 May 99 12:23:34 GMT\{tz}");
assert(d.toString() == "Sun, 02 May 1999 12:23:34 GMT\{tz}");

var d = new Date("Tue, 02 (uh?)May 2006 (?) 12:23:34 GMT\{tz} (untarakantara)");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Tue, 02-May 2006 12:23:34 GMT\{tz}");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Tue 02 May 2006 12:23:34 GMT\{tz}");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Tue 02-May 2006 12:23:34 GMT\{tz}");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Tue May 2 12:23:34 GMT\{tz} 2006");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Tue May 2 12:23:34 GMT\{tz} 06");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Tue May 2 12:23:34 GMT\{tz} 106");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("Sun May 2 12:23:34 GMT\{tz} 99");
assert(d.toString() == "Sun, 02 May 1999 12:23:34 GMT\{tz}");

var d = new Date("2006/5/2 12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("2006/5/2 12:23:34.11");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("2006/5/2 am 8:23:34");
assert(d.toString() == "Tue, 02 May 2006 08:23:34 GMT\{tz}");

var d = new Date("2006/5/2 pm 8:23:34");
assert(d.toString() == "Tue, 02 May 2006 20:23:34 GMT\{tz}");

var d = new Date("2006/5/2 8:23:34 am");
assert(d.toString() == "Tue, 02 May 2006 08:23:34 GMT\{tz}");

var d = new Date("2006/5/2 8:23:34 pm");
assert(d.toString() == "Tue, 02 May 2006 20:23:34 GMT\{tz}");

var d = new Date("2006/5/2 8:23:34pm");
assert(d.toString() == "Tue, 02 May 2006 20:23:34 GMT\{tz}");

var d = new Date("2006/May/2 12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("May/2/2006 12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("2/May/2006 12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("2006/2/May 12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("2006 2 May, 12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("2006 May 2, 12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("may 2 2006 12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

var d = new Date("may 2 2006, 12:23:34");
assert(d.toString() == "Tue, 02 May 2006 12:23:34 GMT\{tz}");

"ok"; //=> "ok"
