/**
 * partymodedialog.js
 **/

StartDateEqualsStopDate = function() {
  var startDate = jQuery("#partyStartDate").datepicker('getDate'),
  stopDate = jQuery("#partyEndDate").datepicker('getDate');
  if (startDate && stopDate) {
    return (startDate.getDate() == stopDate.getDate()) ? true : false;
  } else {
    return false;
  }
};

refreshPartyDatePicker = function() {
  conInfo("refresh DatePickerEnd");
  var jDatePickerStart = jQuery("#partyStartDate"),
    jDatePickerEnd = jQuery("#partyEndDate"),
    newDate = jDatePickerStart.datepicker("getDate");

  jDatePickerEnd.datepicker("option", "minDate", new Date(newDate));
  jDatePickerEnd.datepicker("refresh");
  refreshPartyTimePicker();
};

refreshPartyTimePicker= function() {
  allDataSet();
  conInfo("refresh TimePicker");
  if(StartDateEqualsStopDate() && (showEmptyTimeFields != true)) {
    var jTimePickerStart = jQuery("#partyStartTime"),
      jTimePickerEnd = jQuery("#partyEndTime"),
      startHour = jTimePickerStart.timepicker("getHour"),
      startMinute = jTimePickerStart.timepicker("getMinute"),
      endHour = jTimePickerEnd.timepicker("getHour"),
      newEndTime = startHour + 1 + ":" + startMinute;

    if(endHour < startHour) {
      jTimePickerEnd.timepicker("setTime", newEndTime);
      return;
    }
    if(endHour == startHour) {
      if (parseInt(startMinute) > 0) {
        jTimePickerEnd.timepicker("setTime", newEndTime);
      } else {
        jTimePickerEnd.timepicker("setTime", startHour + ":30");
      }
    }
  }
};

allDataSet = function() {
    var startDate = jQuery("#partyStartDate").val(),
    endDate = jQuery("#partyEndDate").val(),
    startTime = jQuery("#partyStartTime").val(),
    endTime = jQuery("#partyEndTime").val();
    if ((startDate != "") && (endDate != "") && (startTime != "") && (endTime  != "")) {
      showEmptyTimeFields = false;
      jQuery("#yesButton").show();
      return true;
    }
    return false;
};

/**
 * Dialogbox mit den Schaltflächen "Ja" und "Nein"
 * Normalerweise wird als content Text übergeben,
 * wenn contentType 'html' gesetzt ist, kann auch HTML übergeben werden.
 * Die Höhe des Dialoges sollte sich dynamisch der Contentgröße anpassen.
 **/
PartyModeDialog = Class.create({
 
  initialize: function(title, content, deviceData ,callback, contentType)
  {
    showEmptyTimeFields = deviceData.showEmptyFields;

    var _this_ = this;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer";

    this.partyModeObject ={};
    this.SetPartyMode = false;
    this.deviceData = deviceData;
    this.defaultPartyTemp = 5;

    var dialog = document.createElement("div");
    dialog.className = "YesNoDialog";
    
    var titleElement = document.createElement("div");
    titleElement.className = "YesNoDialogTitle";

    var orientation = document.createAttribute("align");
    orientation.nodeValue = "center";
    titleElement.setAttributeNode(orientation);

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


    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('footerBtnCancel')));
    noButton.onclick = function() { _this_.no(); };
    footer.appendChild(noButton);

    var yesButton = document.createElement("div");
    yesButton.className = "YesNoDialog_yesButton borderRadius5px colorGradient50px";
    yesButton.id = "yesButton";
    yesButton.appendChild(document.createTextNode(translateKey('footerBtnOk')));
    yesButton.onclick = function() { _this_.yes(); };
    footer.appendChild(yesButton);

    dialog.appendChild(footer);
    
    this.m_layer.appendChild(dialog);
    
    Layer.add(this.m_layer);

    this.j_YesButton = jQuery("#yesButton");
    if (showEmptyTimeFields) {this.j_YesButton.hide();}

    //Sorgt dafür, daß die Dialoghöhe sich dynamisch dem Content anpasst.
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + 78);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + 26);

    translatePage(".YesNoDialog");
    this.initInputFields();
    refreshPartyDatePicker();
  },

  _isUserInputOk: function(sDate, sTime, eDate, eTime) {
    if (sDate && sTime && eDate && eTime) {
      return true;
    }
    return false;
  },

  _createPartyModeObject: function() {
    var startDate = jQuery("#partyStartDate").datepicker('getDate'),
     startTime = jQuery("#partyStartTime").timepicker('getTime'),
     stopDate = jQuery("#partyEndDate").datepicker('getDate'),
     stopTime = jQuery("#partyEndTime").timepicker('getTime');

    if(this._isUserInputOk(startDate, startTime, stopDate, stopTime)) {
      startDate.setHours(startTime.split(":")[0]);
      startDate.setMinutes(startTime.split(":")[1]);
      stopDate.setHours(stopTime.split(":")[0]);
      stopDate.setMinutes(stopTime.split(":")[1]);

      this.partyModeObject.startDay = startDate.getDate();
      this.partyModeObject.startMonth = startDate.getMonth() + 1;

      var startFullYear = startDate.getFullYear().toString();
      this.partyModeObject.startYear = parseInt(startFullYear[2] + startFullYear[3]);
      this.partyModeObject.startHour = startDate.getHours();
      this.partyModeObject.startMin = startDate.getMinutes();
      this.partyModeObject.startMinutesSinceMidnight = parseInt((startDate.getHours() * 60) + startDate.getMinutes());

      this.partyModeObject.stopDay = stopDate.getDate();
      this.partyModeObject.stopMonth = stopDate.getMonth() + 1;

      var stopFullYear = stopDate.getFullYear().toString();
      this.partyModeObject.stopYear = parseInt(stopFullYear[2] + stopFullYear[3]);
      this.partyModeObject.stopHour = stopDate.getHours();
      this.partyModeObject.stopMin = stopDate.getMinutes();
      this.partyModeObject.stopMinutesSinceMidnight = parseInt((stopDate.getHours() * 60) + stopDate.getMinutes());

      this.partyModeObject.temp = jQuery("#partyTempOption option:selected").val();
    } else {
      conInfo("Error PartyModeDialog._createPartyModeObject()");
    }
  },

  close: function(result)
  {
    var partyModeObject = false;
    if (this.SetPartyMode) {
      this._createPartyModeObject();
    }
    if (Object.keys(this.partyModeObject).length > 0 || ! this.SetPartyMode) {
      Layer.remove(this.m_layer);
      if (this.m_callback) { this.m_callback(result); }
    }
  },
  
  yes: function()
  {
    this.SetPartyMode = true;
    this.close(PartyModeDialog.RESULT_YES);
  },
  
  no: function()
  {
    this.SetPartyMode = false;
    this.close(PartyModeDialog.RESULT_NO);
  },

  // Activates the date- and timepicker and initializes the option fields of the tempererature selectbox
  initInputFields: function() {
    var jDatePickerStart = jQuery("#partyStartDate"),
      jDatePickerEnd = jQuery("#partyEndDate"),
      jTimePickerStart = jQuery("#partyStartTime"),
      jTimePickerEnd = jQuery("#partyEndTime"),
      startDate = new Date(),
      stopDate = new Date();

    if (showEmptyTimeFields != true) {
      if (this.deviceData.stPartyStartYear == 0 || this.deviceData.stPartyStartYear == undefined) {
        // Device Fw. < 1.3
        startDate.setHours(parseInt(startDate.getHours() + 1));
        stopDate.setHours(parseInt(startDate.getHours() + 3));
        jTimePickerStart.val(startDate.getHours() + ":00");
        jTimePickerEnd.val(stopDate.getHours() + ":00");
      } else {
        // Device Fw. >= 1.3
        startDate.setDate(this.deviceData.stPartyStartDay);
        startDate.setMonth(parseInt(this.deviceData.stPartyStartMonth) - 1);
        startDate.setYear((this.deviceData.stPartyStartYear.length == 4) ? this.deviceData.stPartyStartYear : 2000 + parseInt(this.deviceData.stPartyStartYear));
        startDate.setHours(this.deviceData.stPartyStartTime / 60);
        startDate.setMinutes(this.deviceData.stPartyStartTime % 60);
        startDate.setSeconds(0);

        stopDate.setDate(this.deviceData.stPartyStopDay);
        stopDate.setMonth(parseInt(this.deviceData.stPartyStopMonth) - 1);
        stopDate.setYear((this.deviceData.stPartyStopYear.length == 4) ? this.deviceData.stPartyStopYear : 2000 + parseInt(this.deviceData.stPartyStopYear));
        stopDate.setHours(this.deviceData.stPartyStopTime / 60);
        stopDate.setMinutes(this.deviceData.stPartyStopTime % 60);
        stopDate.setSeconds(0);

        var startMinutes = (parseInt(startDate.getMinutes()) < 10) ? "0" + startDate.getMinutes() : startDate.getMinutes();
        var stopMinutes = (parseInt(stopDate.getMinutes()) < 10) ? "0" + stopDate.getMinutes() : stopDate.getMinutes();

        jTimePickerStart.val(startDate.getHours() + ":" + startMinutes);
        jTimePickerEnd.val(stopDate.getHours() + ":" + stopMinutes);
      }
    } else {
      // Show empty inputfields
      jTimePickerStart.val();
      jTimePickerEnd.val();
      jTimePickerEnd.prop("disabled", true);
    }

    jDatePickerStart.datepicker({
      showOn: "focus",
      dateFormat: "dd.mm.yy",
      autoSize: true,
      firstDay: 1,
      minDate: 0,
      showButtonPanel:true,
      currentText: translateKey("btnToday"),
      closeText: translateKey("btnOk"),
      onClose: this.OnStartDateClose
    });

    jDatePickerEnd.datepicker({
      showOn: "focus",
      dateFormat: "dd.mm.yy",
      autoSize: true,
      firstDay: 1,
      minDate: 0,
      showButtonPanel: true,
      currentText: translateKey("btnToday"),
      closeText: translateKey("btnOk"),
      onClose: refreshPartyTimePicker
    });

    if (showEmptyTimeFields != true) {
      if (this.deviceData.stPartyStartYear == 0) {
        jDatePickerStart.datepicker("setDate", "+0d");
        jDatePickerEnd.datepicker("setDate", "+0d");
        this.deviceData.stPartyTemp = this.defaultPartyTemp;
      } else {
        jDatePickerStart.datepicker("setDate", startDate);
        jDatePickerEnd.datepicker("setDate", stopDate);
      }
    } else {
      jDatePickerStart.val();
      jDatePickerEnd.val();
      jDatePickerEnd.prop("disabled", true);
    }

    jTimePickerStart.timepicker({
      //showNowButton: true,
      showOn: "focus",
      minutes: {
        starts: 0,
        ends: 30,
        interval: 30
      },
      showCloseButton: true,
      closeButtonText: translateKey("btnOk"),
      beforeShow: this.OnStartTimeBeforeShow,
      onSelect: this.OnStartTimeSelect,
      onClose: this.OnStartTimeClose
    });

    jTimePickerEnd.timepicker({
      //showNowButton: true,
      showOn: "focus",
      minutes: {
        starts: 0,
        ends: 30,
        interval: 30
      },
      showCloseButton: true,
      closeButtonText: translateKey("btnOk"),
      onHourShow : this.OnEndHourShowCallBack,
      onMinuteShow: this.OnEndMinuteShowCallBack,
      onClose: allDataSet
    });

    // Adds a selectbox for the temperature to the party mode dialog
    var selBox = "<select>";
    for (var loop = 5; loop <= 30; loop++) {
      selBox += "<option value='"+loop+"'>"+loop+"&nbsp;&deg;C</option>";
    }
    selBox += "</select>";

    jQuery("#partyTempOption").html(selBox);
    jQuery("#partyTempOption option[value='"+this.deviceData.stPartyTemp+"']").prop("selected", true);
  },

  OnStartHourShowCallBack: function(hour) {
    return true;
  },

  OnStartTimeBeforeShow: function(elem) {
    if (showEmptyTimeFields == true) {
      var jTimePickerStart = jQuery(elem),
        date = new Date(),
        hour = date.getHours(),
        min = "00";

      if (jTimePickerStart.val() == "") {
        jTimePickerStart.timepicker("setTime", hour + ":" + min);
      }
    }
  },

  OnStartTimeSelect: function(time) {
    var jTimePickerEnd = jQuery("#partyEndTime"),
      endHour = jTimePickerEnd.timepicker("getHour"),
      endMinute = jTimePickerEnd.timepicker("getMinute"),
      arTime = time.split(":"),
      startHour = arTime[0],
      startMinute = arTime[1],
      newEndTime =  parseInt(startHour) + 1 + ":" + startMinute; //startHour + 1 + ":" + startMinute;
    if (StartDateEqualsStopDate && (showEmptyTimeFields != true)) {
      if ((startHour >= endHour) && (startMinute >= endMinute)) {
        jTimePickerEnd.timepicker("setTime", newEndTime);
      }
    }
    return true;
  },

  OnStartDateClose: function() {
    var jDatePickerStart = jQuery("#partyStartDate"),
      jDatePickerEnd = jQuery("#partyEndDate");

    if (jDatePickerStart.val() != "") {
      jDatePickerEnd.prop("disabled", false);
    }

    refreshPartyDatePicker();
  },

  OnStartTimeClose: function() {
    var jTimePickerStart = jQuery("#partyStartTime"),
      jTimePickerEnd = jQuery("#partyEndTime");

    if (jTimePickerStart.val() != "") {
      jTimePickerEnd.prop("disabled", false);
    }
    allDataSet();
  },

  // Makes only valid time selectors selectable - e. g. its´s not possible to set an end time earlier than the start time
  OnEndHourShowCallBack: function(hour) {
    var startHour = jQuery("#partyStartTime").timepicker('getHour'),
    startMin = jQuery("#partyStartTime").timepicker('getMinute');

    if (StartDateEqualsStopDate()) {
      if (hour < startHour || ((hour == startHour) && (parseInt(startMin) >= 30))) {
        return false;
      }
    }
    return true;
  },

  // Makes only valid time selectors selectable
  OnEndMinuteShowCallBack: function(endHour, endMin) {
    var startHour = jQuery("#partyStartTime").timepicker('getHour'),
    startMin = jQuery("#partyStartTime").timepicker('getMinute');

    if (StartDateEqualsStopDate()) {
      if ((startHour == endHour) && (endMin <= startMin)) {
        return false;
      }
    }
    return true;
  },

  getPartyModeObject: function() {
    return this.partyModeObject;
  }

});

PartyModeDialog.RESULT_NO = 0;
PartyModeDialog.RESULT_YES = 1;
