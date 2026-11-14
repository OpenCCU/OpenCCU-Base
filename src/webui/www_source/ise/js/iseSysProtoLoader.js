/**
 * ise/iseSysProtoLoader.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class
 **/
iseSysProtoLoader = Class.create();
iseSysProtoLoader.prototype =
{
  initialize: function(sTBody,iCount,bUpdateOnly)
  {
    this.tbody = $(sTBody);
    this.updateOnly = bUpdateOnly;
    this.currentIndex = 0;
    this.count = iCount;
    this.url = "/pages/tabs/control/systemProtocolLoader.htm?sid="+SessionId;
    Cursor.set(Cursor.WAIT);
    if( this.updateOnly )
    {
      this.updateTable();
    }
    else
    {
      arHistoryData = new Array();
      this.loadHistoryData();
    }
  },
  
  loadHistoryData: function()
  {
    var btnRefreshTable = jQuery("#btnRefreshTable");
    btnRefreshTable.prop("onclick","").unbind();
    var t = this;
    var pb = '';
    pb += 'string sStart = "'+this.currentIndex+'";';
    pb += 'string sCount = "'+this.count+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(transParams)
      {
        transParams.responseText.evalScripts();
        var resp = iseStripAll(transParams.responseText.stripScripts());
        var iResponse = parseInt( resp );
        if( isNaN(iResponse) ) { iResponse = 0; }
        if( iResponse !== 0 )
        {
          t.updateTable();
          t.currentIndex += iResponse;
          t.loadHistoryData();
        }
        else
        {
          Cursor.set(Cursor.NORMAL);
          // SPHM-645
          //if( !t.updateOnly ) DivSort("sqldate");
          btnRefreshTable.click(function() {refreshTable(false);});
        }
      }
    };
    new Ajax.Updater("dummy",this.url,opts);
  },
  
  updateTable: function()
  {
    if( this.updateOnly )
    {
      arTempHistoryData = arHistoryData;
    }
    for (var i = 0; i < arTempHistoryData.length; i++)
    {
      var sName = arTempHistoryData[i]['name'];
      var sValue = arTempHistoryData[i]['value'];
      var sDateTime = arTempHistoryData[i]['datetime'];
      var sDate = arTempHistoryData[i]['date'];
      var sTime = arTempHistoryData[i]['time'];
      
      var tr = Builder.node('tr', {id:  arTempHistoryData[i]['trid']} );
      var td;
      
      if( this.tbody.id == "sysprotopreview" )
      {
        td = Builder.node('td', {className: "GrayBkg"}, sDate+" "+sTime);
        tr.appendChild(td);        
      }
      else
      {
        td = Builder.node('td', {className: "GrayBkg"}, sDate);
        tr.appendChild(td);
  
        td = Builder.node('td', {className: "GrayBkg"}, sTime);
        tr.appendChild(td);
      }
  
      td = Builder.node('td', {className: "GrayBkg"}, translateString(sName));
      tr.appendChild(td);
  
      var div = document.createElement("div");
      //div.innerHTML = sValue;
      div.innerHTML = translateString(sValue) ;
      td = Builder.node('td', {className: "GrayBkg"}, div);
      Element.setStyle(td, {textAlign: "left", paddingLeft: "6px"});
      tr.appendChild(td);
      
      if( this.tbody )
      {
        this.tbody.appendChild(tr);
      }
    }
    if( this.updateOnly ) Cursor.set(Cursor.NORMAL);
    updateTable();
    arTempHistoryData = new Array();
  }
};
