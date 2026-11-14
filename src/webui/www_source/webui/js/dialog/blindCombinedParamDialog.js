setShutterVirtualReceiverInitValue = function(elmID, chnAddress) {
  var paramElm = jQuery("#"+elmID);
  var val = paramElm.val();
  var ch = DeviceList.getChannelByAddress(chnAddress),
    virtChannelType = (typeof ch.virtChannelType != "undefined") ? ch.virtChannelType : "";

  if (virtChannelType == "SHUTTER_VIRTUAL_RECEIVER") {
    paramElm.val(val.split(",")[0]);
  }
};

BlindCombinedParamDialog = Class.create({
 
  initialize: function(title, content, value, virtChannelType, callback, contentType)
  {
    var _this_ = this;

    this.RESULT_NO = 0;
    this.RESULT_YES = 1;
    this.initValue = value;
    this.virtChannelType = virtChannelType;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer";

    this.idSHUTTER = "SHUTTER_VIRTUAL_RECEIVER";

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

    this.elmLevel = jQuery("#tdCombinedParam_Level");
    this.elmLevel2 = jQuery("#tdCombinedParam_Level2"); // not available when mode SHUTTER_VIRTUAL_RECEIVER activated

    this.initDialog();

  },

  initDialog: function() {
    var arElmValues = this.initValue.split(","),
      valueL = arElmValues[0].split("=")[1],
      valueL2 = (this.virtChannelType != this.idSHUTTER) ? arElmValues[1].split("=")[1] : "";

    this.elmLevel.val(valueL);

    if (this.virtChannelType != this.idSHUTTER) {
      this.elmLevel2.val(valueL2);
    }
  },

  getConfigString: function() {
    var valL = this.elmLevel.val(),
      result;

    if (this.virtChannelType != this.idSHUTTER) {
      var valL2 = this.elmLevel2.val();
      result = "L="+valL+",L2="+valL2;
    } else {
      // See SPHM-1302 result = "L="+valL;

      // New with SPHM-1302
      result = "L="+valL+",L2="+valL;
    }

    Layer.remove(this.m_layer);

    return result;
  },

  close: function(result)
  {
    if (this.m_callback) { this.m_callback(result); }
    Layer.remove(this.m_layer);
  },
  
  yes: function()
  {
    this.close(YesNoDialog.RESULT_YES);
  },
  
  no: function()
  {
    this.close(YesNoDialog.RESULT_NO);
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
    var yesNoDlgElm = jQuery(".YesNoDialog"),
      yesNoDlgFooterElm = jQuery(".YesNoDialogFooter"),
      yesNoDlgTitleElm = jQuery(".YesNoDialogTitle"),
      yesNoDlgContentWrapperElm = jQuery(".YesNoDialogContentWrapper"),
      yesNoDlgYesBtnElm = jQuery(".YesNoDialog_yesButton");
    
    var defaultWith = 600,
      offsetWidth = 4,
      offsetPosYesButton = 109,
      offsetDialogHeight = 78,
      offsetDialogFooterHeight = 26;

    var width = dlgWidth - offsetWidth,
      yesButtonPos = dlgWidth - offsetPosYesButton,
      position = yesNoDlgElm.position();

    // dlgWidth = (defaultWith < dlgWidth) ? defaultWith : dlgWidth;

    yesNoDlgElm.width(dlgWidth).css({left: position.left + ((defaultWith - dlgWidth) / 2)});
    yesNoDlgTitleElm.width(width);
    yesNoDlgContentWrapperElm.width(width);
    yesNoDlgFooterElm.width(width);
    yesNoDlgYesBtnElm.css("left", yesButtonPos);

    //Dialoghöhe an Content anpassen.
    yesNoDlgElm.css("height", yesNoDlgContentWrapperElm.height() + offsetDialogHeight);
    yesNoDlgFooterElm.css("top", yesNoDlgContentWrapperElm.height() + offsetDialogFooterHeight);
  }
  
});
