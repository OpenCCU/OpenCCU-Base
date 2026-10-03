/**
 * configdata.js
 **/
 
/**
 * @fileOverview Daten für die Konfgurationsseiten
 * @author F. Werner (eQ-3)
 **/
 
ConfigData = Singleton.create({

  initialize: function()
  {
    this.isPresent = false;
    this.showMessage = false;
  },
  
  check: function(callback)
  {
    this.callback = callback;
    if (this.isPresent === false)
    {
      this.isPresent = true;
      new ConfigDataLoader(callback);
      //this.configDataLoader.showMessage();
      //this.showMessage = true;
    }
    else
    {
      if (callback) { callback(); }
    }
  },

  reload: function(callback) {
    //var loader = new ConfigDataLoader(callback);
    this.isPresent = false;
    this.showMessage = true;
    this.configDataLoader = new ConfigDataLoader(callback);
    this.configDataLoader.showMessage();
  },

  handleReloadDone: function() {
    //Shall be called in callback function of reload function
    this.isPresent = true;
    if (this.showMessage) { this.configDataLoader.hideMessage(); }
  },


  load: function() {
    if ((typeof measureLoadingConfigData != "undefined") && (measureLoadingConfigData == true)) {console.log("Start ConfigDataLoader"); console.time(); }
    this.configDataLoader = new ConfigDataLoader(function() {
      if ((typeof measureLoadingConfigData != "undefined") && (measureLoadingConfigData == true)) {console.timeEnd(); }
      conInfo("Config data ready to use");
      jQuery("#PagePath").css('color',"white");
      // With a CCU without devices the elem PagePath isn't available sometimes at this point,
      // so the color doesn't change to white. The next line prevents that.
      window.setTimeout(function() {jQuery("#PagePath").css('color',"white");},100);
      ConfigData.isPresent = true;
      if (ConfigData.showMessage) { this.hideMessage(); }
      showDutyCycleHmIP(); // The DeviceList is now available for displaying the duty cycle and carrier sense of the HAP's
      showCarrierSense();
      if (ConfigData.callback) { ConfigData.callback(); }
    });
  },

  destroy: function()
  {
    this.isPresent = false;
  }

});

ConfigDataLoader = Class.create({

  initialize: function(callback)
  {
    var that = this;

    this.TASKLIST = [
      {name: "WEBUILOADER_DEVICES"  , action: function() { DeviceList.reload(that); }},
      {name: "WEBUILOADER_ROOMS"    , action: function() { RoomList.reload(that); }},
      {name: "WEBUILOADER_FUNCTIONS", action: function() { SubsectionList.reload(that); }}
    ];
    this.m_currentTask = -1;
    this.m_callback = callback;
    this.ready();
  },

  showMessage: function() {
    var screenWidth = WebUI.getWidth();
       var screenHeight = WebUI.getHeight();
       var frameWidth = ConfigDataLoader.CONTENT_WIDTH;
       var frameHeight = ConfigDataLoader.CONTENT_HEIGHT;
       var frameX = parseInt((screenWidth - frameWidth ) / 2);
       var frameY = parseInt((screenHeight - frameHeight) / 2);

       this.m_layer = document.createElement("div");
       this.m_layer.className = "DialogLayer";

       this.m_frame = new UI.Frame()
         .setTitle(translateKey('infoLoadConfigData'))
         .setContentSize(frameWidth, frameHeight)
         .setPosition(frameX, frameY)
         .add(new UI.Text()
           .setPosition(10, 10)
           .setWidth(frameWidth - 10)
           //.setHtml(ConfigDataLoader.CONTENT)
           .setHtml("<img src='/ise/img/ajaxload_white.gif' style='float:left;margin-right:10px' />" + translateKey('infoLoadConfigDataPlsWait'))
         );

    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
  },

  hideMessage: function() {
    this.m_frame.dispose();
    Layer.remove(this.m_layer);
  },

  ready: function()
  {
    this.m_currentTask++;
    if (this.m_currentTask < this.TASKLIST.length)
    {
      this.TASKLIST[this.m_currentTask].action();
    }
    else
    {
      if (this.m_callback) { this.m_callback(); }
    }
  },
  
  reportLoadingState: function(state)
  {
    var width = parseInt((this.m_currentTask + state) * 100);
    //this.m_frame.setTitle(ConfigDataLoader.TITLE + " (" + width + "%)");
    this.m_frame.setTitle(translateKey('infoLoadConfigData') + " (" + width + "%)");
  }
  
});

ConfigDataLoader.CONTENT_WIDTH = 320;
ConfigDataLoader.CONTENT_HEIGHT = 60;
ConfigDataLoader.TITLE = translateKey('infoLoadConfigData');
ConfigDataLoader.CONTENT = "<img src='/ise/img/ajaxload_white.gif' style='float:left;margin-right:10px' />" + translateKey('infoLoadConfigDataPlsWait');
