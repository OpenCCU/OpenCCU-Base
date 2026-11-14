/**
 * vir_lg_whitecontrollerdialog.js
 **/

/**
 * Dialogbox mit den Schaltflächen "Ja" und "Neine"
 * Normalerweise wird als content Text übergeben,
 * wenn contentType 'html' gesetzt ist, kann auch HTML übergeben werden.
 * Die Höhe des Dialoges sollte sich dynamisch der Contentgröße anpassen.
 **/
VIR_LG_WHITEControllerDialog = Class.create({

  initialize: function (title, content, values, callback, contentType) {
    var _this_ = this;

    this.sliderInfoElm;
    this.sliderElm;
    this.curValue = values.curVal;
    this.minValue = values.minVal;
    this.maxValue = values.maxVal;
    this.step = 50;

    this.whiteLevel = this.curValue;


    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer";

    var dialog = document.createElement("div");
    dialog.className = "YesNoDialog";

    var titleElement = document.createElement("div");
    titleElement.className = "YesNoDialogTitle";
    titleElement.appendChild(document.createTextNode(title));
    titleElement.onmousedown = function (event) {
      new Drag(this.parentNode, event);
    };
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
    footer.className = "YesNoDialogFooter";

    var yesButton = document.createElement("div");
    yesButton.className = "YesNoDialog_yesButton borderRadius5px colorGradient50px";
    yesButton.appendChild(document.createTextNode(translateKey('btnOk')));
    yesButton.onclick = function () {
      _this_.yes();
    };
    footer.appendChild(yesButton);

    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('dialogBack')));
    noButton.onclick = function () {
      _this_.no();
    };
    footer.appendChild(noButton);

    dialog.appendChild(footer);

    this.m_layer.appendChild(dialog);

    Layer.add(this.m_layer);

    this.__activateSubDialog();

    //AG sorgt dafür, daß die Dialoghöhe sich dynamisch dem Content anpasst.
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + 78);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + 26);

    // this.__bindEvents();
    this.sliderInfoElm.prop("disabled", true);
    translatePage('#WhiteLevelController');

  },

  __bindEvents: function() {
    var self = this;
      this.sliderInfoElm.blur(function () {
        self.whiteLevel = jQuery(this).val();
        if (isNaN(self.whiteLevel)){
          self.whiteLevel = self.minValue;
        } else if (parseInt(self.whiteLevel) < self.minValue) {
          self.whiteLevel = self.minValue;
        } else if (parseInt(self.whiteLevel) > self.maxValue) {
          self.whiteLevel = self.maxValue;
        }
        jQuery(this).val(parseInt(self.whiteLevel));
        self.sliderElm.slider("value", self.whiteLevel);
      });
  },

  // This creates the content of the dialog.
  __activateSubDialog: function () {
      this.__initSliderElm();
      window.setTimeout(function() {jQuery("#whiteLevelElm").show();}, 500);
    },

  __initSliderElm: function() {
    var self = this;

    this.sliderElm = jQuery("#hookSlider");

    this.sliderInfoElm = jQuery("#infoSliderPos");
    this.sliderInfoElm.val(this.whiteLevel);

    var self = this;
    this.sliderElm.slider({
      animate: "fast",
      value: this.whiteLevel,
      min: this.minValue,
      max: this.maxValue,
      step: this.step,
      orientation: "horizontal",
      slide: function (event, ui) {},
      stop: function( event, ui ) {}
    });
    this.sliderElm.on("slide", function (event, ui) {
      self.__onSliderChange(ui.value);
    });

    this.sliderElm.on("slidestop", function(event, ui){
      self.__onSliderStop(ui.value);
    });
  },

  // PUBLIC
  getValue: function () {
    return parseFloat(this.whiteLevel);
  },


  __onSliderChange: function (val) {
    this.sliderPos = val;
    this.sliderInfoElm.val(this.sliderPos);
    this.whiteLevel = this.sliderPos;
  },

  __onSliderStop: function(val) {
    this.whiteLevel = this.sliderPos;
  },

  close: function (result) {
    Layer.remove(this.m_layer);
    if (this.m_callback) {
      this.m_callback(result);
    }
  },

  yes: function () {
    this.close(YesNoDialog.RESULT_YES);
  },

  no: function () {
    this.close(YesNoDialog.RESULT_NO);
  }

});

VIR_LG_WHITEControllerDialog.RESULT_NO = 0;
VIR_LG_WHITEControllerDialog.RESULT_YES = 1;
