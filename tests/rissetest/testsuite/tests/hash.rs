function make_hash(iv)
{
	iv = (integer)iv;
	var hash = iv;
	hash += (hash << 10);
	hash &= 0xffffffff;
	hash ^= (hash >> 6);
	hash &= 0xffffffff;
	hash += (iv >> 11);
	hash &= 0xffffffff;
	hash += (hash << 10);
	hash &= 0xffffffff;
	hash ^= (hash >> 6);
	hash &= 0xffffffff;
	hash += (iv >> 32);
	hash &= 0xffffffff;
	hash += (hash << 10);
	hash &= 0xffffffff;
	hash ^= (hash >> 6);
	hash &= 0xffffffff;
	hash += (hash << 3);
	hash &= 0xffffffff;
	hash ^= (hash >> 10);
	hash &= 0xffffffff;
	hash += (hash << 15);
	return hash & 0xffffffff;
}

assert(void.hash == 0x8f84b331 && void.hint == void.hash);
assert(0 .hash == make_hash(0) && 0 .hash == 0 .hint);
assert(17 .hash == make_hash(17) && 17 .hash == 17 .hint);


assert(0b10.01 . hash == make_hash(0x4002000000000000));
	// 0b10.01 = 2.25, IEEE double:  0x4002000000000000
assert(0b10.01 . hash == 0b10.01 . hint);

assert(NaN.hash == 1 && NaN.hash == NaN.hint);
assert((-NaN).hash == 1 && (-NaN).hash == (-NaN).hint);
assert(Infinity.hash == 2 && Infinity.hash == Infinity.hint);
assert((-Infinity).hash == 3 && (-Infinity).hash == (-Infinity).hash);
// assert(null.hash == 0x9b371c72);
// assert ( octet hash, Octet class not implemented yet )
assert(true.hash == 0xab2ed843 && true.hash == true.hint);
assert(false.hash == 0xbd8b88c4 && false.hash == false.hint);

// 文字列の hint は常に hash と同じである保証はない
assert("".hash == 0xffffffff);
assert("0".hash == 0x6e3c5c6b);
assert("1".hash == 0x806b80c9);
assert("expression".hash == 0x7a5ba1cb);

"ok" //=> "ok"
