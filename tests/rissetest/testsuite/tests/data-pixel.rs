assert(pixel.a == 0xaa);
assert(pixel.r == 0x11);
assert(pixel.g == 0x22);
assert(pixel.b == 0x33);

pixel.a = 255;
pixel.r = 255;
pixel.g = 255;
pixel.b = 255;

assert(pixel.value == 0xffffffff);

"ok" //=> "ok"

