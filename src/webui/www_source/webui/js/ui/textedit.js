/**
 * textedit.js
 **/
 
/**
 * Text-Eingabefeld
 **/
UI.TextEdit = Class.create(UI.InputComponent, {

  initialize: function()
  {
    this.m_isEnabled = true;
    
    this.m_element = document.createElement("input");
    this.m_element.className = UI.TextEdit.CLASS_NAME;
    this.m_element.type = "text";
    this.m_element.value = UI.TextEdit.DEFAULT_VALUE;
  },
  
  setText: function(text)
  {
    this.m_element.value = text;
    return this;
  },
  
  getText: function()
  {
    return this.m_element.value;
  }


});

UI.TextEdit.CLASS_NAME = "UITextEdit";
UI.TextEdit.DEFAULT_VALUE = "";