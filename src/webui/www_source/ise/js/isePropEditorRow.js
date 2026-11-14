/**
 * ise/isePropEditorRow.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

// isePropEditorRow
// Repräsentiert eine Zeile in den Popup-Fenstern "Eigenschaft bearbeiten"

/**
 * @class
 **/
isePropEditorRow = Class.create();

isePropEditorRow.prototype = {

  // id: Id des Objekts dessen Inhalt in der Zeile dargestellt wird
  //     Bei neu angelegten Zeilen = -1
  // baseId: ID_ROOMS, ID_FUNCTIONS etc
  // objInfo 
  //    .name, .comment
  initialize: function(id, baseId, objInfo) {
    this.id = id;
    this.baseId = parseInt(baseId);
    this.objInfo = {
      comment: "< "+translateKey("lblEmpty")+" >"
    };
    Object.extend(this.objInfo, objInfo || {});
    
    if (id == -1) {
      switch(this.baseId) {
        case ID_ROOMS:
          this.objInfo.name = "< "+translateKey("lblNewRoom")+" >";
          break;
        case ID_FUNCTIONS: 
          this.objInfo.name = "< "+translateKey("lblNewFunction")+" >";
          break;
        default:
          break;
      }
    }
    bNecc = true;
    this.oldVal = "";
    
    // IDs der einzelnen Tabellenzellen
    this.nameCellId    = (id == -1 ? 'newName'    : 'name' + id);
    this.commentCellId = (id == -1 ? 'newComment' : 'comment' + id);
    
    var tr = Builder.node("tr", {className: "popupWhiteCells"});
    
    // Namen erzeugen
    var td1 = Builder.node("td", {id: this.nameCellId}, this.objInfo.name);
    tr.appendChild(td1);
    this.nameListener = this.editName.bindAsEventListener(this);
    Event.observe(td1, 'click', this.nameListener);

    // Kommentar erzeugen
    var td4 = Builder.node("td", {id: this.commentCellId}, this.objInfo.comment);
    tr.appendChild(td4);
    this.commentListener = this.editComment.bindAsEventListener(this);
    Event.observe(td4, 'click', this.commentListener);
    
    $("propTable").appendChild(tr);
    jQuery("#tableContainer").animate({scrollTop: jQuery("#propTable").height()},1000);
  },
    
  /*---------------------------------*/
  /*-----   Name Functions      -----*/
  /*---------------------------------*/  
  editName: function() {
    var inputId = 'nEdit' + this.id;
    var nameElem = $(this.nameCellId);
    this.oldVal = nameElem.innerHTML;
    if (this.oldVal.toLowerCase().indexOf('input') == -1) {
      var s = "<input id='"+inputId+"' type='text' value='"+this.oldVal+"' />";
      nameElem.innerHTML = s;
      
      // Namen im DOM ändern beim Verlassen des Textfeldes
      var changeListener = this.saveNameToDom.bindAsEventListener(this);
      Event.observe($(inputId), 'blur', changeListener);
      
      $(inputId).focus();
    }
  },
  
  saveNameToDom: function () {
    var newName = $("nEdit" + this.id).value;
    if (newName === "") {
      if (this.oldVal !== "") {
        $(this.nameCellId).innerHTML = this.oldVal;
      }
      else {
        if (this.baseId == ID_ROOMS) {
          $(this.nameCellId).innerHTML = "< neuer Raum >";
        }
        if (this.baseId == ID_FUNCTIONS) {
          $(this.nameCellId).innerHTML = "< neues Gewerk >";
        }
      }
      return;
    } 
    if( !isTextAllowed( newName , 1 ) ) return;
    $(this.nameCellId).innerHTML = newName;
    if(newName != this.objInfo.name) { 
      var opts;
      var t = this;
      var url = "/esp/system.htm?sid="+SessionId;
      var pb = '';
      pb += 'string action = "checkName";';
      pb += 'string checkName = "'+newName+'";';   
      pb += 'integer checkTypeId = '+ this.baseId +';';
       
      if (this.baseId == ID_ROOMS) {
        if (this.id == -1) {
          opts = {
            postBody: ReGa.encode(pb), 
            asynchronous: false,
            onComplete: function(resp) {
              newName = resp.responseText.replace(/[\r\n\t]/g, "");
              iseRooms.CreateRoomRetId(newName, t.baseId);
            }
          };
          new Ajax.Request(url, opts);  
        } 
        else {
          iseSystem.saveName(this.id, newName, this.nameCellId);
        }
      }
      
      
      if (this.baseId == ID_FUNCTIONS) {
        if (this.id == -1) {
          opts = {
            postBody: ReGa.encode(pb), 
            asynchronous: false,
            onComplete: function(resp) {
              newName = resp.responseText.replace(/[\r\n\t]/g, "");
              iseFunctions.CreateFunctionRetId(newName, t.baseId);
            }
          };
          new Ajax.Request(url, opts);  
        } 
        else {
          iseSystem.saveName(this.id, newName, this.nameCellId);
        }
      }
    }
  },
  
  /*---------------------------------*/
  /*-----   Comment Functions   -----*/
  /*---------------------------------*/   
  editComment: function() {
    var inputId = 'cEdit' + this.id;
    var commentElem = $(this.commentCellId);
    var oldVal = commentElem.innerHTML;
    
    if (oldVal.toLowerCase().indexOf('input') == -1) {
      var s = "<input id='"+inputId+"' type='text' value='"+oldVal+"' />";
      commentElem.innerHTML = s;
      
      // Namen im DOM ändern beim Verlassen des Textfeldes
      var changeListener = this.saveCommentToDom.bindAsEventListener(this);
      Event.observe($(inputId), 'blur', changeListener);
      
      $(inputId).focus();
    }
  },
  
  saveCommentToDom: function() {
    var newComment = $("cEdit" + this.id).value;
    $(this.commentCellId).innerHTML = newComment;
    
    if (this.baseId == ID_ROOMS) {
      iseRooms.ChangeRoomComment(this.id, newComment);
    }
    
    if (this.baseId == ID_FUNCTIONS) {
      iseFunctions.ChangeFunctionComment(this.id, newComment);
    }
  }
};


/*
iseCellEditor = Class.create();

iseCellEditor.prototype = {
  initialize: function(ctrlObj, id) {
    this.cell = ctrlObj;
    this.id = id;
    this.oldVal = "";
    var clickListener = this.editName.bindAsEventListener(this);
    Event.observe(this.cell, 'click', clickListener);
  },
  
  editName: function() {
    var inputId = 'nEdit' + this.cell.id;
    this.oldVal = this.cell.innerHTML;
    
    if (this.oldVal.toLowerCase().indexOf('input') == -1) {
      var s = "<input id='"+inputId+"' type='text' value='"+this.oldVal+"' />";
      this.cell.innerHTML = s;
      
      // Namen im DOM ändern beim Verlassen des Textfeldes
      var changeListener = this.saveNameToDom.bindAsEventListener(this);
      Event.observe($(inputId), 'blur', changeListener);
      
      $(inputId).focus();
    }
  },
  
  saveNameToDom: function () {
    var newName = $("nEdit" + this.cell.id).value;
    $(this.cell).innerHTML = newName;
    iseSystem.saveName(this.id, newName);
  },
};
*/