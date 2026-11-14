/**
 * JalousieActorConvertHexValDialog.js
 **/
 
/**
 * Dialogbox mit den Schaltflächen "Ja" und "Neine"
 * Normalerweise wird als content Text übergeben,
 * wenn contentType 'html' gesetzt ist, kann auch HTML übergeben werden.
 * Die Höhe des Dialoges sollte sich dynamisch der Contentgröße anpassen.
 **/
JalousieActorConvertHexValDialog = Class.create({
 
  initialize: function(title, content, iseVal, callback, contentType)
  {
    var _this_ = this;

    this.iseVal = iseVal;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer";
    this.configString = "not initialized";

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
    footer.appendChild(yesButton);
    
    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('footerBtnCancel')));
    noButton.onclick = function() { _this_.no(); };
    footer.appendChild(noButton);
    
    dialog.appendChild(footer);
    
    this.m_layer.appendChild(dialog);
    
    Layer.add(this.m_layer);

    //AG sorgt dafür, daß die Dialoghöhe sich dynamisch dem Content anpasst.
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + 78);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + 26);

    translatePage(".YesNoDialog");

    this.blindLevelElm = jQuery("#idBlindLevel");
    this.slatLevelElm = jQuery("#idSlatsLevel");

    this._initDialogElements();

  },

  _initDialogElements: function() {
    var self = this;
    var arHexVal = this.iseVal.split(",");
    this.blindLevelElm.val(parseInt(arHexVal[0]) / 2);
    this.slatLevelElm.val(parseInt(arHexVal[1]) / 2);

    this.blindLevelElm.keyup(function(event) {self._checkVal(event.keyCode, self.blindLevelElm);});
    this.slatLevelElm.keyup(function(event) {self._checkVal(event.keyCode, self.slatLevelElm);});

  },

  _checkVal: function(keyCode, elm) {

    // Decimal point or comma
    if ((keyCode == 110) || (keyCode == 188) || (keyCode == 190) ) {
      return;
    }

    var value = parseFloat(elm.val());

    if ((keyCode != 8) && ((keyCode < 48) || (keyCode > 57))) {
      if (isNaN(value)) {
        elm.val(0);
      } else {
          elm.val(value);
      }
    }

    if (value < 0) {
      elm.val(0);
    } else if (value > 100) {
      elm.val(100);
    }

  },

  _createConfigString: function() {
    var level = parseInt(parseFloat(this.blindLevelElm.val()) * 2),
      levelSlat = parseInt(parseFloat(this.slatLevelElm.val()) * 2);

    level = (isNaN(level)) ? 0: level;
    levelSlat = (isNaN(levelSlat)) ? 0 : levelSlat;

    var levelHex = level.toString(16),
    levelSlatHex = levelSlat.toString(16),
    dpValue;

    dpValue = (levelHex.length < 2) ? "0X0" + levelHex : "0X" + levelHex;
    dpValue += ",";
    dpValue += (levelSlatHex.length < 2) ? "0X0" + levelSlatHex : "0X" + levelSlatHex;
    this.configString = dpValue;
  },

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
    this.close(JalousieActorConvertHexValDialog.RESULT_YES);
  },
  
  no: function()
  {
    this.close(JalousieActorConvertHexValDialog.RESULT_NO);
  }
  
});

JalousieActorConvertHexValDialog.RESULT_NO = 0;
JalousieActorConvertHexValDialog.RESULT_YES = 1;
