var s1 = "abcdef"; // 6 length
assert(s1.substr(0, 6) === "abcdef");
assert(s1.substr(0   ) === "abcdef");
assert(s1.substr(0, 0) === "");
assert(s1.substr(0, 5) === "abcde");
assert(s1.substr(1, 5) === "bcdef");
assert(s1.substr(1, 6) === "bcdef");
assert(s1.substr(1, 7) === "bcdef");
assert(s1.substr(-1, 7) === "f");
assert(s1.substr(-6, 6) === "abcdef");
assert(s1.substr(-7, 6) === void);
assert(s1.substr(-7, 0) === void);
assert(s1.substr(-7, 0) === void);
assert(s1.substr(6, 6) === void);
assert(s1.substr(6, 0) === void);

"ok"; //=> "ok"
