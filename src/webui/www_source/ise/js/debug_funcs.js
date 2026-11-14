/**
 * ise/debug_funcs.js
 **/

/**
 * @fileOverview Debug-Funktionen
 * @author ise
 **/
 
debugAddMsg = function() {
  if(typeof(eventLogSystem) == "undefined") {
    $("dbgLog").innerHTML = "eventLogSystem -> undefined";
  } 
  else {
    $("dbgLog").innerHTML = "calling eventLogSystem.add()";
    eventLogSystem.add("10.07.2007 12:15", "Wetterstation", "LowBat");
  }
};

createDebugAlarmMsgs = function() {
  
  var table = Builder.node("table", {id:"tblDebugAlarms", style: "width:100%;color:Black;"});
  var colgroup = Builder.node("colgroup");
  var col1 = Builder.node("col", {width: "25%"});
  var col2 = Builder.node("col", {width: "50%"});
  var col3= Builder.node("col", {width: "25%"});
  colgroup.appendChild(col1);
  colgroup.appendChild(col2);
  colgroup.appendChild(col3);
  table.appendChild(colgroup);
  // Zeile 1 erstellen
  var tbody = Builder.node("tbody");
  var tr = Builder.node("tr");
  var td1 = Builder.node("td", {}, "01.03. 13:22");
  var td2 = Builder.node("td", {}, "Bewegungsmelder - Eingang");
  var td3 = Builder.node("td", {}, "Low Bat");
  
  // Zeile 1 hinzufügen
  tr.appendChild(td1);
  tr.appendChild(td2);
  tr.appendChild(td3);
  tbody.appendChild(tr);
  
  // Zeile 2 erstellen
  tr = Builder.node("tr");
  td1 = Builder.node("td", {}, "01.03. 13:19");
  td2 = Builder.node("td", {}, "Kombi-Wettersensor");
  td3 = Builder.node("td", {}, "Empfangsausfall");
  
  // Zeile 2 hinzufügen
  tr.appendChild(td1);
  tr.appendChild(td2);
  tr.appendChild(td3);
  
  tbody.appendChild(tr);
  
  tr = Builder.node("tr");td1 = Builder.node("td", {}, "01.03. 13:44");td2 = Builder.node("td", {}, "Mülleimer");td3 = Builder.node("td", {}, "Voll");tr.appendChild(td1);tr.appendChild(td2);tr.appendChild(td3);
  tbody.appendChild(tr);
  tr = Builder.node("tr");td1 = Builder.node("td", {}, "01.03. 13:45");td2 = Builder.node("td", {}, "Badewanne");td3 = Builder.node("td", {}, "Übergelaufen");tr.appendChild(td1);tr.appendChild(td2);tr.appendChild(td3);
  tbody.appendChild(tr);
  
  table.appendChild(tbody);

  if( $('logAlarms') )
  {
    $('logAlarms').appendChild(table);
  }
};

debugCreatePopup = function() {
  var s = $("dbgSelect").options[$("dbgSelect").selectedIndex].text;
  var type = 0;
  if (s == "ID_INSERT_VALUE")
     type = 1;
   if (s == "ID_INSERT_STRING")
     type = 2;
  CreatePopup(eval(s), type);
};

debugForcePos = function() {
  if ($("btnRoomsSub")) {
    var topCoords = Position.positionedOffset($("btnRooms"));
    var offsetCoords = Position.positionedOffset($("subOffsetDiv"));
    var dimTop = $("btnRooms").getDimensions();
    
    $("btnRoomsSub").style.top = topCoords[1] - offsetCoords[1] + dimTop.height;
    $("btnRoomsSub").style.left = topCoords[0] - offsetCoords[0] - 5;
    $("btnRoomsSub").show();
  }
};

debugShowFilters = function() {
  if (typeof(filterOptions) != 'undefined') {
    var s = "";
    for (var i = 0; i < filterOptions.filters.length; i++) {
      s += filterOptions.filters[i]["id"] + "\n";
    }
    if(dbg)alert(s);
  }
  else {
    if(dbg)alert("No filter options");
  }
};

showTimemodule = function() {
  CreatePopup(ID_TIMEMODULE, $("tmID").value);
};