
ASIR_SetAlarmDialog = Class.create({
 
  initialize: function(title, content, callback, contentType)
  {
    var _this_ = this;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer"; 
    this.maxDuration = 600;

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

    //AG sorgt dafür, daß die Dialoghöhe sich dynamisch dem Content anpasst.
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + 78);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + 26);
    translatePage(".YesNoDialog");
    this.getElmFromDOM();
    this.bindElements();
  },

  getElmFromDOM: function() {
    this.selectedAcousticSignalElm = jQuery("#AcousticAlarmSelect");
    this.selectedOpticalSignalElm = jQuery("#OpticalAlarmSelect");
    this.selectDurationElm = jQuery("#DurationSelect");
    this.durationPanelElm = jQuery("[name='durationPanel']");
    this.durationUnitElm = jQuery("#DurationUnit");
    this.durationValueElm = jQuery("#DurationValue");
    this.maxDurationValueElm = jQuery("#maxDurationValue");
  },

  bindElements: function() {
    var self=this;
    this.selectDurationElm.change(function() {
      self.setTimePanel();
    });

    this.durationUnitElm.change(function() {
      self.setMaxValue(this.value);
    });

    this.durationValueElm.keyup(function() {
      self.checkIsValid(this.value);
    });

    this.setTimeShortCut();
  },

  setMaxValue: function(val) {
    // val = M(in)or S(ec)
    this.maxDuration = (val == "S") ? 600 : 10;
    this.durationValueElm.val(this.maxDuration);
    this.maxDurationValueElm.text("(0 - " + this.maxDuration + ")");
  },

  checkIsValid: function(val) {
    var correctedVal;
    if ((isNaN(val)) ||(val < 0) || (val > this.maxDuration)) {
      correctedVal = ((isNaN(val)) || (val < 0)) ? 0 : this.maxDuration;
      this.durationValueElm.val(correctedVal);
    }
  },

  setTimePanel: function() {
    if (this.selectDurationElm.val() == "setDuration") {
      //this.durationPanelElm.show();
      this.durationPanelElm.css("visibility","visible");
    } else {
      //this.durationPanelElm.hide();
      this.durationPanelElm.css("visibility","hidden");
      this.setTimeShortCut();
    }
  },

  setTimeShortCut: function() {
    var arSelectedDurationShortcut = this.selectDurationElm.val().split("_");
    var value = arSelectedDurationShortcut[0],
      unit = arSelectedDurationShortcut[1];

    this.durationUnitElm.val(unit);
    this.durationValueElm.val(value);
  },

  close: function(result)
  {
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
  },
  
  yes: function()
  {
    this.close(ASIR_SetAlarmDialog.RESULT_YES);
  },
  
  no: function()
  {
    this.close(ASIR_SetAlarmDialog.RESULT_NO);
  }
  
});

ASIR_SetAlarmDialog.RESULT_NO = 0;
ASIR_SetAlarmDialog.RESULT_YES = 1;
