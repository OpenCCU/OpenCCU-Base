Groups = Singleton.create({

  /**
   * Konstruktor
   **/
  initialize: function()
  {
    this.devices = {};
  },

  _getAllSerialNumbers: function(arDevices) {
    var arSN = [];
    jQuery.each(arDevices, function(index, dev){
      arSN.push(dev.serialNumber);
    });
    return arSN;
  },

  _getRegaID: function (devSN) {
    var result = "";
    jQuery.each(this.devices, function(index, device) {
      if (device.serialNumber == devSN) {
        result = device.regaID;
        return false; // leave each loop
      }
    });
    return result;
  },

  _getGroupAssignment:  function(url, arDevList, type, callback) {
   ShowWaitAnim();
    // In case of an unanswered call the animation will be switched off after 60 seconds.
    // Otherwise the animation would be visible as long as the WebUI is reloaded.
    HideWaitAnimAutomatically(60);
    var request = jQuery.ajax({
      url: url,
      type: (type == "POST") ? type : "GET",
      async: true,
      cache: false,
      data: JSON.stringify({ serialNumbers : arDevList }),
      dataType: "json"
    });

    request.done(function( result ) {
      HideWaitAnim();
      callback(result);
    });

    request.fail(function( jqXHR, textStatus ) {
      HideWaitAnim();
      conInfo(url + ".failed: " + jqXHR.statusText);
      callback(textStatus);
    });
  },

  _addCreateGroupBtn: function(sn) {
    var tbl = jQuery("#"+sn),
      objId = this._getRegaID(sn),
      devFailure = false,
      hiddenVisible = "";

    if (sessionStorage) {
      if (sessionStorage.getItem("teachInFailure_" + objId) == "true") {
        devFailure = true;
      }
    }

    hiddenVisible = (bCheckForAllChannels || devFailure) ? "hidden" : "";

    tr = Builder.node('tr', {id: 'btnCreateNewGroup' + objId, className: hiddenVisible});
    td = Builder.node('td');
    div = Builder.node('div', {className: 'StdButton CLASS04308 width160px border1px',  onclick: 'Groups.createNewGroup("'+sn+'","'+objId+'");'}, translateKey('btnCreateNewGroup'));
    td.appendChild(div);
    tr.appendChild(td);
    tbl.append(tr);
  },

  _addDevToGroupBtn: function(sn) {
    var tbl = jQuery("#"+sn),
      objId = this._getRegaID(sn),
      devFailure = false,
      hiddenVisible = "";

    if (sessionStorage) {
      if (sessionStorage.getItem("teachInFailure_" + objId) == "true") {
        devFailure = true;
      }
    }

    hiddenVisible = (bCheckForAllChannels || devFailure) ? "hidden" : "";

    tr = Builder.node('tr', {id: 'btnAddToGroup' +  objId, name: 'btnGroup', className: hiddenVisible});
    td = Builder.node('td');
    div = Builder.node('div', {className: 'StdButton CLASS04308 width160px border1px', onclick: 'Groups.addDevToGroup("'+sn+'","'+objId+'");'}, translateKey('btnAddToGroup'));
    td.appendChild(div);
    tr.appendChild(td);
    tbl.append(tr);
  },

  _isAjaxCallNecessary: function(arDevInBox) {
    var result = false;

    if ( GROUPASSIGNMENT == null || (GROUPASSIGNMENT.length < arDevInBox.length) ) {
        return true;
    }

    jQuery.each(arDevInBox, function(index, dev){
      var ok = false,
      devSN = dev.serialNumber;

      jQuery.each(GROUPASSIGNMENT, function(index, val){
        if (val.serialNumber == devSN) {
          ok = true;
          return false; //leave each loop
        }
      });

      if (ok == false) {
        result = true;
        return false; // leave each loop
      }
    });

    return result;
  },

  _addGroupButtons: function(grpAssignment) {
    if (grpAssignment.isAssignableToExistingGroup){
     // This will show a button for adding a device to a group within the device inbox
     this._addDevToGroupBtn(grpAssignment.serialNumber);
   }

   if (grpAssignment.isAssignableToNewGroup) {
     // This will show a button for creating an new group within the device inbox
     this._addCreateGroupBtn(grpAssignment.serialNumber);
   }
  },


  _getPossibleGroups: function(sn, regaID, callback) {
    ShowWaitAnim();
    HideWaitAnimAutomatically(60);
    var self = this;
    var url = "/pages/jpages/group/listPossibleGroups?sid="+SessionId;
    var request = jQuery.ajax({
      url: url,
      type: "POST",
      async: true,
      cache: false,
      data: JSON.stringify({serialNumber : sn, regaID: regaID}),
      dataType: "html"
    });

    request.done(function( result ) {
      HideWaitAnim();
       callback(result);
    });

    request.fail(function( jqXHR, textStatus ) {
      HideWaitAnim();
      conInfo(url + ".failed: " + jqXHR.statusText);
      callback(textStatus);
    });
  },

  createNewGroup: function(sn, regaID) {
    conInfo("createNewGroup");
    ShowWaitAnim();
    HideWaitAnimAutomatically(60);
    var url = "/pages/jpages/group/create?sid="+SessionId;
    var request = jQuery.ajax({
      url: url,
      type:  "POST",
      async: true,
      cache: false,
      data: JSON.stringify({serialNumber : sn, regaID: regaID}),
      dataType: "html"
    });
    request.done(function( result ) {
      HideWaitAnim();
      var content = jQuery.parseJSON(result).content;
      jQuery("#content").html(content);
    });

    request.fail(function( jqXHR, textStatus ) {
      HideWaitAnim();
      conInfo(url + ".failed: " + jqXHR.statusText);
      //callback(textStatus);
    });

  },

  addDevToGroup: function(sn, regaId) {
    this._getPossibleGroups(sn, regaId, function(result) {
      var patt = /javascript/g;
      if (patt.test(result)) {
        var data = '{ "serialNumber" : "'+sn+'", "regaID" : "'+regaId+'" }';
        CreateCPPopup("/pages/jpages/group/listPossibleGroups", data);
      } else {
        conInfo("addDevToGroup : no valid html");
      }
    });
  },

  createGroupButtons: function(arDevInBox) {
    this.devices = arDevInBox;
    var self = this,
    callNecessary = true, //this._isAjaxCallNecessary(this.devices),
    url = "/pages/jpages/group/checkGroupAssignment?sid="+SessionId;

    if ((arDevInBox.length > 0) && callNecessary) {
      var arSN = this._getAllSerialNumbers(this.devices);
      this._getGroupAssignment(url, arSN, "POST", function(result) {
        if (result.errorCode == "" && result.isSuccessful) {
          var content = jQuery.parseJSON(result.content);

          GROUPASSIGNMENT = content.groupAssignments;

          jQuery.each(GROUPASSIGNMENT, function(index,val) {
            self._addGroupButtons(val);
          });
        } else {
          conInfo(result.errorCode);
        }
      });
    } else {
      jQuery.each(GROUPASSIGNMENT, function(index,val) {
        self._addGroupButtons(val);
      });
    }
  }
});