/**
 * ic_msgbox.js
 **/

MsgBox = Class.create();

MsgBox.prototype =
{
  initialize: function(width, height, container_id)
  {
    if (width && height) this.init(width, height, container_id);
  },
  init: function(width, height, container_id)
  {
  if (width || height)
  {
      this.setWidth(width);
      this.setHeight(height);
  }
  else
  {
    this.stretchToMax();
  }

  if (container_id && container_id !== "") { this.container_id = container_id; }
  else                                     { this.container_id = "centerbox"; }

    this.setTopMargin(6);
    this.setLeftMargin(6);
  
    this.createMessagebox();
  },
  setWidth: function(width)
  {
    this.width = width;
  },
  setHeight: function(height)
  {
    this.height = height;
  },
  setLeftMargin: function(left)
  {
    this.LeftMargin = left;
  },
  setTopMargin: function(top)
  {
    this.TopMargin = top;
  },  
  stretchToMax: function() { 
    var maxHeight = 0;
    if (window.innerHeight) {
      maxHeight = window.innerHeight;
    }
    else {
      if (window.document.documentElement && window.document.documentElement.clientHeight)
        maxHeight = window.document.documentElement.clientHeight;
      else
        maxHeight = window.document.body.offsetHeight;
      
      this.setHeight(maxHeight - 50);
    }
    var maxWidth = screen.availWidth;
    this.setWidth(maxWidth - 50);
  },
  createMessagebox: function()
  {
    if ( !$('messagebox') )
    {      
      var newDiv = document.createElement("div");
      var newDivId = document.createAttribute("id");
      newDivId.nodeValue = "messagebox";
      newDiv.setAttributeNode(newDivId);
      var newDivStyle = document.createAttribute("style");
      newDivStyle.nodeValue = "width:"+this.width+"px;height:"+this.height+"px;padding: 0px;position:absolute;left: 50%;margin-left: -"+(this.width/2)+"px;margin-top: -"+(this.height/2)+"px;";
      if( typeof( newDiv.style.cssText ) == "string" )
        newDiv.style.cssText = newDivStyle.nodeValue;
      else
        newDiv.setAttributeNode(newDivStyle);
      
      $(this.container_id).appendChild(newDiv);
      jQuery("#messagebox").draggable();
    }  
  },
  removeMessagebox: function()
  {
    if ( $('messagebox') )
    {
      $('messagebox').remove();
    }
  },
  setMessageText: function(text)
  {
    $(this.messagediv).innerHTML = text;
  },
  readaptSize: function()
  {
    if ( $('messagebox') )
    {
  /*
      $('messagebox').style.marginLeft = ($('messagebox').style.width.replace(/px$/,"")/2)*(-1)+"px";
      $('messagebox').style.marginTop = ($('messagebox').style.height.replace(/px$/,"")/2)*(-1)+"px";
  */
    //var h = $("messagebox").getHeight();
    var h = parseInt(jQuery("#messagebox").css("height"));
    $('messagebox').style.marginLeft = ($('messagebox').style.width.replace(/px$/,"")/2)*(-1)+"px";
    $('messagebox').style.marginTop = (h/4)*(-1)+"px";
    }
  },

  addMessage: function(id,message)
  {
    var newDiv;
    var newDivId;
    var newDivStyle;
    
    this.messagediv = id;
    newDiv = document.createElement("div");
    newDivId = document.createAttribute("id");
    newDivId.nodeValue = id;
    newDiv.setAttributeNode(newDivId);
    newDiv.className = "CLASS10102";
    var newTextNode = document.createTextNode(message);
    newDiv.appendChild(newTextNode);
    $('messagebox').appendChild(newDiv);
    newDiv = document.createElement("div");
    newDivId = document.createAttribute("id");
    newDivId.nodeValue = id+"_ctrls";
    newDiv.setAttributeNode(newDivId);
    newDiv.className = "CLASS10103";
    $('messagebox').appendChild(newDiv);
  },
  addButton: function(id,text,func)
  {
    var newInput = document.createElement("input");
    newInput.id = id;
    newInput.type = "button";
    newInput.value= text;
    newInput.onclick = func;
    Element.addClassName(newInput, "CLASS10100");  
//    var newBR = document.createElement("<br>");
//    $(this.messagediv).appendChild(newBR);
    $(this.messagediv+"_ctrls").appendChild(newInput);
  },
  show: function()
  {
  //$('messagebox').style.visibility = "visible";

  this.readaptSize();
  $('trlayer').show();
  $(this.container_id).show();
  },  
  hide: function(do_not_remove)
  {
  //$('messagebox').style.visibility = "hidden";
  //$('messagebox').hide();

  $(this.container_id).hide();
  if (! do_not_remove) this.remove();
  $('trlayer').hide();
  },
  removeAndReload: function()
  {
    this.hide();
//  this.remove();
  reloadPage();
  //location.reload();
  },    
  remove: function()
  {
    this.removeMessagebox();
    //this.removeModalbox();
  },
  LoadFromFile: function(url)
  {
  url = Get_ReGa_Path(url);
/*
    new Ajax.Updater('messagebox',url,{method:'get', evalScripts: true, onFailure:function(){alert('LoadFromFile-Error');} });
    try { this.OnFileLoaded(); } catch (e) {}
*/

  //var url = fn+'?sid='+SessionId;
  //var t = this; 
  var opt = 
  {
    method: 'get', 
    evalScripts: true,
    onComplete: function(trans) {
      centerMessageBox();
    },
    onFailure: function() { 
      alert('LoadFromFile-Error');
    }
  };

  new Ajax.Updater('messagebox', url, opt);
  },

  CreateButton: function(text) 
  {
    var newInputDiv = document.createElement("span");
    Element.addClassName(newInputDiv, "CLASS10101 colorGradient borderRadius2px");
    
    var newInputDivText = document.createTextNode(text);
    newInputDiv.appendChild(newInputDivText);
    
    return newInputDiv;
  }, 
  
  AddDivWrapper: function(divid, parentnodeid) {
    
  var newDiv = document.createElement("div");
  newDiv.setAttribute("id", divid);

  if (parentnodeid) $(parentnodeid).appendChild(newDiv);
    else              $('messagebox').appendChild(newDiv);
  }
};
