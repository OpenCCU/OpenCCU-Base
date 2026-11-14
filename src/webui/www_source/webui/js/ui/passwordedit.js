/**
 * textedit.js
 **/
 
/**
 * Text-Eingabefeld
 **/
UI.PasswordEdit = Class.create(UI.InputComponent, {

  initialize: function()
  {
    this.m_isEnabled = true;
    
    this.m_element = document.createElement("input");
    this.m_element.className = UI.PasswordEdit.CLASS_NAME;
    this.m_element.type = "password";
    this.m_element.value = UI.PasswordEdit.DEFAULT_VALUE;
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

UI.PasswordEdit.CLASS_NAME = "UITextEdit";
UI.PasswordEdit.DEFAULT_VALUE = "";