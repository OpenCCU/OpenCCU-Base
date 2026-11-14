WTHSelectModeDialog = Class.create({
 
  initialize: function(title, content, val, paramName, callback, contentType)
  {
    var _this_ = this;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer"; 

    this.m_selectedMode = val;
    this.m_paramName = paramName;

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
    footer.appendChild(yesButton);
    
    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('dialogBack')));
    noButton.onclick = function() { _this_.no(); };
    footer.appendChild(noButton);
    
    dialog.appendChild(footer);
    
    this.m_layer.appendChild(dialog);
    
    Layer.add(this.m_layer);

    //AG sorgt dafür, daß die Dialoghöhe sich dynamisch dem Content anpasst.
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + 78);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + 26);
    translatePage(".YesNoDialog");
    this.m_initDialog();

  },

  m_initDialog: function() {
    jQuery("#dlgLblParam").text(this.m_paramName);
    this.optionElm = jQuery("#idWTHControlMode");
    this.optionElm.val(this.m_selectedMode);
  },

  getSelectedMode: function() {
    return this.optionElm.val();
  },

  close: function(result)
  {
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
  },
  
  yes: function()
  {
    this.close(WTHSelectModeDialog.RESULT_YES);
  },
  
  no: function()
  {
    this.close(WTHSelectModeDialog.RESULT_NO);
  }
  
});

WTHSelectModeDialog.RESULT_NO = 0;
WTHSelectModeDialog.RESULT_YES = 1;
