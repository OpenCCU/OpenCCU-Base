/**
 * mainmenu.js
 **/

/**
 * Hauptmenü
 **/
MainMenu = Singleton.create({  
  MAINMENU_ADMIN_FILE:   "/webui/js/mainmenu/admin.js",
  MAINMENU_USER_FILE:    "/webui/js/mainmenu/user.js",
  ID:                    "menubar",
  MAINMENU_ITEM_LEFT:    "MainMenuItem_Left",
  MAINMENU_ITEM_RIGHT:   "MainMenuItem_Right",
  MAINMENU_ITEM_CAPTION: "MainMenuItem_Caption",
  SUBMENU:               "MainMenuSubMenu",
  CAPTION_SUFFIX:        "_TITLE",
  SUBMENU_SUFFIX:        "_SUBMENU",
  ITEM_SELECTED:         "MainMenuItem_Selected",
  ITEM_HIGHLIGH:         "MainMenuItem_Highlight",
  SUBITEM_HIGHLIGHT:     "MainMenuSubItem_Highlight",
  
  /**
   * Initialisiert das Hauptmenü
   **/
  initialize: function()
  {
    this.m_selectedId = null;
    this.m_id         = null;
    
    switch (getUPL())
    {
      case UPL_ADMIN: this.m_menu = eval("(" + HttpLoader.getText(this.MAINMENU_ADMIN_FILE) + ")"); break;
      case UPL_USER:  this.m_menu = eval("(" + HttpLoader.getText(this.MAINMENU_USER_FILE) + ")"); break;
      default:        this.m_menu = null; break;
    }
  },
  
  /**
   * Erzeugt ein Untermenü-Element.
   **/
  m_createSubmenuItem: function(menuItem, submenuItem)
  {
    var _submenuItem_ = submenuItem;
    var _menuItem_ = menuItem;
    
    var cell = document.createElement("div");
    var img = document.createElement("img");
    img.src="../ise/img/menuicons/"+submenuItem.id+".png";
    img.alt = "";
    cell.appendChild(img);
    cell.className = "MainMenuSubItem";
    cell.id = submenuItem.id;
    //cell.appendChild(document.createTextNode("${"+submenuItem.id+"}"));  
    cell.appendChild(document.createTextNode(" "+translateKey(submenuItem.id)));
    Event.observe(cell, "mouseover", function() { MainMenu.highlightOn(this); });
    Event.observe(cell, "mouseout", function()  { MainMenu.highlightOff(this); });
    Event.observe(cell, "click", function() { MainMenu.beginHideSubmenu(_menuItem_); _submenuItem_.action.defer(); });
    
    return cell;
  },
  
  /**
   * Erzeugt ein Menüelement
   **/
  m_createMenuItem: function(menuItem)
  {
    var _menuItem_ = menuItem;
    
    var menuElement = document.createElement("div");
    menuElement.id = menuItem.id;
    if (menuItem.align == "left") { menuElement.className = "MainMenuItem_Left"; }
    else                          { menuElement.className = "MainMenuItem_Right"; }

    if (menuItem.id == "menuHelpPage") {menuElement.className = "MainMenuItem_Right MainMenuItem_Help";}

    Event.observe(menuElement, "mouseover", function() { MainMenu.showSubmenu(_menuItem_); });
    Event.observe(menuElement, "mouseout", function() { MainMenu.beginHideSubmenu(_menuItem_); });
    
    var caption = document.createElement("div");
    caption.id = menuItem.id + "_TITLE";
    caption.className = "MainMenuItemCaption";
    //caption.appendChild(document.createTextNode("${"+menuItem.id+"}"));
    caption.appendChild(document.createTextNode(translateKey(menuItem.id)));
    Event.observe(caption, "click", function() { MainMenu.beginHideSubmenu(_menuItem_); _menuItem_.action.defer(); });
    menuElement.appendChild(caption);
    
    var submenu = menuItem.submenu;
    if (submenu.length > 0)
    {
      var submenuElement = document.createElement("div");
      submenuElement.id = menuItem.id + "_SUBMENU";
      submenuElement.className = "MainMenuSubMenu";
      Element.setStyle(submenuElement, {display: "none"});
      
      
      var tbody =  document.createElement("div");
      Element.setStyle(tbody, {"display": "flex"});
      Element.setStyle(tbody, {"flex-direction": "column"});
      submenuElement.appendChild(tbody);
      
      for (var i = 0, len = submenu.length; i < len; i++)
      {
        tbody.appendChild(this.m_createSubmenuItem(menuItem, submenu[i]));
      }
      menuElement.appendChild(submenuElement);
    }
    
    return menuElement;
  },
    
  /**
   * Erzeugt ein neues Hauptmenü
   **/
  create: function(menubar)
  {
    if (this.m_menu !== null)
    {
      for (var i = 0, len = this.m_menu.length; i < len; i++)
      {
        menubar.appendChild(this.m_createMenuItem(this.m_menu[i]));
      }
    }
    else
    {
      menubar.hide();
    }
  },
    
  /**
   * Zeigt ein Untermenü an
   **/
  showSubmenu: function(menuItem)
  {
    this.hideSubmenu();
    
    var id = menuItem.id;
    
    if (this.m_id == id) { this.m_id = null; }
    
    Element.addClassName(id, this.ITEM_HIGHLIGH);
    var submenu = $(id + this.SUBMENU_SUFFIX);
    if (submenu) { submenu.show(); }
  },
  
  /**
   * Beginnt damit, ein Untermenü verzögert zu schließen
   **/
  beginHideSubmenu: function(menuItem)
  {
    this.m_id = menuItem.id;
    window.setTimeout("MainMenu.hideSubmenu();", 1);
  },
  
  /**
   * Schließt ein Untermenü
   **/
  hideSubmenu: function()
  {
    if ($(this.m_id))
    {
      var menu    = $(this.m_id);
      var submenu = $(this.m_id + this.SUBMENU_SUFFIX);

      if (menu)    { menu.removeClassName(this.ITEM_HIGHLIGH); }
      if (submenu) { submenu.hide(); }
      this.m_id = null;
    }
  },
  
  /**
   * Hebt ein Untermenüelement farblich hervor.
   **/
  highlightOn: function(element, event)
  {
    Element.addClassName(element, this.SUBITEM_HIGHLIGHT);
  },
  
  /**
   * Deaktiviert die Hervorhebung eines Untermenüelements
   **/
  highlightOff: function(element, event)
  {    
    Element.removeClassName(element, this.SUBITEM_HIGHLIGHT);
  },
  
  /**
   * Wählt ein Menüelement aus.
   * Dieses wird dann entsprechend farblich hinterlegt.
   **/
  select: function(id)
  {
    if (this.m_selectedId !== null) { Element.removeClassName(this.m_selectedId, this.ITEM_SELECTED); }
    this.m_selectedId = id;
    if (this.m_selectedId !== null) { Element.addClassName(this.m_selectedId, this.ITEM_SELECTED); }
  }
  
});
