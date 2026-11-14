/**
 * ise/iseEventLog.js
 **/

/**
 * @fileOverview ?
 * @author Michael Niehaus (ise)
 **/

// Author: Michael Niehaus
// Created: 16.04.2007
// 2007-08-03 ASC: added deleteAll method.
// 2007-08-04 ASC: tblId not defined error fixed.

/**
 * @class
 **/
iseEventLog = Class.create();

iseEventLog.prototype = {
  initialize: function(parentElemId, bIsAlarms) {
    this.parentElemId = parentElemId;
    this.tblId = "tbl" + parentElemId;
    this.bIsAlarms = bIsAlarms;
    this.tBodyElem = null;
    this.buildTable();
  },
  
  // Add a message to the table structure
  add: function(s1, s2, s3) {
    var tr = Builder.node("tr");
    var td1 = Builder.node("td", {}, s1);
    var td2 = Builder.node("td", {}, s2);
    var td3= Builder.node("td", {}, s3);
    tr.appendChild(td1);
    if (!this.bIsAlarms) // Kanalname bei Alarmen ausblenden
      tr.appendChild(td2);
    tr.appendChild(td3);
    if (this.bIsAlarms) {
      var tdTmp = Builder.node("td", {}, "");
      tr.appendChild(tdTmp);
    }
    this.tBodyElem.appendChild(tr);
  },
  
  // Delete all rows in the table
  deleteAll: function()
  {
    if( $(this.tblId) )
    {
      while( $(this.tblId).rows.length )
      {
        $(this.tblId).deleteRow(0);
      }
    }
  },
  
  // Build table node
  buildTable: function() {
    var table = Builder.node("table", {id: this.tblId, style: "width:100%;white-space: nowrap;color:Black;"});
    var colgroup = Builder.node("colgroup");
    var col1 = Builder.node("col", {width: "25%"});
    var col2 = Builder.node("col", {width: "50%"});
    var col3= Builder.node("col", {width: "25%"});
    colgroup.appendChild(col1);
    colgroup.appendChild(col2);
    colgroup.appendChild(col3);
    table.appendChild(colgroup);
    var tbody = Builder.node("tbody");
    table.appendChild(tbody);
    
    this.tBodyElem = tbody;
    $(this.parentElemId).appendChild(table);
  }
};