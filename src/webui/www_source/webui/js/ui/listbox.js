/**
 * listbox.js
 **/
 
/**
 * Auswahl aus einer Liste
 **/
UI.ListBox = Class.create(UI.InputComponent, {

  /**
   * Konstruktor. Erstellt eine neue Listbox
   **/
  initialize: function()
  {
    this.m_isEnabled = true;
    this.m_onChangeHandler = this.m_onChange.bind(this);
    
    this.m_items = [];
    
    this.m_element = document.createElement("select");
    this.m_element.className = UI.ListBox.CLASS_NAME;
    Event.observe(this.m_element, "change", this.m_onChangeHandler);
  },
  
  m_onChange: function(event)
  {
    if (this.m_onChangeCallback)
    {
      this.m_onChangeCallback(this.getSelectedItem());
    }
  },
  
  getSelectedItem: function()
  {
    return this.m_items[this.m_element.selectedIndex];
  },
  
  setOnChangeCallback: function(onChangeCallback)
  {
    this.m_onChangeCallback = onChangeCallback;
    return this;
  },
  
  add: function(item)
  {
    var option = document.createElement("option");
    option.appendChild(document.createTextNode(item.name));
    this.m_element.appendChild(option);
    this.m_items.push(item);
    return this;
  },
    
  remove: function(item)
  {
    /* todo */
  },
    
  selectItem: function(item)
  {
    return this.selectIndex(this.m_items.indexOf(item));
  },
	
	/**
	 * @fn selectItemById
	 * Wählt ein
	 **/
	selectItemById: function(id)
	{
		var items = this.m_items;
		
		for (var i = 0, len = items.length; i < len; i++)
		{
			var item = items[i];
			if (item.id == id) { this.selectIndex(i); }
		}

		return this;
	},
  
  selectIndex: function(index)
  {
    this.m_element.selectedIndex = index;
    return this;
  }
  
  
  

});

UI.ListBox.CLASS_NAME = "UIListbox";