
RenameDeviceDialog = Class.create({
 
  initialize: function(title, content, devId, allChannels, callback, contentType)
  {
    var _this_ = this;
    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer"; 

    var dialog = document.createElement("div");
    dialog.className = "YesNoDialog";
    
    var titleElement = document.createElement("div");
    titleElement.className = "YesNoDialogTitle";
    titleElement.appendChild(document.createTextNode(title));
    titleElement.onmousedown = function(event) { new Drag(this.parentNode, event); };
    dialog.appendChild(titleElement);
    
    var contentWrapper = document.createElement("div");
    contentWrapper.className = "YesNoDialogContentWrapper";
    
    var contentElement = document.createElement("div");
    contentElement.className = "YesNoDialogContent";

    if (this.m_contentType == "html") {
      contentElement.innerHTML = content;
    } else {
      contentElement.appendChild(document.createTextNode(content));
    }

    contentWrapper.appendChild(contentElement);
    
    dialog.appendChild(contentWrapper);

    var footer = document.createElement("div");
    footer.className= "YesNoDialogFooter";
    
    var yesButton = document.createElement("div");
    yesButton.className = "YesNoDialog_yesButton borderRadius5px colorGradient50px";
    yesButton.appendChild(document.createTextNode(translateKey('dialogYes')));
    yesButton.onclick = function() { _this_.yes(); };
    yesButton.id="btnYes";
    footer.appendChild(yesButton);
    
    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('dialogNo')));
    noButton.onclick = function() { _this_.no(); };
    noButton.id = "btnNo";
    footer.appendChild(noButton);
    
    dialog.appendChild(footer);
    
    this.m_layer.appendChild(dialog);
    Layer.add(this.m_layer);

    translatePage(".YesNoDialog");

    this.setHeight();

    this.renameAllChannels = (allChannels == "true" || allChannels == true) ? true : false;
    this.device = homematic("Device.get", {id: devId}); // selected device
    this.newDeviceName = this.device.name;

    this.arChnObject = [];

    this.elmBasic = jQuery("#deviceBasisName");
    this.elmSN = jQuery("#partSN");
    //this.elmChannel = jQuery("#partChannel");
    this.tblChannelList = jQuery("#tblChannelList");
    this.elmChannelNames = jQuery("#channelNames");

    this.basicName = "";
    this.tailSN = "";
    this.virtChnGroup = 0;
    this.virtChnGroupLock = false;

    this.generateDeviceName();
    if (this.renameAllChannels) {
      this.showChannelList();
    }
  },

  generateDeviceName: function() {
    this.basicName = (this.device.name.indexOf(":") == -1 ) ? this.device.name.split(" ")[0] : this.device.name.split(":")[0];
    this.tailSN = this.device.address.slice(-4);

    this.elmBasic.val(this.basicName);
    this.elmSN.val(this.tailSN);


  },

  showChannelList: function() {
    var self = this, chnList = "";
    this.generateChannelNames();
    jQuery.each(this.arChnObject, function(index, chnObj) {
      if (chnObj.chnType != "ALARM_COND_SWITCH_TRANSMITTER") {
        chnList += "<div>Kanal: " + (index + 1) + " = <span name='previewChannelName'>" + self.convertToValidBasicName(chnObj.name) + "</span></div>";
      }
    });

    this.elmChannelNames.html(chnList);
    this.tblChannelList.show();
  },

  generateChannelNames: function() {
    var self= this,
      virtChnCounter = 0;
      this.arChnObject = [];
      this.virtChnGroup = 0;

    jQuery.each(this.device.channels, function(index, channel) {

      if (channel.channelType != "MAINTENANCE") {

        if (channel.channelType.indexOf("_VIRTUAL_") != -1)  {
          virtChnCounter++;
        } else {
          virtChnCounter = 0;
        }
        self.arChnObject.push({id: channel.id, name: self.getNewChannelName(channel.index, channel.channelType, virtChnCounter), chnType: channel.channelType});
       }
    });
  },

  getNewChannelName: function(chnIndex, chnType, virtChnCounter) {

    chnType  =  (chnType == "ACCESSPOINT_GENERIC_RECEIVER") ? chnType + "_" + chnIndex : chnType;

    var result = "",
      arExt = [translateKey("chType_" + chnType), "A","B","C","D","E"]; // currently there are only devices with 3 virtuals channels in use (A, B, C)

    // No virtual channel
    if (virtChnCounter == 0) {
      result = this.convertToValidBasicName(this.elmBasic.val()) + ":" + this.elmSN.val() + ":" + chnIndex + ":" + arExt[virtChnCounter];
      this.virtChnGroupLock = false;
    } else {
      if (! this.virtChnGroupLock) {
        this.virtChnGroupLock = true;
        this.virtChnGroup++;
      }
      result = this.convertToValidBasicName(this.elmBasic.val()) + ":" + this.elmSN.val() + ":"+ chnIndex + "-CH" + this.virtChnGroup + "-" + arExt[virtChnCounter];
    }
    return result;
  },

  convertToValidBasicName: function(value) {
    value = value.substring(0,99);
    value = value.replace(/[!\"§$%&\/=?\´\´#\'^°;,~]/g,"");
    value = value.replace(/<[^>]*>/g, " "); // replace html code with a space
    //value = value.replace(/(<\/?(?:br)[^>]*>)|<[^>]+>/ig, '$1'); // Remove html code except <br/>
    value = value.trim(); // Remove whitespace from the start and the end of the value

    return value;
  },

  close: function(result)
  {
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
  },
  
  yes: function()
  {
    this.newDeviceName = this.elmBasic.val() + ":" + this.elmSN.val();

    if (this.renameAllChannels) {
      this.generateChannelNames();
    }

    this.close(RenameDeviceDialog.RESULT_YES);
  },
  
  no: function()
  {
    this.close(RenameDeviceDialog.RESULT_NO);
  },

  btnTextYes: function(btnTxt) {
    jQuery(".YesNoDialog_yesButton").text(btnTxt);
  },

  btnYesHide: function() {
    jQuery("#btnYes").addClass("hidden");
  },

  btnYesShow: function() {
    jQuery("#btnYes").removeClass("hidden");
  },

  btnTextNo: function(btnTxt) {
    jQuery(".YesNoDialog_noButton").text(btnTxt);
  },

  btnNoHide: function() {
    jQuery("#btnNo").addClass("hidden");
  },

  btnNoShow: function() {
    jQuery("#btnNo").removeClass("hidden");
  },

  setHeight: function() {
    var heightContentWrapper = jQuery(".YesNoDialogContentWrapper").height(),
      yesNoElm = jQuery(".YesNoDialog"),
      footerElm = jQuery(".YesNoDialogFooter");

    yesNoElm.css("height", heightContentWrapper + 78);
    footerElm.css("top", heightContentWrapper + 26);
    yesNoElm.css("top", (window.innerHeight / 2) - (yesNoElm.height() / 2));
  },

  resetHeight: function() {
    this.setHeight();
  },

  setWidth: function(dlgWidth) {
    var defaultWith = 600,
      offsetWidth = 4,
      offsetPosYesButton = 109,
      offsetDialogHeight = 78,
      offsetDialogFooterHeight = 26;

    var width = dlgWidth - offsetWidth,
      yesButtonPos = dlgWidth - offsetPosYesButton,
      position = jQuery(".YesNoDialog").position();

    // dlgWidth = (defaultWith < dlgWidth) ? defaultWith : dlgWidth;

    jQuery(".YesNoDialog").width(dlgWidth).css({left: position.left + ((defaultWith - dlgWidth) / 2)});
    jQuery(".YesNoDialogTitle").width(width);
    jQuery(".YesNoDialogContentWrapper").width(width);
    jQuery(".YesNoDialogFooter").width(width);
    jQuery(".YesNoDialog_yesButton").css("left", yesButtonPos);

    //Dialoghöhe an Content anpassen.
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + offsetDialogHeight);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + offsetDialogFooterHeight);
  },

  getNewDeviceName: function() {
    return this.convertToValidBasicName(this.newDeviceName);
  },

  getNewChannelNames: function () {
    return this.arChnObject;
  }
  
});

RenameDeviceDialog.RESULT_NO = 0;
RenameDeviceDialog.RESULT_YES = 1;
