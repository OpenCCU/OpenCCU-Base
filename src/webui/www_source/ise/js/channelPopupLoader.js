/**
 * ise/channelPopupLoader.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/
 
/**
 * @class
 **/
channelPopupLoader = Class.create();
channelPopupLoader.prototype =
{  
  initialize: function(path,colName,bSecondary,operations,channellist)
  {
    this.currentIndex = 0;
    this.count = 5;
    this.path = path;
    this.colName = colName;
    this.operations = operations;
    this.channellist = channellist;
    this.loadArray(this.currentIndex);
  },

  loadArray: function()
  {
    var url = this.path+"?sid="+SessionId;
    var pb = '';
    pb += 'string sStartPos = "'+this.currentIndex+'";';
    pb += 'string sCount = "'+this.count+'";';
    pb += 'string sOperations = "'+this.operations+'";';
    pb += 'string eChannelList = "'+this.channellist+'";';
    var me = this;
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        t.responseText.evalScripts();
        var resp = iseStripAll(t.responseText.stripScripts());
        if( resp == "stop" )
        {
          ChnSort(ccLastSort);
        }
        else
        {
          me.currentIndex+=me.count;
          me.loadArray();
        }
      }      
    };
    Cursor.set(Cursor.WAIT);
    new Ajax.Updater("dummy",url,opts);
  } 
  
};
