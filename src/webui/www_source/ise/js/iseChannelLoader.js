/**
 * ise/iseChannelLoader.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class
 **/ 
iseChannelLoader = Class.create();
iseChannelLoader.prototype =
{
  initialize: function(sTBody,iCountPerUpdate,bUpdateOnly)
  {
    this.tbody = $(sTBody);
    this.updateOnly = bUpdateOnly;
    this.currentIndex = 0;
    this.count = iCountPerUpdate;
    this.url = "/pages/tabs/statusviews/channelLoader.htm?sid="+SessionId;
    Cursor.set(Cursor.WAIT);
    if( this.updateOnly )
    {
      this.updateTable();
    }
    else
    {
      arChannels = new Array();
      this.loadChannels();
    }
  },
  loadChannels: function()
  {
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
        //alert("["+resp+"]");
        if( resp != "0" )
        {
          t.updateTable();
          t.currentIndex += parseInt(resp,10);
          t.loadChannels();
        }
        else
        {
          Cursor.set(Cursor.NORMAL);
          if( !t.updateOnly ) DivSort("name");
        }
      }
    };
    new Ajax.Updater("dummy",this.url,opts);
  },
  updateTable: function()
  {
    if( this.updateOnly )
    {
      arTempChannels = arChannels;
    }
    for (var i = 0; i < arTempChannels.length; i++)
    {
      var tr = Builder.node('tr', {id: 'tr'+arTempChannels[i]['id']} );
      var td;
      
      var sName = arTempChannels[i]['name'];
      td = Builder.node('td', {className: "GrayBkg"}, sName);
      tr.appendChild(td);
      
      var sRooms = arTempChannels[i]['rooms'];
      td = Builder.node('td', {className: "GrayBkg"}, sRooms);
      tr.appendChild(td);
      
      var sFunctions = arTempChannels[i]['funcs'];
      td = Builder.node('td', {className: "GrayBkg"}, sFunctions);
      tr.appendChild(td);
      
      var sLastTime = arTempChannels[i]['lasttime'];
      td = Builder.node('td', {id: 'tmc'+arTempChannels[i]['id'], className: "GrayBkg"}, sLastTime);
      tr.appendChild(td);
      
      var sDatapoint = arTempChannels[i]['id'];
      td = Builder.node('td', {id: 'dpc'+arTempChannels[i]['id'], className: "GrayBkg"});
      tr.appendChild(td);
      
      if( this.tbody ) this.tbody.appendChild(tr);
      
      recreateControl(arTempChannels[i]['id'],sLastTime);
    }
    
    if( this.updateOnly ) Cursor.set(Cursor.NORMAL);
    arTempChannels = new Array();
  }
};
