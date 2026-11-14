/**
 * textarea.js
 **/
 
/**
 * @fileOverview UI.Textarea
 * @author F. Werner (eQ-3)
 **/
 
/**
 * @class UI.Textarea
 **/
UI.Textarea = Class.create(UI.InputComponent, {

  initialize: function()
  {
    this.isEnabled = true;
    
    this.m_element = document.createElement("textarea");
    this.m_element.className = UI.Textarea.CLASS_NAME;
    Element.writeAttribute(this.m_element, "wrap", "off");
    this.m_element.value = UI.Textarea.DEFAULT_VALUE;
  },
  
  setText: function(text)
  {
    this.m_element.value = text;
    return this;
  },
  
  setID : function(id) {
    this.m_element.setAttribute("id",id);
    return this;
  },

  setName : function(name) {
    this.m_element.setAttribute("name",name);
    return this;
  },

  setWrap: function(isWrap)
  {
    if (isWrap) { Element.writeAttribute(this.m_element, "wrap", "soft"); }
    else        { Element.writeAttribute(this.m_element, "wrap", "off"); }
  
    return this;
  },
  
  getText: function()
  {
    return this.m_element.value;
  },

  enableResize: function() {
    Element.setStyle(this.m_element, {"resize": "both"});
    return this;
  },

  disableResize: function() {
    Element.setStyle(this.m_element, {"resize": "none"});
    return this;
  }

});

UI.Textarea.CLASS_NAME = "UITextArea";
UI.Textarea.DEFAULT_VALUE = "";
