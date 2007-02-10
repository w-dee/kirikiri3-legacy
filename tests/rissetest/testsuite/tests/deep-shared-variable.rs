function callback() proc { return proc(); }

function a()
{
	var i = "deadbeef";
	function b()
	{
		var j = i;
		function c()
		{
			var k = j;
			return global.callback() {
				var l = k;
				function d()
				{
					var m = l;
					function e()
					{
						var n  = m;
						return n;
					}
					return e();
				}
				continue d();
			};
		}
		return c();
	}
	return b();
}
return a(); //=> "deadbeef"

