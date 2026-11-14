/**
 * statusdisplaydialog.js
 **/

/**
 * When no text is active the header of the table has to be stripped (remove color and icon)
 */
function setStatusDisplayTableHeader() {
  var optionHeaderElems = jQuery("[name='optionHeader']"),
  hideOptionHeader = true,
  row;

  for(var loop = 0; loop <= 5; loop++) {

    row = jQuery("#textSelect_"+loop).val();

    if ((row != undefined) && (row != "-1")) {
     hideOptionHeader = false;
    }
  }
  if (hideOptionHeader) {
    optionHeaderElems.hide();
  } else {
    optionHeaderElems.show();
  }
}

function setIconPreview(oValue) {
  var displayType = statusDisplayDialog.displayType,
  picPath = (displayType == "DIS") ? "/ise/img/icons_hm_dis_wm55/24/" : "/ise/img/icons_hm_dis_ep_wm55/24/";

  oValue.index = oValue.index.toString();
  var previewElm = jQuery("#iconPreview_"+oValue.index);
  previewElm.html("<img src='"+picPath+oValue.value+".png' alt='' style='height:20px; background-color:#f0f0f0;'>");
}

function displayStatusDisplayOptionContainer(lineIndex, mode) {
  var optionContainer = jQuery("[name='optionContainer_"+lineIndex+"']");
  if(mode) {
    optionContainer.show();
  } else {
    optionContainer.hide();
  }
}
/**
 * When a text for a line is chosen the selectboxes color and icon are visible, otherwise they are hidden.
 * @param elm - Text selectbox for the chosen line
 */
function textOnChange(elm) {
  var selBoxIndex = elm.id.split("_")[1],
  selIndex = jQuery(elm).val();
  if (selIndex != -1) {
    var iconSelElm = jQuery("#iconSelect_" + selBoxIndex);
    displayStatusDisplayOptionContainer(selBoxIndex, true);
    setIconPreview({index: selBoxIndex, value: iconSelElm.val()});
  } else {
    displayStatusDisplayOptionContainer(selBoxIndex, false);
  }
  setStatusDisplayTableHeader();
  setFreeTextContainer();
}

function iconOnChange(elm) {
  var selBoxIndex = elm.id.split("_")[1],
  preViewIndex = jQuery(elm).val();
  setIconPreview({index: selBoxIndex, value: preViewIndex});
}

function setFreeTextContainer() {
  var jHeadFreeText = jQuery("#headFreeText"),
  freeTextActive = false;

  // Check each line if the free text mode is active.
  // If yes switch on the appropriate text input field
  // and set the variable freeTextActive to true
  for (var line = 0; line < 6; line++) {
    var jLine = jQuery("#textSelect_" + line),
    jCellFreeText = jQuery("#cellFreeText_"+line);

    if (jLine.val() == "99") {
      jCellFreeText.show();
      jHeadFreeText.show();
      freeTextActive = true;
    } else {
      jCellFreeText.hide();
    }
  }

  // If at least one line has an active free text mode
  // we have to activate the placeholder for the other lines and the header (color and icon are moving up)
  if (freeTextActive) {
    for (var line = 0; line < 6; line++) {
      var jLine = jQuery("#textSelect_" + line),
      jPlaceHolder = jQuery("#placeHolder_"+line);

      if (jLine.val() != "99") {
        jPlaceHolder.show();
      } else {
        jPlaceHolder.hide().css("display","none");
      }
    }
  } else {
    jHeadFreeText.hide().css("display", "none");
    for (var line = 0; line < 6; line++) {
      jQuery("#placeHolder_"+line).hide().css("display", "none");
    }
  }
}

StatusDisplayDialog = Class.create({
 
  initialize: function(title, content, value, callback, contentType) {
    var _this_ = this;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer"; 

    this.arDisplayData = [];
    this.channelValue = value;

    this.startKey = "0x02";
    this.endKey = "0x03";
    this.lf = "0x0A";
    this.textKey = "0x12";
    this.colorKey = "0x11";
    this.iconKey = "0x13";
    this.soundKey ="0x14";
    this.soundQuantityKey = "0x1C";
    this.soundTimeLagKey = "0x1D";
    this.flashKey = "0x16";
    this.configString = "not initialized";
    this.displayType = "DIS";

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
    yesButton.appendChild(document.createTextNode(translateKey('btnOk')));
    yesButton.onclick = function() { _this_.yes(); };
    footer.appendChild(yesButton);
    
    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('btnCancel')));
    noButton.onclick = function() { _this_.no(); };
    footer.appendChild(noButton);
    
    dialog.appendChild(footer);
    
    this.m_layer.appendChild(dialog);
    
    Layer.add(this.m_layer);

    this.initEPaper();
    this._addElements();
    this._initAllValues();

    setStatusDisplayTableHeader();


    // Remark 1: This doesn´t work satisfying -- see Remark 2
    // This should display an icon within the options of the icon selector
    //jQuery("[name='statusDialogIconOptions']").msDropDown({childWidth:"200px"});

    // This adapts the height of the dialog to the content
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + 78);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + 26);
  },

  initEPaper: function () {},

  // This is for testing only
  _getAllTextPresets: function() {
    var tmp = homematic("Interface.getMasterValue", {"interface": "BidCos-RF", "address": "MYS0000666:1", "valueKey": "TEXTLINE_1"});
    conInfo(tmp);
  },

  // This creates the content of the dialog.
  _addElements: function() {
    var dialogContentElem = jQuery("#statusDisplayDialog");
    var textOptions = this._getTextOptions();
    var colorOptions = this._getColorOptions();
    var iconOptions = this._getIconOptions();
    var freeTextValue = translateKey("statusDisplayOptionFreeText");

    dialogContentElem.append(function(index,html){
      var content =  "<tr><th>&nbsp;</th><th>Text</th> <th id='headFreeText' class='hidden'></th><th name='optionHeader'>Color</th><th name='optionHeader'>Icon</th></tr>";
      for (var loop = 0; loop <= 5; loop++) {
        content +=
          "<tr>" +
            "<td>"+translateKey("statusDisplayLine")+ " "+(loop + 1)+": </td>"+
            "<td><select id='textSelect_"+loop+"' onchange='textOnChange(this)'>"+textOptions+"</select></td>" +
            //"<td id='cellFreeText_"+loop+"' class='hidden'><input id='freeText_"+loop+"' type='text' onchange='encodeStringStatusDisplay(this);' value='"+freeTextValue+"' maxlength='12' size='15' style='text-align:center'></td>" +
            "<td id='cellFreeText_"+loop+"' class='hidden'><input id='freeText_"+loop+"' type='text' value='"+freeTextValue+"' maxlength='12' size='15' style='text-align:center'></td>" +
            "<td id='placeHolder_"+loop+"' class='hidden'></td>" +
            "<td name='optionContainer_"+loop+"' class='hidden'><select id='colorSelect_"+loop+"'>"+colorOptions+"</select></td>" +
            "<td name='optionContainer_"+loop+"' class='hidden'><select id='iconSelect_"+loop+"' onchange='iconOnChange(this)'>"+iconOptions+"</select></td>" +
            "<td name='optionContainer_"+loop+"' class='hidden' id='iconPreview_"+loop+"'></td>"+
          "</tr>";
      }
      return content;
    });
  },

  // Creates the options for the text selector
  _getTextOptions: function() {
    var options = "";
    options += "<option value='-1'>"+translateKey("stringTableNotUsed")+"</option>";
    for (var loop = 0; loop <= 19; loop++) {
      options += "<option value='"+loop+"'>"+ translateKey("optionStatusDisplayText")+ " " +(loop + 1)+"</option>";
    }
    options += "<option value='99'>"+translateKey("optionStatusDisplayFreeText")+"</option>";
    return options;
  },

  // Creates the options for the color selector
  _getColorOptions: function() {
    var arColors =[
      translateKey("optionStatusDisplayWhite"),
      translateKey("optionStatusDisplayRed"),
      translateKey("optionStatusDisplayOrange"),
      translateKey("optionStatusDisplayYellow"),
      translateKey("optionStatusDisplayGreen"),
      translateKey("optionStatusDisplayBlue")
    ];

    var options = "";
    for (var loop = 0; loop < arColors.length; loop++) {
      options += "<option value='"+loop+"'>" + arColors[loop] + "</option>";
    }
    return options;
  },

  // Creates the options for the icon selector
  _getIconOptions: function() {
    var options = "",
    arOptionText = [
      translateKey("iconOff"),
      translateKey("iconOn"),
      translateKey("iconOpen"),
      translateKey("iconClosed"),
      translateKey("iconError"),
      translateKey("iconOK"),
      translateKey("iconInfo"),
      translateKey("iconNewMessage"),
      translateKey("iconServiceMessage"),
      translateKey("iconSignalGreen"),
      translateKey("iconSignalYellow"),
      translateKey("iconSignalRed")
    ];

    options += "<option name='option_NotUsed' value='-1'>" + translateKey("stringTableNotUsed") + "</option>";
    for (var loop = 0; loop < 12; loop++) {
      // Remark 2: This doesn´t work satisfying -- see Remark 1
      //options += "<option name='option'"+loop+" value='"+loop+"' data-image='/ise/img/tr50.gif'>Icon "+loop+"</option>";
      options += "<option name='option_"+loop+"' value='"+loop+"'>" + arOptionText[loop] + "</option>";
    }
    return options;
  },

  // Returns an array of objects with the values of all lines.
  // [Object {text="0x80, color="0x81", icon="0x82},.....]}
  _getAllValues: function() {
    var val = this.channelValue;
    var arValues = val.split(","), //replace(/ /g, "").split(","),
    arLines = []; // contains the lines 0 - 5

    // Is a start key and end key available? Otherwise the string isn´t valid.
    if (arValues[0] == this.startKey && arValues[arValues.length - 1] == this.endKey) {
      var lineIndex = 0,
      textEndIndex,
      txtLengthCounter = 0,
      lineLengthOffset = 0;

      arValues.shift(); // remove the start key 0x02
      arValues.pop(); // remove the end key 0x03

      for (var loopx = 0; loopx < arValues.length; loopx++) {
        var valueSet = {};
        // Is LF?
        if (arValues[loopx] == this.lf) {
          valueSet.text="notUsed";
           arLines[lineIndex] = valueSet;
          lineIndex++;
        }
        if (arValues[loopx] == this.textKey) {
          textEndIndex = jQuery.inArray(this.colorKey, arValues, loopx);
          // Fetch text
          valueSet.text = "";
          for (var loopy = loopx + 1; loopy < textEndIndex; loopy++) {
            valueSet.text += arValues[loopy];
            txtLengthCounter++;
            if (loopy < textEndIndex - 1) {
              valueSet.text += ",";
            }
          }
          // Fetch color
          valueSet.color = arValues[textEndIndex + 1];
          // Fetch icon
          if (arValues[textEndIndex + 2] == this.iconKey) {
            valueSet.icon = arValues[textEndIndex + 3];
            lineLengthOffset = 5;
          } else {
            valueSet.icon = -1;
            lineLengthOffset = 3;
          }
          arLines[lineIndex] = valueSet;
          lineIndex++;

          // Set loopx counter to the end of the text line
          loopx+=txtLengthCounter+lineLengthOffset;
          txtLengthCounter = 0;
        }
      }
    } else {
      conInfo("Value string invalid");
    }
    return arLines;
  },

  // Determines the real value of the HexVal
  // For example: 0x80 = 0, 0x81 = 1 and so on
  _convertHexVal2Val: function(hexVal) {
    if (parseInt(hexVal.split(",")[0],16) < 128 ) {
      // User defined text
      return 99;
    }
    return parseInt(hexVal, 16) - 128;
  },

  // Converts a value to the necessary hex format
  // For example: 0 = 0x80, 1 = 0x81 and so on
  _convertVal2HexVal: function(intVal) {
    return "0x" + (parseInt("0x80",16) + parseInt(intVal)).toString(16);
  },

  // Converts a ASCII string to a string with comma separated hex values
  _convertPlainText2Hex: function(sPlainText) {
    var hex = '';
   	for(var i=0;i<sPlainText.length;i++) {
   		hex += '0x'+sPlainText.charCodeAt(i).toString(16);
      hex += ",";
   	}
   	return hex;
  },

  // Converts a comma separated string of hex values to a ASCII string
   _convertHexString2PlainText: function(sHexString) {
    var arHexValues = sHexString.split(","),
    tmpStr,str = "";
    for(var loop = 0; loop < (arHexValues.length); loop++) {
      tmpStr = arHexValues[loop].slice(2,4);
      str += String.fromCharCode(parseInt(tmpStr, 16));
    }
    return str;
  },

  /**
   * Initializes the dialog
   * @private
   */
  _initAllValues: function() {
    var self = this,
    arAllValues = this._getAllValues();

    conInfo("DIS: All values of the channel: ");
    conInfo(arAllValues);

    jQuery.each(arAllValues, function(index, line) {
      var textElm = jQuery("#textSelect_" + index),
      colorElm = jQuery("#colorSelect_" + index),
      iconElm = jQuery("#iconSelect_" + index),
      freeTextElm = jQuery("#freeText_" + index);

      if (line.text != "notUsed") {
        textElm.val(self._convertHexVal2Val(line.text));
        // User defined text
        if (parseInt(line.text.split(",")[0],16) < 128) {
          freeTextElm.val(decodeStringStatusDisplay(self._convertHexString2PlainText(line.text)));
        }
        colorElm.val(self._convertHexVal2Val(line.color));
        if (line.icon != -1) {
          iconElm.val(self._convertHexVal2Val(line.icon));
          setIconPreview({index: index, value: iconElm.val()});
        } else {
          // Icon not in use
          iconElm.val("-1");
        }
        displayStatusDisplayOptionContainer(index, true);
      } else {
        textElm.val("-1");
      }
    });
    setFreeTextContainer();
  },

  // Not in use
  _initIconPreview: function() {
    var previewElm;
    for(var loop = 0; loop < 9; loop++) {
      previewElm = jQuery("#iconPreview_"+loop);
      previewElm.html("<img src='/ise/img/icons_hm_dis_wm55/24/"+"0.png' alt='' style='height:16px;'>");
    }
  },

  // Not in use
  _initDisplayConfigObject: function() {
    var template = {
      "keyText" : this.keyText,
      "valText" : this.valText,
      "keyColor" : this.keyColor,
      "valColor" : this.valColor,
      "keyIcon" : this.keyIcon,
      "valIcon" : this.valIcon
    };
    for (var loop = 0; loop <= 5; loop++) {
      this.arDisplayData[loop] = template;
    }
  },

  // Creates the string, necessary for the text field within the program
  _createConfigString: function() {
    var textElm, freeTextElm, colorElm, iconElm;
    var result = this.startKey + ","; // Start key

    // Read 6 lines and create string
    for (var loop = 0; loop < 6; loop++) {
      textElm = jQuery("#textSelect_" + loop);
      colorElm = jQuery("#colorSelect_" + loop);
      iconElm = jQuery("#iconSelect_" + loop);
      freeTextElm = jQuery("#freeText_" + loop);

      if (textElm.val() == -1) {
        result+= this.lf + ",";
      } else {
        result += this.textKey + ",";
        if (textElm.val() != "99") {
          // Predefined text bloc
          result += this._convertVal2HexVal(textElm.val()) + ",";
        } else {
          // Free user customized text
          //result += freeTextElm.val() + ",";
          result += this._convertPlainText2Hex(freeTextElm.val());
        }
        result += this.colorKey + ",";
        result += this._convertVal2HexVal(colorElm.val()) + ",";
        if (iconElm.val() != -1) {
          result += this.iconKey + ",";
          result += this._convertVal2HexVal(iconElm.val()) + ",";
        }
        result += this.lf + ",";
      }
    }
    result+= this.endKey; // End key
    this.configString = result;
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
    this.close(YesNoDialog.RESULT_YES);
  },
  
  no: function()
  {
    this.close(YesNoDialog.RESULT_NO);
  }
  
});

StatusDisplayDialogEPaper = Class.create(StatusDisplayDialog, {

  initEPaper: function () {
    conInfo("StatusDisplayDialogEPaper - initEPaper");
    this.displayType = "DIS-EP";
  },

    // This creates the content of the dialog.
  _addElements: function() {
    var dialogContentElem = jQuery("#statusDisplayDialog"),
    textOptions = this._getTextOptions(),
    iconOptions = this._getIconOptions(),
    soundOptions = this._getSoundOptions(),
    soundQuantityOptions = this._getSoundQuantityOptions(),
    soundTimeLagOptions = this._getSoundTimeLagOptions(),
    flashOptions = this._getFlashOptions(),

    freeTextValue = translateKey("statusDisplayOptionFreeText"),
    arrDisabledElements = ["disabled", "","","","disabled","disabled"];

    dialogContentElem.append(function(index,html){
      //var content =  "<tr><th>&nbsp;</th><th>Text</th> <th id='headFreeText' class='hidden'></th><th name='optionHeader'>Color</th><th name='optionHeader'>Icon</th></tr>";
      var content =  "<tr><th>&nbsp;</th><th>Text</th> <th id='headFreeText' class='hidden'></th><th name='optionHeader'>Icon</th></tr>";
      for (var loop = 0; loop <= 4; loop++) {
        content +=
          "<tr>" +
            "<td>"+translateKey("statusDisplayLine")+ " "+(loop + 1)+": </td>"+
            "<td><select id='textSelect_"+loop+"' onchange='textOnChange(this)' "+arrDisabledElements[loop]+">"+textOptions+"</select></td>" +
            "<td id='cellFreeText_"+loop+"' class='hidden'><input id='freeText_"+loop+"' type='text' value='"+freeTextValue+"' maxlength='12' size='15' style='text-align:center'></td>" +
            "<td id='placeHolder_"+loop+"' class='hidden'></td>" +
            "<td name='optionContainer_"+loop+"' class='hidden'><select id='iconSelect_"+loop+"' onchange='iconOnChange(this)'>"+iconOptions+"</select></td>" +
            "<td name='optionContainer_"+loop+"' class='hidden' id='iconPreview_"+loop+"'></td>"+
          "</tr>";
      }

      content += "<tr><td colspan='4'><hr></td></tr>";
      //content += "<tr><td>"+translateKey('lblAcusticalSignal')+":</td><td><select id='soundSelectBox'>"+soundOptions+"</select></td></tr>";
      //content += "<tr><td>"+translateKey('lblOpticalSignal')+":</td><td><select id='flashSelectBox'>"+flashOptions+"</select></td></tr>";

      content += "<tr><th align='center'>"+translateKey('lblAcusticalSignal')+"</th><th>"+translateKey('lblQuantity')+"</th><th>"+translateKey('lblTimeLag')+"</th><th></th></tr>";
      content +=
        "<tr>" +
          "<td style='text-align:center;'><select id='soundSelectBox'>"+soundOptions+"</select></td>" +
          "<td style='text-align:center;'><select id='soundQuantitySelectBox'>"+soundQuantityOptions+"</select></td>" +
          "<td style='text-align:center;'><select id='soundTimeLagSelectBox'>"+soundTimeLagOptions+"</select></td>" +
        "</tr>";

      content += "<tr><td height='15px;'></td></tr>";

      content += "<tr><th align='center'>"+translateKey('lblOpticalSignal')+"</th></tr>";
      content +=
        "<tr>" +
          "<td style='text-align:center;'><select id='flashSelectBox'>"+flashOptions+"</select></td>" +
        "</tr>" ;

      content += "<tr><td height='15px;'></td></tr>";

      return content;
    });
  },

    // Creates the options for the text selector
  _getTextOptions: function() {
    var options = "";
    options += "<option value='-1'>"+translateKey("stringTableNotUsed")+"</option>";
    for (var loop = 0; loop <= 9; loop++) {
      options += "<option value='"+loop+"'>"+ translateKey("statusDisplayOptionText")+ " " +(loop + 1)+"</option>";
    }
    options += "<option value='99'>"+translateKey("statusDisplayOptionFreeText")+"</option>";
    return options;
  },

    // Creates the options for the icon selector
  _getIconOptions: function() {
    var options = "",
    arOptionText = [
      translateKey("iconOff"),
      translateKey("iconOn"),
      translateKey("iconOpen"),
      translateKey("iconClosed"),
      translateKey("iconError"),
      translateKey("iconOK"),
      translateKey("iconInfo"),
      translateKey("iconNewMessage"),
      translateKey("iconServiceMessage")
    ];

    options += "<option name='option_NotUsed' value='-1'>" + translateKey("stringTableNotUsed") + "</option>";
    for (var loop = 0; loop < 9; loop++) {
      options += "<option name='option_"+loop+"' value='"+loop+"'>" + arOptionText[loop] + "</option>";
    }
    return options;
  },

  _getSoundOptions:function() {
    var options = "",
    arSound = ["",
      translateKey("optionLong") + " " + translateKey("optionLong"),
      translateKey("optionLong") + " " + translateKey("optionShort"),
      translateKey("optionLong") + " " + translateKey("optionShort") + " " + translateKey("optionShort"),
      translateKey("optionShort"),
      translateKey("optionShort") + " " + translateKey("optionShort")];

    options += "<option value='0xC0'>"+translateKey("stringTableNotUsed")+"</option>";
    options += "<option value='0xC6'>"+translateKey("optionLong")+"</option>"; // This is a belated value which should appear as first option after 'not used'
    for (var loop = 1; loop <= 5; loop++) {
      options += "<option value='0xC"+loop+"'>"+ arSound[loop] +"</option>";
    }
    return options;
  },

  _getSoundQuantityOptions: function() {
    var options = "";
    for (var loop = 0; loop <= 14; loop++) {
      options += "<option value='0xD"+loop.toString(16).toUpperCase()+"'>"+ (loop + 1) +"</option>";
    }
    options += "<option value='0xDF'>"+ translateKey("optionInfinite") +"</option>";
    return options;
  },

  _getSoundTimeLagOptions: function() {
    var options = "";
    for (var loop = 0; loop <= 15; loop++) {
      options += "<option value='0xE"+loop.toString(16).toUpperCase()+"'>"+ (parseInt(loop) + 1) * 10 + " " + translateKey("optionUnitS") +"</option>";
    }
    return options;
  },

  _getFlashOptions:function() {
    var options = "",
    arFlash = [translateKey("stringTableNotUsed"), translateKey("redFlash"), translateKey("greenFlash"), translateKey("orangeFlash")];
    for (var loop = 0; loop <= 3; loop++) {
      options += "<option value='0xF"+loop+"'>"+ arFlash[loop] +"</option>";
    }
    return options;
  },

   // Creates the string, necessary for the text field within the program
  _createConfigString: function() {
    var textElm, freeTextElm,iconElm,
    result = this.startKey + ","; // Start key

    // Read 5 lines and create string
    for (var loop = 0; loop < 5; loop++) {
      textElm = jQuery("#textSelect_" + loop);
      iconElm = jQuery("#iconSelect_" + loop);
      freeTextElm = jQuery("#freeText_" + loop);

      if (textElm.val() == -1) {
        result+= this.lf + ",";
      } else {
        result += this.textKey + ",";
        if (textElm.val() != "99") {
          // Predefined text bloc
          result += this._convertVal2HexVal(textElm.val()) + ",";
        } else {
          // Free user customized text
          //result += freeTextElm.val() + ",";
          result += this._convertPlainText2Hex(freeTextElm.val());
        }
        if (iconElm.val() != -1) {
          result += this.iconKey + ",";
          result += this._convertVal2HexVal(iconElm.val()) + ",";
        }
        result += this.lf + ",";
      }
    }
    result += this.soundKey + "," + jQuery("#soundSelectBox").val() + ",";
    result += this.soundQuantityKey + "," + jQuery("#soundQuantitySelectBox").val() + ",";
    result += this.soundTimeLagKey + "," + jQuery("#soundTimeLagSelectBox").val() + ",";
    result += this.flashKey + "," + jQuery("#flashSelectBox").val() + ",";
    result+= this.endKey; // End key
    this.configString = result;
  },
  /**
   * Initializes the dialog
   * @private
   */
  _initAllValues: function() {
    var self = this,
    arValues = this.channelValue.split(","),
    sizeChannelValue = arValues.length,
    arAllValues = this._getAllValues(),
    soundElm = jQuery("#soundSelectBox"),
    soundQuantityElm = jQuery("#soundQuantitySelectBox"),
    soundTimeLagElm = jQuery("#soundTimeLagSelectBox"),
    flashElm = jQuery("#flashSelectBox");


    conInfo("DIS-EP: All values of the channel: ");
    conInfo(arAllValues);

    jQuery.each(arAllValues, function(index, line) {
      var textElm = jQuery("#textSelect_" + index),
      iconElm = jQuery("#iconSelect_" + index),
      freeTextElm = jQuery("#freeText_" + index);

      if (line.text != "notUsed") {
        textElm.val(self._convertHexVal2Val(line.text));
        // User defined text
        if (parseInt(line.text.split(",")[0],16) < 128) {
          freeTextElm.val(decodeStringStatusDisplay(self._convertHexString2PlainText(line.text)));
        }
        if (line.icon != -1) {
          iconElm.val(self._convertHexVal2Val(line.icon));
          setIconPreview({index: index, value: iconElm.val()});
        } else {
          // Icon not in use
          iconElm.val("-1");
        }
        displayStatusDisplayOptionContainer(index, true);
      } else {
        textElm.val("-1");
      }
    });
    soundElm.val(arValues[sizeChannelValue - 8]); // initialize sound select box
    soundQuantityElm.val(arValues[sizeChannelValue - 6]); // initialize sound quantity select box
    soundTimeLagElm.val(arValues[sizeChannelValue - 4]); // initialize sound time lag box
    flashElm.val(arValues[sizeChannelValue - 2]); // initialize flash select box

    setFreeTextContainer();
  },
    // Returns an array of objects with the values of all lines.
  // [Object {text="0x80, color="0x81", icon="0x82},.....]}
  _getAllValues: function() {
    var val = this.channelValue;
    var arValues = val.split(","), //replace(/ /g, "").split(","),
    arLines = []; // contains the lines 1,2,3

    // Is a start key and end key available? Otherwise the string isn´t valid.
    if (arValues[0] == this.startKey && arValues[arValues.length - 1] == this.endKey) {
      var lineIndex = 0,
      textIndex,
      nextTextBlockIndex = 0,
      textOffset = 0;

      arValues.shift(); // remove the start key 0x02
      arValues.pop(); // remove the end key 0x03
      //console.log("arValues: " + arValues);
      for (var loopx = 0; loopx < arValues.length; loopx++) {
        //console.log("current loopx: " + loopx);
        var valueSet = {};
        nextTextBlockIndex = 0;
                // Is LF?
        if (arValues[loopx] == this.lf) {
          valueSet.text="notUsed";
           arLines[lineIndex] = valueSet;
          lineIndex++;
        }

        if (arValues[loopx] == this.textKey) {
          valueSet.text = "";
          // Read till icon or lf and increase loopx by the number of read chars
          textIndex = loopx + 1;
          do {
            valueSet.text += arValues[textIndex];
            if ((arValues[textIndex + 1] != this.iconKey) && (arValues[textIndex + 1] != this.lf)) {
              valueSet.text += ",";
            }

            //console.log("added char: " + arValues[textIndex]);
            textIndex++;
            nextTextBlockIndex++;
          } while ((arValues[textIndex] != this.iconKey) && (arValues[textIndex] != this.lf)) ;

          // Icon hinzufügen, entweder nicht benutzt (-1) oder den entsprechenden Wert
          // Add the icon, either not used (-1) or the correspondent value
          valueSet.icon = (arValues[textIndex] == this.iconKey) ? arValues[textIndex + 1] : -1;
          arLines[lineIndex] = valueSet;
          lineIndex++;
          if (valueSet.icon == -1) {textOffset = 1;} else {textOffset = 3;}
          // Jump to the next text block
          loopx += nextTextBlockIndex + textOffset; // Springe zum nächsten Textblock
          //console.log("new loopx : " + loopx);
        }
      }
    } else {
      conInfo("Value string invalid");
    }
    return arLines;
  }

});

YesNoDialog.RESULT_NO = 0;
YesNoDialog.RESULT_YES = 1;
