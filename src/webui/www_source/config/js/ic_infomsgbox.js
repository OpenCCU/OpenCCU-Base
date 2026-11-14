/**
 * ic_infomsgbox.js
 **/

ShowInfoMsg = function(msg)
{
  InfoMsg = new InfoMessageBox("INFO", msg);
  InfoMsg.show();
};

ShowWarningMsg = function(msg)
{ 
  InfoMsg = new InfoMessageBox("WARNING", msg);
  InfoMsg.show();
};

ShowErrorMsg = function(msg)
{ 
  InfoMsg = new InfoMessageBox("ERROR", msg);
  InfoMsg.show();
};

InfoMessageBox = Class.create();

InfoMessageBox.prototype = Object.extend(new MsgBox(), {
  
  initialize: function(type, msg) {
  
  this.init(320, 240);

  this.AddDivWrapper("id_messagebox");
  this.AddDivWrapper("id_messagebox_wrapper", "id_messagebox");
  this.AddDivWrapper("id_messagebox_head",    "id_messagebox_wrapper");
  this.AddDivWrapper("id_messagebox_body",    "id_messagebox_wrapper");
  this.AddDivWrapper("id_messagebox_foot",    "id_messagebox_wrapper");

  var img = document.createElement("img");
  img.style.margin = "4px";
  
  if      (type == "INFO")    img.src = "/ise/img/dialog-information.png";
  else if (type == "WARNING") img.src = "/ise/img/dialog-warning.png";
  else                        img.src = "/ise/img/dialog-error.png";

  $("id_messagebox_head").style.verticalAlign = "top";
  $("id_messagebox_head").appendChild(img);
  
  var textnode;

  //if      (type == "INFO")    textnode = document.createTextNode("HomeMatic Information");
  if      (type == "INFO")    textnode = document.createTextNode(translateKey("dialogTitleHomeMaticInfo"));
  //else if (type == "WARNING") textnode = document.createTextNode("HomeMatic Warnung");
  else if (type == "WARNING") textnode = document.createTextNode(translateKey("dialogTitleHomeMaticWarn"));
  //else                        textnode = document.createTextNode("HomeMatic Fehler");
  else                        textnode = document.createTextNode(translateKey("dialogTitleHomeMaticError"));
  
  $("id_messagebox_head").appendChild(textnode);

  div = document.createElement("div");
  Element.addClassName(div, "CLASS10300");

  if (type == "ERROR")
  {
    Element.addClassName("CLASS10301");
  }
  div.innerHTML = msg;

  $('id_messagebox_body').appendChild(div);

  //var newInputDiv = this.CreateButton("OK");
  var newInputDiv = this.CreateButton(translateKey("btnOk"));
  newInputDiv.onclick = function() { InfoMsg.OnOK(); };
  newInputDiv.style.paddingRight = "20px";
  newInputDiv.style.paddingLeft  = "20px";
  $('id_messagebox_foot').appendChild(newInputDiv);
  $('id_messagebox_foot').style.textAlign = "center";

  //$('id_messagebox_wrapper').style.marginBottom = "5px";
  $('id_messagebox_wrapper').addClassName("CLASS10302");
  },

  OnOK: function ()
  {
  //InfoMsg.remove();
  InfoMsg.hide();
  }
});
