WiredDisplaySystemKey = Class.create({
 
  initialize: function(title, content, deviceType, chnAddress, value, callback, contentType)
  {

    var _this_ = this;

    this.iface = "HmIP-RF";

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer"; 

    this.RESULT_NO = 0;
    this.RESULT_YES = 1;

    this.deviceType = deviceType;
    this.chnAddress = chnAddress;
    this.initValue = value;

    this.wgs = (this.deviceType.includes("HmIP-WGS")) ? true : false;

    this.hasDisplay = false;
    this.checkIfDisplay();

    var dialog = document.createElement("div");
    dialog.className = "YesNoDialog";
    
    var titleElement = document.createElement("div");
    titleElement.className = "YesNoDialogTitle";
    //titleElement.appendChild(document.createTextNode(title));
    titleElement.appendChild(document.createTextNode(deviceType + " - " + chnAddress));
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
    this.fetchDialogElements();
    this.initDialog();
  },

  fetchDialogElements: function() {
    this.displayElems = jQuery("[name='display']");
    this.displayChkBox = jQuery("#display");
    this.sysKeyChkBox = jQuery("#sysKey");
    this.durationSelect = jQuery("#duration");
  },

  getConfigString: function(type) {
   /*
    See SPHM-769
    IDENTIFICATION_MODE_LCD_BACKLIGHT -> IMLB
    IDENTIFICATION_MODE_KEY_VISUAL -> IMKV
    IDENTIFY_TARGET_LEVEL -> IMTL  --> can only be 0 (Off) or > 0 (On)
    IDENTIFY_DURATION -> IMDU
    */

    var result,
      onDisplay = this.displayChkBox.prop("checked"),
      onSysKey = this.sysKeyChkBox.prop("checked"),
      brightness = (onDisplay || onSysKey) ? 1 : 0,
      durationSelect = this.durationSelect.val();
    if (this.hasDisplay) {
      result = "IMLB=" + onDisplay + ",IMKV=" + onSysKey + ",IMTL=" + brightness + ",IMDU=" + durationSelect;
    } else {
      result = "IMKV=" + onSysKey + ",IMTL=" + brightness + ",IMDU=" + durationSelect;
    }
    return result;
  },

  checkIfDisplay: function() {
    var self = this;
    var devDescr =  homematic("Interface.getParamsetDescription", {"interface": this.iface, "address": this.chnAddress, "paramsetKey": "VALUES"});
    jQuery.each(devDescr, function(index, val) {
      if (val.NAME == "IDENTIFICATION_MODE_LCD_BACKLIGHT") {
        self.hasDisplay = true;
        return; // leave each loop
      }
    });
  },

  initDialog: function() {
    var self = this,
      arElmValues = this.initValue.split(","),
      valBacklight, valKeyVisual, valTargetLevel, valDuration;

    if (this.wgs) {
      jQuery("#sysKey").parent().parent().hide();
    }

    if (this.hasDisplay) {
      this.displayElems.show();

      valBacklight = (arElmValues[0].split("=")[1] == "true") ? true : false;
      valKeyVisual = (arElmValues[1].split("=")[1] == "true") ? true : false;
      valDuration = arElmValues[3].split("=")[1];

      this.displayChkBox.prop("checked", valBacklight);
      this.sysKeyChkBox.prop("checked", valKeyVisual);
      this.durationSelect.val(valDuration);
    } else {
      valKeyVisual = (arElmValues[0].split("=")[1] == "true") ? true : false;
      valDuration = arElmValues[arElmValues.length - 1].split("=")[1]; // the last entry of the string should be IMDU = IDENTIFY_DURATION

      this.sysKeyChkBox.prop("checked", valKeyVisual);
      this.durationSelect.val(valDuration);
    }
  },
  
  close: function(result)
  {
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
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
  }
  
});
