function tf(f) { f?"true":"false" }

 tf("\x1244!" < "2!") + ':' +
 tf("\x1244" < "\x1245") + ':' +
 tf(" " < "") + ':' +
 tf(" " < " ") + ':' +
 tf(" " < "  ") + ':' +
 tf("" < "") + ':' +
 tf("" < " ") + ':' +
 tf("aa" < "ab") + ':' +
 tf("aa" < "aa") //=> "false:true:false:false:true:false:true:true:false"

