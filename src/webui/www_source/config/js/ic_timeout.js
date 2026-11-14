//TOM = TimeOutModule

//Defines
TOM_DAYIDX = new Array();
TOM_DAYIDX['SATURDAY'] = 0;
TOM_DAYIDX['SUNDAY'] = 1;
TOM_DAYIDX['MONDAY'] = 2;
TOM_DAYIDX['TUESDAY'] = 3;
TOM_DAYIDX['WEDNESDAY'] = 4;
TOM_DAYIDX['THURSDAY'] = 5;
TOM_DAYIDX['FRIDAY'] = 6;

TOM_DAY_ENG = new Array('SATURDAY', 'SUNDAY', 'MONDAY', 'TUESDAY', 'WEDNESDAY', 'THURSDAY', 'FRIDAY');

tom_endtime = 0;
tom_temperature = 1;
tom_level = 1;

tom_maxtimeout = 1440;
tom_mintimeout = 0;
//-----

TimeoutManager = Class.create();

TimeoutManager.prototype = Object.extend(new MsgBox(), {

  initialize: function (iface, address, isOldDevGeneration, prgName) {

    this.TOM_DAY  = new Array (
      translateKey('timeModuleLblSelSerialPatternSaturday') ,
      translateKey('timeModuleLblSelSerialPatternSunday') ,
      translateKey('timeModuleLblSelSerialPatternMonday') ,
      translateKey('timeModuleLblSelSerialPatternTuesday') ,
      translateKey('timeModuleLblSelSerialPatternWednesday') ,
      translateKey('timeModuleLblSelSerialPatternThursday') ,
      translateKey('timeModuleLblSelSerialPatternFriday')
    );

    this.isOldDevGeneration = isOldDevGeneration;
    this.iface = iface;
    this.address = address;

    this.prg = (prgName != undefined && prgName != null) ? prgName : "";

    //Woche anlegen und initialisieren
    this.week = new Array(7);
    this.divname = new Array(7); //DIV-Container
    this.weekdirty = new Array(7); //Sind Änderungen erfolgt?

    this.setMaxTimouts();

    for (var dayidx = 0; dayidx < 7; dayidx++) {
      this.week[dayidx] = new Array();
      this.divname[dayidx] = '';
      this.weekdirty[dayidx] = false;
    }
  },

  setMaxTimouts: function() {
    this.maxTimeOuts = (this.isOldDevGeneration == true) ? 24 : 13;
  },

  setDivname: function (day, divid) {
    var dayidx = TOM_DAYIDX[day];
    this.divname[dayidx] = divid;
  },

  tom_toTime: function (timeout) {

    var h = parseInt(timeout / 60);
    var m = timeout - h * 60;

    if (String(m).length == 1) m = "0" + m;
    if (String(h).length == 1) h = "0" + h;

    return h + ":" + m;
  },

  tom_toTimeout: function (time) {

    var tokens;
    var h, m;
    var timeout = -1;

    if (this.tom_isTime(time)) {
      tokens = time.split(':');
      h = tokens[0];
      m = tokens[1];

      timeout = parseInt(h, 10) * 60 + parseInt(m, 10);
    }

    return timeout;
  },

  tom_isTime: function (time) {
    return time.match(/^[0-2]?[0-9]:[0-5][0,5]$/) != null;
  },

  tom_isTemperature: function (temperature) {
    return temperature.match(/^[1-3]?[0-9]\.?[0-9]*$/) != null;
  },

  tom_checkAndSetTime: function (day, inputel, timeoutIdx) {

    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];
    var elem = inputel.id.split("_");
    var count = -1;
    var inputelem;
    var endtime;
    var prev_endtime;
    var next_endtime;
    var time;

    if (!this.prg) {
      while ($(elem[0] + '_' + elem[1] + '_' + (count + 1))) {
        count++;
        timeouts[count][tom_endtime] = this.tom_toTimeout($(elem[0] + '_' + elem[1] + '_' + count).value);
      }
    } else {
      while ($(this.prg + elem[1] + '_' + elem[2] + '_' + (count + 1))) {
        count++;
        timeouts[count][tom_endtime] = this.tom_toTimeout($(this.prg + elem[1] + '_' + elem[2] + '_' + count).value);
      }
    }

    for (var loop = 0; loop <= (count - 1); loop++) {
      timeoutIdx = loop;
      if (!this.prg) {
        inputelem = elem[0] + "_" + elem[1] + "_" + loop;
      } else {
        inputelem = this.prg + elem[1] + "_" + elem[2] + "_" + loop;
      }
      if (this.isOldDevGeneration) {
        // Minuten der Zeit auf volle 10 pruefen und ggf. anpassen
        $(inputelem).value = time = $(inputelem).value.replace(/[1-9]$/, "0");
      } else {
        // Minuten der Zeit auf volle 5 pruefen und ggf. anpassen
        var arTime = $(inputelem).value.split(":"),
        hour = parseInt(arTime[0]),
        min =Math.round(arTime[1] / 5) * 5 ;
        if (min <= 9) {min = "0" + min;}
        if (min == 60) {min = "00"; hour++;}
        if (hour <= 9) {hour = "0" + hour;}
        if (hour == 24) {hour = "23"; min = "55";}

        $(inputelem).value = time = hour + ":" + min;
      }
      endtime = this.tom_toTimeout(time);
      prev_endtime = -1;
      next_endtime = -1;


      if (timeoutIdx != 0) prev_endtime = timeouts[timeoutIdx - 1][tom_endtime];
      if (timeoutIdx != timeouts.length - 1) next_endtime = timeouts[timeoutIdx + 1][tom_endtime];


      $(inputelem).style.backgroundColor = WebUI.getColor("transparent");
      if (endtime > 0
        && endtime <= 1440
        && (prev_endtime < 0 || prev_endtime < endtime)
        && (next_endtime < 0 || next_endtime > endtime)) timeouts[timeoutIdx][tom_endtime] = parseInt(endtime);
      else $(inputelem).style.backgroundColor = WebUI.getColor("red");

      this.weekdirty[dayidx] = true;
    }
  },

  tom_checkAndSetTemperature: function (day, inputelem, timeoutIdx) {

    var temperature = inputelem.value;
    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];

    inputelem.style.backgroundColor = WebUI.getColor("transparent");

    if (this.tom_isTemperature(temperature)
      && temperature >= 0
      && temperature <= 30) timeouts[timeoutIdx][tom_temperature] = parseFloat(temperature);
    else                      inputelem.style.backgroundColor = WebUI.getColor("red");

    this.weekdirty[dayidx] = true;
  },

  tom_setDirty: function (day, inputelem, timeoutIdx) {
    CC_save_Temp(this.prg);
    var tmp = inputelem.id.split("_");

    if (this.prg) {
      var id = this.prg + tmp[1] + "_" + tmp[2] + "_" + tmp[3];
    } else {
      var id = tmp[0] + "_" + tmp[1] + "_" + tmp[2];
    }
    var temperature = $F(id);
    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];

    inputelem.style.backgroundColor = WebUI.getColor("transparent");

    if (this.tom_isTemperature(temperature)
      && temperature >= 5
      && temperature <= 30) timeouts[timeoutIdx][tom_temperature] = parseFloat(temperature);
    else                      inputelem.style.backgroundColor = WebUI.getColor("red");

    this.weekdirty[dayidx] = true;

  },

  tom_getPostStr: function () {

    var postStr = "";

    for (var dayidx = 0; dayidx < 7; dayidx++) {
      //Welcher Tag enthält die relevanten Daten: "wie am Vortag"-Funktion?
      var prev_day = $(this.prg + 'prevday_' + dayidx);
      var p = dayidx;
      while (prev_day.checked) {
        p--;
        prev_day = $(this.prg+ 'prevday_' + p);
      }
      //-----

      if (this.weekdirty[dayidx] || this.weekdirty[p]) //Dieser Tag oder der Vortag "dirty"?
      {
        var timeouts = this.week[p];

        if (timeouts && timeouts.length > 0) {
          for (var i = 0; i < timeouts.length; i++) {
            if (this.isOldDevGeneration) {
              postStr += "&TEMPERATUR_" + TOM_DAY_ENG[dayidx] +"_" + (i+1) + "=" + timeouts[i][tom_temperature];
              postStr += "&TIMEOUT_"    + TOM_DAY_ENG[dayidx] +"_" + (i+1) + "=" + timeouts[i][tom_endtime];
            } else {
              postStr += "&" + this.prg + "TEMPERATURE_" + TOM_DAY_ENG[dayidx] + "_" + (i + 1) + "=" + timeouts[i][tom_temperature];
              postStr += "&" + this.prg + "ENDTIME_" + TOM_DAY_ENG[dayidx] + "_" + (i + 1) + "=" + timeouts[i][tom_endtime];
            }
          }
        }
      }
    }

    return postStr;
  },

  tom_isDirty: function () {

    for (var dayidx = 0; dayidx < 7; dayidx++) {
      if (this.weekdirty[dayidx]) return true;
    }
    return false;
  },

  //day: MONDAY, TUESDAY, ...
  //endtime: 0..1440
  setTemp: function (day, endtime, temperature) {

    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];
    var i = 0;
    if (!timeouts) timeouts = new Array();

    //Array ist nach Endtime sortiert.
    while (i < timeouts.length && endtime >= timeouts[i][tom_endtime]) {
      //Timeout schon vorhanden? Dann Temperatur setzen.
      if (timeouts[i][tom_endtime] == endtime) {
        //timeouts[i][tom_temperature] = temperature;
        return;
      }

      i++;
    }

    if (i < timeouts.length) {
      //Es muss einsortiert werden.
      //Platz da!!!
      for (j = timeouts.length; j > i; j--) {
        timeouts[j] = timeouts[j - 1];
      }
    }

    timeouts[i] = new Array(2); //Zeitpunkt-Temperatur-Zuordnung anlegen
    timeouts[i][tom_endtime] = endtime;
    timeouts[i][tom_temperature] = temperature;
  },

  delTemp: function (day, endtime) {

    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];

    for (var i = 0; i < timeouts.length; i++) {
      if (timeouts[i][tom_endtime] == endtime) {
        timeouts.splice(i, 1);
        break;
      }
    }
  },

  delTempByIdx: function (day, timeoutIdx) {

    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];
    timeouts.splice(timeoutIdx, 1);

    this.weekdirty[dayidx] = true;
  },

  addTempByIdx: function (day, timeoutIdx) {

    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];

    if (timeouts.length >= this.maxTimeOuts ) {
      //alert('Der Zeitabschnitt kann nicht angelegt werden. Es k�nnen nur bis zu '+this.maxTimeOuts+'  Zeitabschnitte angelegt werden.');
      alert(translateKey('errorCreateTimePeriod') + translateKey('maxTimePeriodReachedA') + this.maxTimeOuts+translateKey('maxTimePeriodReachedB'));
      return;
    }
    else if (timeouts[timeoutIdx][tom_endtime] - 10 == 0) {
      //alert('Der Zeitabschnitt kann nicht angelegt werden. Die Endzeit kann nicht 00:00 Uhr sein.');
      alert(translateKey('errorCreateTimePeriod') + translateKey('endtimeReached'));
      return;
    }
    else if (timeoutIdx > 0 && timeouts[timeoutIdx][tom_endtime] - 10 <= timeouts[timeoutIdx - 1][tom_endtime]) {
      //alert('Der Zeitabschnitt kann nicht angelegt werden. Er existiert schon.');
      alert(translateKey('errorCreateTimePeriod') + translateKey('timePeriodAlreadyExists'));
      return;
    }
    this.setTemp(day, timeouts[timeoutIdx][tom_endtime] - 10, timeouts[timeoutIdx][tom_temperature]);

    this.weekdirty[dayidx] = true;
  },

  OnClickPrevDay: function (day) {
    var dayidx = TOM_DAYIDX[day];
    var elem = $(this.prg + 'prevday_' + dayidx);

    if (elem.checked) {
      $(this.prg+'tempprofile_' + dayidx).style.display = "none";
      $(this.prg+'tempprofile_' + dayidx).style.visibility = "hidden";
    }
    else {
      $(this.prg+'tempprofile_' + dayidx).style.display = "";
      $(this.prg+'tempprofile_' + dayidx).style.visibility = "visible";
    }
    this.tom_setDirty('SATURDAY', $(this.prg + 'temperature_0_0_tmp'), 0);
    this.weekdirty[dayidx] = true;
  },

  tom_equals_prevday: function (day) {

    var dayidx = TOM_DAYIDX[day];

    if (dayidx == 0) return false; //Starttag ist immer ungleich des Vortages

    var this_timeouts = this.week[dayidx    ];
    var prev_timeouts = this.week[dayidx - 1];

    if (!prev_timeouts || !this_timeouts || prev_timeouts.length != this_timeouts.length) return false;

    for (var i = 0; i < this_timeouts.length; i++) {
      if (this_timeouts[i][tom_temperature] != prev_timeouts[i][tom_temperature]
        || this_timeouts[i][tom_endtime] != prev_timeouts[i][tom_endtime]) return false;
    }
    return true;
  },

  checkDayTimeouts: function (dayidx) {
    var timeouts = this.week[dayidx];

    for (i = 1; i < timeouts.length; i++) {
      if (timeouts[i][tom_endtime] <= timeouts[i - 1][tom_endtime]) {
        alert("Der " + i + ". Zeitabschnitt hat eine ungültige Dauer");
      }
    }
  },

  checkTimeouts: function () {

    for (i = 0; i < this.week.length; i++) {
      checkDayTimeouts(i);
    }
  },

  writeDay: function (day) {

    var prgNr = this.prg;

    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];
    var endtime;

    if (!timeouts) return;

    var equals_prevday = this.tom_equals_prevday(day);

    msg = "<hr class=\"CLASS10400\" />";
    msg += "<table class=\"TimeoutTable\">";
    msg += "<tbody>";
    msg += "<tr>";
    //msg += "<td>Temperaturprofil " +TOM_DAY[dayidx] +":</td>";
    msg += "<td>" + translateKey('lblProgramTemperatureProfile') + "&nbsp;" + this.TOM_DAY[dayidx] + ":</td>";
    msg += "<td>&nbsp;";
    if (dayidx == 0) msg += "<div style=\"visibility: hidden; display: none;\">";
    //msg += "<input type=\"checkbox\" id=\"prevday_" +dayidx +"\" onclick=\"tom.OnClickPrevDay('" +day +"');\" " +(equals_prevday?'checked=\"checked\"':'\"\"') +"\"/>wie am Vortag";
    msg += "<input type=\"checkbox\" id=\""+prgNr+"prevday_" + dayidx + "\" onclick=\""+prgNr+"tom.OnClickPrevDay('" + day + "');\" " + (equals_prevday ? 'checked=\"checked\"' : '\"\"') + "\"/>" + translateKey('lblProgramPreviousDay');
    if (dayidx == 0) msg += "</div>";
    msg += "</td>";
    msg += "</tr>";
    msg += "</tbody>";
    msg += "</table>";

    msg += "<table class=\"TimeoutTable\" id=\""+prgNr+"tempprofile_" + dayidx + "\" " + (equals_prevday ? 'style=\"display: none; visibility: hidden;\"' : '') + ">";
    msg += "<thead>";
    //msg += "<tr><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>Startzeit</td><td>Endzeit</td><td>Temperatur</td></tr>";
    msg += "<tr><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>" + translateKey('lblProgramTimeStart') + "</td><td>" + translateKey('lblProgramTimeEnd') + "</td><td>" + translateKey('lblProgramTemperature') + "</td></tr>";
    msg += "</thead>";
    msg += "<tbody>";

    for (var i = 0; i < timeouts.length; i++) {
      if (i == 0) endtime = tom_mintimeout;
      else        endtime = timeouts[i - 1][tom_endtime];

      msg += "<tr>";
      msg += "<td><img " + (i == timeouts.length - 1 ? 'style=\"visibility: hidden;\"' : '') + " title=\"" + translateKey('toolTipProgramDelPeriod') + "\" alt=\"Zeitabschnitt l&ouml;schen\" onclick=\""+prgNr+ "tom.delTempByIdx('" + day + "'," + i + "); "+prgNr+ "tom.writeDay('" + day + "');\" style=\"cursor: pointer; width: 24px; height: 24px;\" src=\"/ise/img/cc_delete.png\"/></td>";
      msg += "<td><img src=\"/ise/img/add.png\" title=\"" + translateKey('toolTipProgramAddPeriod') + "\" alt=\"Zeitabschnitt hinzuf&uuml;gen\" style=\"width: 24px; height: 24px; cursor: pointer;\" onclick=\""+prgNr+ "tom.addTempByIdx('" + day + "'," + i + "); "+prgNr+ "tom.writeDay('" + day + "');\" /></td>";
      msg += "<td>" + (i + 1) + ". " + translateKey('lblProgramPeriod') + "</td>";
      //msg += "<td><input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\"starttime_"   +dayidx +"_"+i+"\" disabled=\"disabled\" type=\"text\" value=\""+ this.tom_toTime( endtime) +"\">Uhr</td>";
      msg += "<td><input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\""+prgNr+"starttime_" + dayidx + "_" + i + "\" disabled=\"disabled\" type=\"text\" value=\"" + this.tom_toTime(endtime) + "\">" + translateKey('lblProgramTimeExtension') + "</td>";
      msg += "<td><input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\""+prgNr+"endtime_" + dayidx + "_" + i + "\" " + (i == timeouts.length - 1 ? 'disabled=\"disabled\"' : '') + " type=\"text\" value=\"" + this.tom_toTime(timeouts[i][tom_endtime]) + "\" onblur=\""+prgNr+"tom.tom_checkAndSetTime ('" + day + "', this, " + i + "); document.getElementById('"+prgNr+"starttime_" + dayidx + "_" + (i + 1) + "').value=this.value; \">" + translateKey('lblProgramTimeExtension') + "</td>";
      msg += "<td><input style=\"text-align: right;\" maxLength=\"4\" size=\"3\" name=\""+prgNr+"temp_tmp\" id=\""+prgNr+"temperature_" + dayidx + "_" + i + "_tmp\" value=\"" + timeouts[i][tom_temperature].toFixed(1) + "\" onblur=\""+prgNr+"tom.tom_setDirty ('" + day + "', this, " + i + ");\">&nbsp;&deg;<span class=\"CF\">C</span></td>";
      // im naechsten Input-Feld stehen versteckt die zum Aktor zu uebertragenden Werte. Da der Aktor Werte im Grad Celsius erwartet,
      // muss der im Kanalparameter evtl. in F angezeigte Wert immer in Celsius umgerechntet und zwischengespeichert werden.
      msg += "<td><input style=\"display:none\" size=\"3\" name=\""+prgNr+"temp\" id=\""+prgNr+ "temperature_" + dayidx + "_" + i + "\" value=\"" + timeouts[i][tom_temperature].toFixed(1) + "\"></td>";
      msg += "</tr>";
    }
    msg += "</tbody>";
    msg += "</table>";
    $(this.divname[dayidx]).innerHTML = msg;

    if (this.isOldDevGeneration) {
      CC_setUnit();
    }
  }

});

TimeoutManagerHmIPOnOff = Class.create();

TimeoutManagerHmIPOnOff.prototype = Object.extend(new TimeoutManager(), {

  setMaxTimouts: function() {
    this.maxTimeOuts = 11;
  },

  setValue: function (day, endtime, temperature) {
    this.setTemp(day, endtime, temperature);
  },

  tom_setDirty: function (day, inputelem, timeoutIdx) {

    var tmp = inputelem.id.split("_");

    if (this.prg) {
      var id = this.prg + tmp[1] + "_" + tmp[2] + "_" + tmp[3];
    }

    var temperature = $F(id);
    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];

    inputelem.style.backgroundColor = WebUI.getColor("transparent");

    if (this.tom_isTemperature(temperature)
      && temperature >= 0
      && temperature <= 1) timeouts[timeoutIdx][tom_temperature] = parseFloat(temperature);
    else inputelem.style.backgroundColor = WebUI.getColor("red");

    this.weekdirty[dayidx] = true;

  },

  tom_getPostStr: function () {

    var postStr = "";

    for (var dayidx = 0; dayidx < 7; dayidx++) {
      //Welcher Tag enthält die relevanten Daten: "wie am Vortag"-Funktion?
      var prev_day = $(this.prg + 'prevday_' + dayidx);
      var p = dayidx;
      while (prev_day.checked) {
        p--;
        prev_day = $(this.prg+ 'prevday_' + p);
      }
      //-----

      if (this.weekdirty[dayidx] || this.weekdirty[p]) //Dieser Tag oder der Vortag "dirty"?
      {
        var timeouts = this.week[p];
        var hour, minute, level;

        if (timeouts && timeouts.length > 0) {
          for (var i = 0; i < timeouts.length; i++) {

            hour = parseInt(timeouts[i][tom_endtime] / 60);
            minute = parseInt(timeouts[i][tom_endtime] % 60);

            postStr += "&" + this.prg + "LEVEL_" + TOM_DAY_ENG[dayidx] + "_" + (i + 1) + "=" + timeouts[i][tom_level];
            postStr += "&" + this.prg + "HOUR_" + TOM_DAY_ENG[dayidx] + "_" + (i + 1) + "=" + hour;
            postStr += "&" + this.prg + "MINUTE_" + TOM_DAY_ENG[dayidx] + "_" + (i + 1) + "=" + minute;
          }
        }
      }
    }

    return postStr;
  },

  tom_isTime: function (time) {
    return time.match(/^[0-2]?[0-9]:[0-5][0-9]$/) != null;
  },

  tom_checkAndSetTime: function (day, inputel, timeoutIdx) {
    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];
    var elem = inputel.id.split("_");
    var count = -1;
    var inputelem;
    var endtime;
    var prev_endtime;
    var next_endtime;
    var time;
    var appendix = '_temp';

    while ($(this.prg + elem[1] + '_' + elem[2] + '_' + (count + 1) + appendix)) {
      count++;
      timeouts[count][tom_endtime] = this.tom_toTimeout($(this.prg + elem[1] + '_' + elem[2] + '_' + count + appendix).value);
    }

    for (var loop = 0; loop <= (count - 1); loop++) {
      timeoutIdx = loop;

      inputelem = this.prg + elem[1] + "_" + elem[2] + "_" + loop + appendix;

      var arTime = $(inputelem).value.split(":"),
      hour = parseInt(arTime[0]),
      min = parseInt(arTime[1]);

      if (min <= 9) {min = "0" + min;}
      if (min == 60) {min = "00"; hour++;}
      if (hour <= 9) {hour = "0" + hour;}
      if (hour == 24) {hour = "23"; min = "55";}

      $(inputelem).value = time = hour + ":" + min;

      endtime = this.tom_toTimeout(time);
      prev_endtime = -1;
      next_endtime = -1;


      if (timeoutIdx != 0) prev_endtime = timeouts[timeoutIdx - 1][tom_endtime];
      if (timeoutIdx != timeouts.length - 1) next_endtime = timeouts[timeoutIdx + 1][tom_endtime];

      $(inputelem).style.backgroundColor = WebUI.getColor("transparent");

      if (endtime > 0
        && endtime <= 1440
        && (prev_endtime < 0 || prev_endtime < endtime)
        && (next_endtime < 0 || next_endtime > endtime)) timeouts[timeoutIdx][tom_endtime] = parseInt(endtime);
      else $(inputelem).style.backgroundColor = WebUI.getColor("red");

      this.weekdirty[dayidx] = true;
    }
  },


  setHmIPData: function(elmID, time)  {
    // elmID = e.g. P1_endtimeHr_0_0  - first value on Saturday
    var prgNr = this.prg,
      arId = elmID.split("_"),
      arTime = time.split(":"),
      dayIdx = arId[2],
      index = arId[3],
      hour = arTime[0],
      minute = arTime[1];

    // This is the visible time field
    jQuery("#"+elmID).val(time);

    // This are the invisible time fields with the values to transmit
    jQuery("#" + prgNr + "endtimeHr_"+dayIdx+"_"+ (parseInt(index) - 1)).val(hour);
    jQuery("#" + prgNr + "endtimeMin_"+dayIdx+"_"+ (parseInt(index) - 1)).val(minute);
    jQuery("#" + prgNr + "starttimeHr_"+dayIdx+"_"+ index).val(hour);
    jQuery("#" + prgNr + "starttimeMin_"+dayIdx+"_"+ index).val(minute);
  },

  writeDayHmIPWeekProgramOnOff: function (day) {
    var prgNr = this.prg;
    var dayidx = TOM_DAYIDX[day];
    var timeouts = this.week[dayidx];
    var endtime;
    var startTimeHour, startTimeMin, endTimeHour, endTimeMin;

    if (!timeouts) return;

    var equals_prevday = this.tom_equals_prevday(day);

    msg = "<hr class=\"CLASS10400\" />";
    msg += "<table class=\"TimeoutTable\">";
    msg += "<tbody>";
    msg += "<tr>";
    msg += "<td>" + translateKey('lblProgramProfile') + "&nbsp;" + this.TOM_DAY[dayidx] + ":</td>";
    msg += "<td>&nbsp;";
    if (dayidx == 0) msg += "<div style=\"visibility: hidden; display: none;\">";
    msg += "<input type=\"checkbox\" id=\""+prgNr+"prevday_" + dayidx + "\" onclick=\""+prgNr+"tomHmIP.OnClickPrevDay('" + day + "');\" " + (equals_prevday ? 'checked=\"checked\"' : '\"\"') + "\"/>" + translateKey('lblProgramPreviousDay');
    if (dayidx == 0) msg += "</div>";
    msg += "</td>";
    msg += "</tr>";
    msg += "</tbody>";
    msg += "</table>";

    msg += "<table class=\"TimeoutTable\" id=\""+prgNr+"tempprofile_" + dayidx + "\" " + (equals_prevday ? 'style=\"display: none; visibility: hidden;\"' : '') + ">";
    msg += "<thead>";
    //msg += "<tr><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>Startzeit</td><td>Endzeit</td><td>Temperatur</td></tr>";
    msg += "<tr><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>" + translateKey('lblProgramTimeStart') + "</td><td>" + translateKey('lblProgramTimeEnd') + "</td><td>" + translateKey('lblProgramState') + "</td></tr>";
    msg += "</thead>";
    msg += "<tbody>";

    for (var i = 0; i < timeouts.length; i++) {
      if (i == 0) endtime = tom_mintimeout;
      else        endtime = timeouts[i - 1][tom_endtime];

      startTimeHour = parseInt(endtime / 60);
      startTimeMin = endtime % 60;
      endTimeHour = parseInt(timeouts[i][tom_endtime] / 60);
      endTimeMin = timeouts[i][tom_endtime] % 60;

      msg += "<tr>";
      msg += "<td><img " + (i == timeouts.length - 1 ? 'style=\"visibility: hidden;\"' : '') + " title=\"" + translateKey('toolTipProgramDelPeriod') + "\" alt=\"Zeitabschnitt l&ouml;schen\" onclick=\""+prgNr+ "tomHmIP.delTempByIdx('" + day + "'," + i + "); "+prgNr+ "tomHmIP.writeDayHmIPWeekProgramOnOff('" + day + "');\" style=\"cursor: pointer; width: 24px; height: 24px;\" src=\"/ise/img/cc_delete.png\"/></td>";
      msg += "<td><img src=\"/ise/img/add.png\" title=\"" + translateKey('toolTipProgramAddPeriod') + "\" alt=\"Zeitabschnitt hinzuf&uuml;gen\" style=\"width: 24px; height: 24px; cursor: pointer;\" onclick=\""+prgNr+ "tomHmIP.addTempByIdx('" + day + "'," + i + "); "+prgNr+ "tomHmIP.writeDayHmIPWeekProgramOnOff('" + day + "');\" /></td>";
      msg += "<td>" + (i + 1) + ". " + translateKey('lblProgramPeriod') + "</td>";

      msg += "<td><input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\""+prgNr+"starttime_" + dayidx + "_" + i + "_temp" + "\" disabled=\"disabled\" type=\"text\" value=\"" + this.tom_toTime(endtime) + "\">" + translateKey('lblProgramTimeExtension') + "</td>";
      // msg += "<td><input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\""+prgNr+"endtime_" + dayidx + "_" + i + "_temp" + "\" " + (i == timeouts.length - 1 ? 'disabled=\"disabled\"' : '') + " type=\"text\" value=\"" + this.tom_toTime(timeouts[i][tom_endtime]) + "\" onblur=\""+prgNr+"tomHmIP.tom_checkAndSetTime ('" + day + "', this, " + i + "); document.getElementById('"+prgNr+"starttime_" + dayidx + "_" + (i + 1)+ "_temp" + "').value=this.value; \">" + translateKey('lblProgramTimeExtension') + "</td>";


      msg += "<td><input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\""+prgNr+"endtime_" + dayidx + "_" + i + "_temp" + "\" " + (i == timeouts.length - 1 ? 'disabled=\"disabled\"' : '') + " type=\"text\" value=\"" + this.tom_toTime(timeouts[i][tom_endtime]) + "\" onblur=\""+prgNr+"tomHmIP.tom_checkAndSetTime ('" + day + "', this, " + i + "); "+prgNr+"tomHmIP.setHmIPData('"+prgNr+"starttime_" + dayidx + "_" + (i + 1)+ "_temp" + "', this.value);\">" + translateKey('lblProgramTimeExtension') + "</td>";

/*
      msg += "<td>";
        // Start Hour
        msg += "<input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\""+prgNr+"starttimeHr_" + dayidx + "_" + i + "\" disabled=\"disabled\" type=\"text\" value=\"" + startTimeHour + "\">";
       // Start Min
        msg += "<input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\""+prgNr+"starttimeMin_" + dayidx + "_" + i + "\" disabled=\"disabled\" type=\"text\" value=\"" + startTimeMin + "\">";

        // End Hour
        msg += "<input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\""+prgNr+"endtimeHr_" + dayidx + "_" + i + "\" " + (i == timeouts.length - 1 ? 'disabled=\"disabled\"' : '') + " type=\"text\" value=\"" + endTimeHour + "\" onblur=\"document.getElementById('"+prgNr+"starttimeHr_" + dayidx + "_" + (i + 1) + "').value=this.value; \">";
        // End Min
        msg += "<input style=\"text-align: right;\" maxLength=\"5\" size=\"7\" id=\""+prgNr+"endtimeMin_" + dayidx + "_" + i + "\" " + (i == timeouts.length - 1 ? 'disabled=\"disabled\"' : '') + " type=\"text\" value=\"" + endTimeMin + "\" onblur=\"document.getElementById('"+prgNr+"starttimeMin_" + dayidx + "_" + (i + 1) + "').value=this.value; \">";

      msg+= "</td>";

*/
      // Todo Implement a option box with 2 entries: On Off
      //msg += "<td><input style=\"text-align: right;\" maxLength=\"4\" size=\"3\" name=\""+prgNr+"level_tmp\" id=\""+prgNr+"level_" + dayidx + "_" + i + "\" value=\"" + parseInt(timeouts[i][tom_level]) + "\" onblur=\""+prgNr+"tomHmIP.tom_setDirty('" + day + "', this, " + i + ");\"></td>";

      var value = parseInt(timeouts[i][tom_level]);
      var sel0 = "", sel1 = "";
      if (value == 0) {
        sel0 = "selected";
      } else {
        sel1 = "selected";
      }

      msg += "<td>" +
        "<select name=\""+prgNr+"level_tmp\" id=\""+prgNr+"level_" + dayidx + "_" + i + "\" value=\"" + parseInt(timeouts[i][tom_level]) + "\" onblur=\""+prgNr+"tomHmIP.tom_setDirty('" + day + "', this, " + i + ");\">" +
          "<option value='0' "+sel0+">"+translateKey('optionWeeklyProgramStateOff')+"</option>" +
          "<option value='1' "+sel1+">"+translateKey('optionWeeklyProgramStateOn')+"</option>" +
        "</select>" +
        "</td>";

      msg += "</tr>";
    }
    msg += "</tbody>";
    msg += "</table>";
    $(this.divname[dayidx]).innerHTML = msg;

    if (this.isOldDevGeneration) {
      CC_setUnit();
    }
  }
});