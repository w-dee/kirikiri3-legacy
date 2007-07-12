	((false || false) ? "ng" : "ok" ) + ':' +
	((false || true ) ? "ok" : "ng" ) + ':' +
	((true  || false) ? "ok" : "ng" ) + ':' +
	((true  || true ) ? "ok" : "ng" ) + ':' +
	((1 || 0 || 3) ? "ok" : "ng" )
  //=> "ok:ok:ok:ok:ok"
