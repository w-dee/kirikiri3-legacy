var array = [1, 2, 3, false, 0, 4, true, 3, 1];

assert(array.remove("fasdfasd") === void);
assert(array.remove(3) === 3);
assert(array.length == 7);
assert(array.remove(1, false) === 1);
assert(array.length == 6);

// array = [2, false, 0, 4, true, 1]

assert(array.erase(0) === 2);
assert(array.erase(-1) === 1);
assert(array[1] === 0);
assert(array.length == 4);

"ok" //=> "ok"



