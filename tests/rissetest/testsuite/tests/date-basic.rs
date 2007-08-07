var d = new Date();
var d2 = new Date(d); // 複製

d.year = d.getYear();
d.month = d.getMonth();
d.date = d.getDate();
d.hours = d.getHours();
d.minutes = d.getMinutes();
d.seconds = d.getSeconds();
d.milliseconds = d.getMilliseconds();

assert(d == d2); // 内容が変わっていないことを確認

d = new Date(d2.getTime());

assert(d == d2); // 内容が変わっていないことを確認

d.setYear(d.getYear());
d.setMonth(d.getMonth());
d.setDate(d.getDate());
d.setHours(d.getHours());
d.setMinutes(d.getMinutes());
d.setSeconds(d.getSeconds());
d.setMilliseconds(d.getMilliseconds());

assert(d == d2); // 内容が変わっていないことを確認

"ok"; //=> "ok"
