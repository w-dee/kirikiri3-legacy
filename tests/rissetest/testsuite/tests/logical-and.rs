	((false && false) ? "ng" : "ok" ) + ':' +
	((false && true ) ? "ng" : "ok" ) + ':' +
	((true  && false) ? "ng" : "ok" ) + ':' +
	((true  && true ) ? "ok" : "ng" ) + ':' +
	((1 && 2 && 3) ? "ok" : "ng" )
  //=> "ok:ok:ok:ok:ok"
