/**
 * ise/iseSubMenuControl.js
 **/

/**
 * @fileOverview ?
 * @author Michael Niehaus (ise)
 **/


// file: iseSubMenuControl
// author: Michael Niehaus
// date created: 18.04.2007

/**
 * @class
 **/
iseSubMenuControl = Class.create();

iseSubMenuControl.prototype = {
  // topMenuId: Id des TopMenüs bei dessen MouseOver das Submenü eingeblendet werden soll
  // subMenuId: Id des SubMenüs das eingeblendet werden soll
  // offsetDivId [optional]: wird benötigt um Verhalten des IE7 bei Positionierung auszugleichen
  //
  //      Wird eine Seite per Ajax nachgeladen ergibt im IE7 der Aufruf von Position.page() und
  //      Position.cumulativeOffset() (für ein Element bei dem Position auf 'aboslute' oder 'relative'
  //      gesetzt ist) immer die Position vom Ursprung der nachgeladenen Seite wieder,
  //      nicht die Position vom Ursprung des Browser-Fensters.
  //      Wenn ein Div direkt am Anfang der nachgeladenen Seite eingefügt wird (mit Position:'static'), kann mit
  //      Position.cumulativeOffset() etc. dessen Position zum Ursprung des Browser-Fenstern ermittelt
  //      werden, und somit kann man die Position des SubMenüs berechnen. 
  // shiftLeft: zur Darstellungs-Korrektur
  // mouseOpts: Um Custom-MouseOvers etc. hinzuzufügen
  // popup: submenüs sind in einem Popup (Darstellungskorrektur MOZ)
  // bOnClick: Submenüs öffnen bei OnClick, nicht bei MouseOver
  initialize: function(topMenuId, subMenuId, offsetDivId, shiftOpts, mouseOpts, popup, bOnClick,iFuncCount,iScreenHight)
  {
    this.bIE = NAV_IE;
    this.bMoz = NAV_MOZ;
    
    this.top = $(topMenuId);
    this.sub = $(subMenuId);
    this.offsetDiv = null;
    if (offsetDivId !== null) {
      this.offsetDiv = $(offsetDivId);
    }   
    //this.offsetDiv = $("indexOffset");
    this.shiftOpts = shiftOpts;
    this.mouseOpts = mouseOpts;
    this.popup = false;
    if (popup)
      this.popup = popup;
    if (iFuncCount)
      this.iFuncCount = iFuncCount;
    if (iScreenHight) // Get iScreenHight from calling application, if possible , cause both browsers are doing wrong with screen hight.
    {
      this.iScreenHight = iScreenHight;
    }
    else
    {
      this.iScreenHight = document.body.clientHeight;
    }
      
    this.positionSubMenu();
    
    // Top menu Click (DEBUG)
    this.topMenuClick = this.topMenuMouseClick.bindAsEventListener(this);
    Event.observe($(topMenuId), 'click', this.topMenuClick);

    if (bOnClick)
    {
      // Top menu Mouseover
      this.topMenuOver = this.topMenuMouseOver.bindAsEventListener(this);
      Event.observe($(topMenuId), 'click', this.topMenuOver);
    }
    else
    {
      // Top menu Mouseover
      this.topMenuOver = this.topMenuMouseOver.bindAsEventListener(this);
      Event.observe($(topMenuId), 'mouseover', this.topMenuOver);

      // Top menu Mouseout
      this.topMenuOut = this.topMenuMouseOut.bindAsEventListener(this);
      Event.observe($(topMenuId), 'mouseout', this.topMenuOut);
      
      // Sub menu Mouseout
      this.subMenuOut = this.subMenuMouseOut.bindAsEventListener(this);
      Event.observe($(subMenuId), 'mouseout', this.subMenuOut);
    }
    
    // Sub menu Mouseover
    this.subMenuOver = this.subMenuMouseOver.bindAsEventListener(this);
    Event.observe($(subMenuId), 'mouseover', this.subMenuOver);
  },
  
  positionSubMenu: function()
  {
    /*
    if (this.top.id == "TestTd") {
      this.sub.style.top = "20px";
      this.sub.style.left = "20px";
      return;
    }
    */
    
    var newX = 0;
    var newY = 0;
    
    //var topCoords = Position.cumulativeOffset(this.top);
    var topCoords = Position.page(this.top);
    
    var dimTop = this.top.getDimensions();
    var offsetCoords = new Array(2);
   
    if (this.popup) 
    {
      var tp = Position.page(this.top);
      var rp = Position.page($("rowhead"));
      var t = $("rowhead").getHeight() + $("rowflt").getHeight();
      if (this.bMoz)
        t += $("divTitle").getHeight();
      var l = tp[0] - rp[0];
      newX = l;
      newY = t;
      newY -= 4;
      
    }
    else 
    { 
      if ( this.offsetDiv && this.bIE ) 
      {
        offsetCoords = Position.page(this.offsetDiv);
        newY = topCoords[1] - offsetCoords[1] + dimTop.height;
        newX = topCoords[0] - offsetCoords[0];
/*
        // Function is disabled, because another solution is avaiable, but is possibly needed once.
        if (this.iFuncCount)  
        { 
          //alert(newY+170+(this.iFuncCount*16)+' > '+(this.iScreenHight)); // Have a lock for debugging.
          if ( (newY+170+(this.iFuncCount*16)>this.iScreenHight) )
            newY -= (this.iFuncCount*16)+3; // Pull-down-menu pulls up, if screen is to short.
          
        }
*/
        if (this.shiftOpts.l) 
          newX -= this.shiftOpts.l;
        if (this.shiftOpts.r) 
          newX += this.shiftOpts.r;
        if (this.shiftOpts.d) 
          newY += this.shiftOpts.d;
        if (this.shiftOpts.u) 
          newY -= this.shiftOpts.u;
/*          
        if (this.offsetDiv.getHeight() > 0) {
          newY -= this.offsetDiv.getHeight();
        }
*/
      }
      else 
      {
        newY = parseInt(topCoords[1] + dimTop.height);
/*
        // Function is disabled, because another solution is avaiable, but is possibly needed once.
        if (this.iFuncCount) 
        {
          if ( (newY+(this.iFuncCount*16)>this.iScreenHight) )
          {
            newY -= ((this.iFuncCount*16)+1); // Pull-down-menu pulls up, if screen is to short.
          }
        }
*/
        newX = topCoords[0];
        if (this.bMoz) {
          newY -= 1;
        }
      }
    }
    
    this.sub.style.top = newY + "px";
    this.sub.style.left = newX + "px";

    jQuery("#btnFilterFuncSub").draggable();
    jQuery("#btnFilterRoomSub").draggable();
  },
  
  topMenuMouseClick: function(mEvent)
  {
    // this.sub.style.left = "0px";
    // this.sub.style.top = "0px";
  },
  
  topMenuMouseOver: function(mEvent)
  {
    this.sub.show();
    this.positionSubMenu();
    if (this.mouseOpts) {
      if (typeof(this.mouseOpts.onTopOver) == 'function')
        this.mouseOpts.onTopOver(this.sub.id);
    }
    
  },
  
  topMenuMouseOut: function(mEvent)
  {
    var xPos = Event.pointerX(mEvent);
    var yPos = Event.pointerY(mEvent);
    if (!Position.within(this.sub, xPos, yPos))
    {
      this.sub.hide();
    }
  },
  
  subMenuMouseOver: function(mEvent)
  {
    var xPos = Event.pointerX(mEvent);
    var yPos = Event.pointerY(mEvent);
  },
  
  subMenuMouseOut: function(mEvent)
  {
    var xPos = Event.pointerX(mEvent);
    var yPos = Event.pointerY(mEvent);
    
    if (this.bIE) { // IE-Probleme für linken Rand beheben
      var subCoords = Position.cumulativeOffset(this.sub);
      if (xPos < (subCoords[0] + 5)) // wenn Maus am linken Rand...
        xPos = xPos - 3;
    }
    if ( (!Position.within(this.top, xPos, yPos)) && (!Position.within(this.sub, xPos, yPos)) ) {
      this.sub.hide();
    }
  },
  
  dbgLog: function(s)
  {
    if ($("dbgLog"))
      $("dbgLog").update(s);
  }
};