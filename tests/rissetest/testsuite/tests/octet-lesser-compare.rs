function tf(f) { f?"true":"false" }

 tf(<% 01 %> < <% 00 %>) + ':' +
 tf(<% 01 %> < <% 02 %>) + ':' +
 tf(<% 00 %> < <% %>) + ':' +
 tf(<% 00 %> < <% 00 %>) + ':' +
 tf(<% 00 %> < <% 00 00 %>) + ':' +
 tf(<% %> < <% %>) + ':' +
 tf(<% %> < <% 00 %>) + ':' +
 tf(<% 00 00 %> < <% 00 01 %>) + ':' +
 tf(<% 00 00 %> < <% 00 00 %>) //=> "false:true:false:false:true:false:true:true:false"
