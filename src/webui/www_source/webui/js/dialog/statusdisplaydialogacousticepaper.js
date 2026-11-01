var arIconDesc = [],
  resetEPaperDisplay = false;

function getIconDescr(no) {
  if(arIconDesc.length == 0) {
    arIconDesc = [
      translateKey("iconLampOff"),
      translateKey("iconLampOn"),
      translateKey("iconPadlockOpen"),
      translateKey("iconPadlockClosed"),
      translateKey("iconX"),
      translateKey("iconTick"),
      translateKey("iconInfo"),
      translateKey("iconEnvelope"),
      translateKey("iconWrench"),
      translateKey("iconSun"),

      translateKey("iconMoon"),
      translateKey("iconWind"),
      translateKey("iconCloud"),
      translateKey("iconCloudBolt"),
      translateKey("iconCloudLightRain"),
      translateKey("iconCloudMoon"),
      translateKey("iconCloudRain"),
      translateKey("iconCloudSnow"),
      translateKey("iconCloudSun"),
      translateKey("iconCloundSunRain"),

      translateKey("iconSnowFlake"),
      translateKey("iconRainDrop"),
      translateKey("iconFlame"),
      translateKey("iconWindowOpen"),
      translateKey("iconShutter"),
      translateKey("iconEco"),
      translateKey("iconProtectionOff"),
      translateKey("iconProtectionExternal"),
      translateKey("iconProtectionInternal"),
      translateKey("iconBell"),

      translateKey("iconClock")
    ];
  }
  return arIconDesc[no];
};

function setIconPreview2(picNr, row) {
  var picPath = "/ise/img/icons_hmip_wrcd/24/",
  previewElm = jQuery("#iconPreview_" + row);

  if (picNr != 0) {

    previewElm.html("<img src='" + picPath + picNr + ".png' alt='' style='height:20px; background-color:#f0f0f0;'>");
  } else {
    previewElm.html("");
  }
};

function showHideSoundParams(soundNo) {
  var soundParamElms = jQuery("[name='soundParam']");
  if (soundNo != -1) {
    soundParamElms.show();
  } else {
    soundParamElms.hide();
  }
};

function showHideDisplayConfig(elm) {
  var resetDisplay = jQuery(elm).is(":checked");
  if (resetDisplay) {
    jQuery("[name='displayConfig']").attr("style", "visibility:hidden");
    resetEPaperDisplay = true;
  } else {
    jQuery("[name='displayConfig']").attr("style", "visibility:visible");
    resetEPaperDisplay = false;
  }
};

function setTextAlignment(row, alignment) {
  var textElm = jQuery("#text_" + row);
  textElm.attr("style","text-align: " + alignment.toLowerCase());
};

function showDisplayConfigTextHelp() {
  var title = translateKey("dialogHelpAcousticDisplayReceiverTitle"),
    content = translateKey("dialogHelpAcousticDisplayReceiverContent");
  MessageBox.show(title, content,'', 475, 450);
};

function toggleSerialNumberDisplay() {
  var tb = document.getElementById("fwUpdates");
  if (tb) {
    var th = tb.getElementsByTagName("th")[2];
    // an empty value restores the default table-cell/table-column display
    var display = (th.style.display === "none") ? "" : "none";
    th.style.display = display;
    tb.getElementsByTagName("col")[2].style.display = display;
    document.querySelectorAll('#fwUpdates tbody tr td:nth-child(3)').forEach(el=>el.style.display = display)
  }
};

StatusDisplayDialogAcousticEPaper = Class.create({
 
  initialize: function(title, content, value, callback, contentType) {
    var _this_ = this;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer";
    this.RESULT_NO = 0;
    this.RESULT_YES = 1;

    this.arInitConfigString = value.split("},{"); // cfg string when entering the dialog

    resetEPaperDisplay = false;

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
    yesButton.appendChild(document.createTextNode(translateKey('footerBtnOk')));
    yesButton.onclick = function() { _this_.yes(); };
    yesButton.id="btnYes";
    footer.appendChild(yesButton);
    
    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('footerBtnCancel')));
    noButton.onclick = function() { _this_.no(); };
    noButton.id = "btnNo";
    footer.appendChild(noButton);
    
    dialog.appendChild(footer);
    
    this.m_layer.appendChild(dialog);
    Layer.add(this.m_layer);

    translatePage(".YesNoDialog");

    this._addElements();
    if (this.arInitConfigString != "") {
      this._initDialog();
    }

    this.setHeight();
    this.setWidth(700);
  },

  _initDialog: function() {
    var self = this;
    jQuery.each(this.arInitConfigString, function(index, cfg){
      var cfg = cfg.replace(/{/,"").replace(/}/,""),
        arCfgParamString = cfg.split(","),
        arParam = [];

      var curRow;

      jQuery.each(arCfgParamString, function(index, param) {
        var _arParam = param.split("=");
        arParam[_arParam[0]] = _arParam[1];
      });

      if (typeof arParam["DDID"] != "undefined") {
        curRow = arParam["DDID"];

        if (arParam["DDS"] != "") {
          if (arParam["DDS"] == "XXX") {
            jQuery("#resetDisplay").attr("checked", true).change();
            arParam["DDS"] = "";
            // Because the reset command doesn't set DDA and DDTC
            // we have to set DDA and DDTC manually when initializing the dialog within programs.
            arParam["DDA"] = "CENTER";
            arParam["DDBC"] = "WHITE";
            arParam["DDTC"] = "BLACK";
            arParam["DDI"] = "0";
          }
          jQuery("#text_" + curRow).val(self._decodeSpecialChars(arParam["DDS"]));
        }

        jQuery("#align_" + curRow).val(arParam["DDA"]).change();
        jQuery("#bgColor_" + curRow).val(arParam["DDBC"]);
        jQuery("#textColor_" + curRow).val(arParam["DDTC"]);
        jQuery("#icon_" + curRow).val(arParam["DDI"]).change();
      }

      if (typeof arParam["ANS"] != "undefined") {
        jQuery("#soundSelectBox").val(arParam["ANS"]).change();
        jQuery("#soundQuantitySelectBox").val(arParam["R"]);
        jQuery("#soundTimeLagSelectBox").val(arParam["IN"]);
      }

    });
  },

  _addElements: function() {
    var self = this,
    dialogContentElem = jQuery("#statusDisplayDialog");

    dialogContentElem.append(function(index,html){
      var content =  "<table name='displayConfig'>";
        content += "<th>&nbsp;</th>";
        content += "<th>"+translateKey('lblText')+"&nbsp;&nbsp;<img src='/ise/img/help.png' height='16' width='16' onclick='showDisplayConfigTextHelp();'></th>";
        content += "<th>"+translateKey('lblAlign')+"</th>";
        content += "<th>"+translateKey('lblBGColorBR')+"</th>";
        content += "<th>"+translateKey('lblTextColorBR')+"</th>";
        content += "<th>"+translateKey('lblIcon')+"</th>";

      for (var loop = 1; loop <= 5; loop++) {
        content +=
          "<tr>" +
            "<td>"+translateKey('statusDisplayLine')+": "+loop+"</td>" +
            "<td>"+self._getTextElm(loop)+"</td>" +
            "<td>"+self._getAlignElm(loop)+"</td>" +
            "<td>"+self._getBgColorElm(loop)+"</td>" +
            "<td>"+self._getTextColorElm(loop)+"</td>" +
            "<td>"+self._getIconElm(loop)+"</td>" +
            "<td id='iconPreview_"+loop+"'></td>"+
          "</tr>";
      }
      content += "</table>";

      content +=
       "<hr name='displayConfig'>";

      content +=
      "<table name='displayConfig' align='center'>" +
        "<th align='center'>"+translateKey('lblAcusticalSignal')+"</th><th class='hidden' name='soundParam'>"+translateKey('lblRepetition')+"</th><th class='hidden' name='soundParam'>"+translateKey('lblTimeLag')+"</th>";

      content +=
        "<tr>" +
          "<td style='text-align:center;'><select class='centerSelect' id='soundSelectBox' onchange='showHideSoundParams(this.value)'>"+self._getSoundOptions()+"</select></td>" +
          "<td class='hidden' name='soundParam' align='center'><select class='centerSelect' id='soundQuantitySelectBox'>"+self._getSoundQuantityOptions()+"</select></td>" +
          "<td class='hidden' name='soundParam' align='center'><select class='centerSelect' id='soundTimeLagSelectBox'>"+self._getSoundTimeLagOptions()+"</select></td>" +
        "</tr>" +

      "</table>";

      content +=
        "<hr name='displayConfig'>";
      content +=
        "<table align='center'>";
        content +=
          "<tr>" +
            "<td>"+translateKey("resetDevice")+"</td>" +
            "<td><input id='resetDisplay' type='checkbox' onchange='showHideDisplayConfig(this);'></td>" +
          "</tr>" +
      "</table>";

      return content;
    });
  },

  _getTextElm: function(no) {
    return "<input id='text_"+no+"' type='text' maxlength='15' size='15' style='text-align:center;' tabindex='"+no+"'>";
  },

  _getAlignElm: function(no) {
    var html = "<select class='centerSelect' id='align_"+no+"' onchange='setTextAlignment("+no+", this.value)'>";
      html += "<option value='LEFT'>"+translateKey('lblLeft')+"</option>";
      html += "<option value='CENTER' selected>"+translateKey('lblCenter')+"</option>";
      html += "<option value='RIGHT'>"+translateKey('lblRight')+"</option>";
    html += "</select>";
    return html;
  },

  _getBgColorElm: function(no) {
    var html = "<select class='centerSelect' id='bgColor_"+no+"'>";
      html += "<option value='WHITE'>"+translateKey('optionColorWHITE')+"</option>";
      html += "<option value='BLACK'>"+translateKey('optionColorBLACK_A')+"</option>";
      // html += "<option value='RED'>"+translateKey('colorRED')+"</option>";
    html += "</select>";
    return html;
  },

  _getTextColorElm: function(no) {
    var html = "<select class='centerSelect' id='textColor_"+no+"'>";
      html += "<option value='WHITE'>"+translateKey('optionColorWHITE')+"</option>";
      html += "<option value='BLACK' selected>"+translateKey('optionColorBLACK_A')+"</option>";
     // html += "<option value='RED'>"+translateKey('colorRED')+"</option>";
    html += "</select>";
    return html;
  },

  _getIconElm: function(no) {
    var html = "<select class='centerSelect' id='icon_"+no+"' onchange='setIconPreview2(this.value, "+no+");'>";
    html += "<option value='0'>"+translateKey('stringTableNotUsed')+"</option>";
    for (var loop = 0; loop < 31; loop++) {
      html += "<option value='" +(loop + 1)+"'>"+getIconDescr(loop)+"</option>";
    }
    html += "</select>";
    return html;
  },

  _getSoundOptions:function() {
    var options = "",
    arSound = [
      translateKey("lblLowBattery"),
      translateKey("lblDisarmed"),
      translateKey("lblExternallyArmed"),
      translateKey("lblInternallyArmed"),
      translateKey("lblDelayExternalArming"),
      translateKey("lblDelayInternalArming"),
      translateKey("lblEvent"),
      translateKey("lblError")
    ];

    options += "<option value='-1'>"+ translateKey('stringTableNotUsed') +"</option>";

    for (var loop = 0; loop < arSound.length; loop++) {
      options += "<option value='"+loop+"'>"+ arSound[loop] +"</option>";
    }
    return options;
  },

  _getSoundQuantityOptions: function() {
    var options = "";
    options += "<option value='0'>"+translateKey('optionNoRepetition')+"</option>";
    for (var loop = 1; loop <= 14; loop++) {
      options += "<option value='"+loop+"'>"+loop+"</option>";
    }
    options += "<option value='15'>"+translateKey('optionInfinite')+"</option>";
    return options;
  },

  _getSoundTimeLagOptions: function() {
    var options = "";
    for (var loop = 5; loop <= 80; loop+=5) {
      options += "<option value='"+ loop +"'>"+ loop + " " + translateKey("optionUnitS") +"</option>";
    }
    return options;
  },

  _getTextConfigString: function() {
    var result = "";

    var rowCounter = 0,
      text = [],
      align = [],
      bgColor = [],
      textColor = [],
      icon = [];

    var rowNo,
      rowText,
      rowIcon;

    for (var loop = 0; loop < 5; loop++) {
      rowNo = loop + 1;
      rowText = jQuery("#text_" + rowNo).val();
      rowIcon = jQuery("#icon_" + rowNo).val();
      if (rowText != "" || rowIcon != 0) {
        if (rowText != "") {
          text[rowCounter] = this._encodeSpecialChars(rowText);
        } else {
          text[rowCounter] = "";
        }

        icon[rowCounter] = rowIcon;
        align[rowCounter] = jQuery("#align_" + rowNo).val();
        bgColor[rowCounter] = jQuery("#bgColor_" + rowNo).val();
        textColor[rowCounter] = jQuery("#textColor_" + rowNo).val();

        result = result.replace(/,DDC=true/g, '');

        result += "{" +
          "DDBC=" + bgColor[rowCounter] +     // DISPLAY_DATA_BACKGROUND_COLOR
          ",DDTC=" + textColor[rowCounter] +  // DISPLAY_DATA_TEXT_COLOR
          ",DDI=" + icon[rowCounter] +        // DISPLAY_DATA_ICON
          ",DDA=" + align[rowCounter] +       // DISPLAY_DATA_ALIGNMENT
          ",DDS=" + text[rowCounter] +        // DISPLAY_DATA_STRING
          ",DDID=" + rowNo +                  // DISPLAY_DATA_ID
          ",DDC=true"+                        // DISPLAY_DATA_COMMIT
          "},";
        rowCounter++;
      }
    }
    return result;
  },

  _encodeSpecialChars: function(txt) {
    return txt
      .replace(/Ä/g,"[")
      .replace(/Ö/g,"#")
      .replace(/Ü/g,"$")
      .replace(/ä/g,"²")
      .replace(/ö/g,"|")
      .replace(/ü/g,"³")
      .replace(/ß/g,"_")
      .replace(/&/g,"]")
      .replace(/'/g,"µ");
  },

  _decodeSpecialChars: function(txt) {
    return txt
      .replace(/\[/g,"Ä")
      .replace(/#/g,"Ö")
      .replace(/\$/g,"Ü")
      .replace(/²/g,"ä")
      .replace(/\|/g,"ö")
      .replace(/³/g,"ü")
      .replace(/\_/g,"ß")
      .replace(/\]/g,"&")
      .replace(/µ/g,"'");
  },

  _getAcousticConfigString: function() {
    var result = "",
      selectedSound = jQuery("#soundSelectBox").val();

    if (selectedSound != -1) {
      result += "{" +
        "R=" + jQuery("#soundQuantitySelectBox").val() +    // REPETITIONS
        ",IN=" + jQuery("#soundTimeLagSelectBox").val() +   // INTERVAL
        ",ANS=" + selectedSound +                           // ACOUSTIC_NOTIFICATION_SELECTION
        "}";
    }

    return result;
  },

  _createConfigString: function() {
    var textConfigString = "", acousticConfigString = "";
    if (! resetEPaperDisplay) {
       textConfigString = this._getTextConfigString();
       acousticConfigString = this._getAcousticConfigString();
      this.configString = (acousticConfigString == "") ? textConfigString.slice(0, textConfigString.length - 1) : textConfigString + acousticConfigString;
    } else {
      //acousticConfigString = this._getAcousticConfigString();
      //this.configString = (acousticConfigString == "") ? "{DDS=XXX,DDID=1,DDC=true}" : "{DDS=XXX,DDID=1,DDC=true}," + this._getAcousticConfigString() ;
      this.configString = "{DDS=XXX,DDID=1,DDC=true}";
    }
  },

  // PUBLIC
  getConfigString: function() {
    return this.configString;
  },

  close: function(result)
  {
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
  },
  
  yes: function()
  {
    this._createConfigString();
    this.close(this.RESULT_YES);
  },
  
  no: function()
  {
    this.close(this.RESULT_NO);
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
