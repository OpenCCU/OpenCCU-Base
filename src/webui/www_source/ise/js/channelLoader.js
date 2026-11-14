/**
 * ise/channelLoader.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/
 
/**
 * @class
 **/
channelLoader = Class.create();

channelLoader.prototype = {
  /*
   * id = DOM-ID of switch
   * initState = Creation State 
   */
  initialize: function(channelCount) {
    this.channelCount = channelCount;    
    this.currentIndex = 0;
    this.loadChannels(this.currentIndex);
  },

  
  loadChannels: function(index) {
    var url = "";
    var t = this;
    var pb ="";
    //pb += "system.SetSessionVar('sessionIdx', " + this.currentIndex + ");";
    pb += 'string index = "'+index+'";';
    var opts = {
      postBody: ReGa.encode(pb),
      evalScripts: true,
      onComplete: function(transParams) {
        t.currentIndex+=5;
        if (t.currentIndex <= t.channelCount)
        {
          t.loadChannels(t.currentIndex);
        }
        else
        {
          if(FirstSort){FirstSort();}
        }
        Cursor.set(Cursor.NORMAL);
      }
    };
    new Ajax.Updater("dummy", "/pages/tabs/statusviews/channelBody.htm?sid=" + SessionId, opts);
  }
};

/**
 * @class
 **/
newDevChnLoader = Class.create();
newDevChnLoader.prototype = {
  /*
   * id = DOM-ID of switch
   * initState = Creation State 
   */
  initialize: function(channelCount) {
    this.channelCount = channelCount;    
    this.currentIndex = 0;
    this.loadChannels(channelCount);
  },

  
  loadChannels: function(index)
  {
    var url = "";
    var t = this;
    var pb ="";
    pb += "system.SetSessionVar('sessionIdx', '" + this.currentIndex + "');";
    //pb += 'string index = "'+index+'";';
    var opts = {
      postBody: ReGa.encode(pb),
      evalScripts: true,
      onComplete: function(transParams) {
        t.currentIndex+=5;
        if (t.currentIndex <= t.channelCount)
        {
          t.loadChannels(t.currentIndex);
        }
        else
        {
          Cursor.set(Cursor.NORMAL);
        }
      }
    };
    new Ajax.Updater("dummy", "/pages/tabs/admin/views/newdevicechannelsloader.htm?sid=" + SessionId, opts);
  }
};


