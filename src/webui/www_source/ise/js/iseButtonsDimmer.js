/**
 * ise/iseButtonsDimmer.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class
 **/ 
iseButtonsDimmer = Class.create();

iseButtonsDimmer.prototype = {
  /*
   * id = DOM-ID of switch
   * initState = Creation State 
   */
  initialize: function(id, initState, lvlDP, oldLvlDP, iViewOnly, bSliderPosFlag, label)
  {
    conInfo( "iseDimmer: initialize()" );
    this.id = id;
    this.state = initState;
    this.lvlDP = lvlDP;
    this.oldLvlDP = oldLvlDP;
    // For some new devices (OSRAM-Lightify e. g.) we don`t know the state of the button
    // so we treat the On/Off button as a push-button instead of a switch.
    this.OnOffEqualsSwitch = (isNonCCUDevice(label)) ? false : true;


    if(bSliderPosFlag)
    {
        this.bSliderPosFlag = bSliderPosFlag;
    }
    else
    {
        this.bSliderPosFlag = false;
    }
    this.slider = new sliderControl("dimmer", this.id, initState, iViewOnly,this.bSliderPosFlag);
    
    this.hasRampClicked = false;
    
    this.txtPerc = $(this.id + "Perc");
    
    // Add event handlers
    if (iViewOnly === 0)
    {
      this.mouseOut = this.onMouseOut.bindAsEventListener(this);
      //Event.observe($("slidCtrl" + this.id), 'mouseout', this.mouseOut);
      Event.observe($("slidCtrl" + this.id), 'mouseleave', this.mouseOut);

      this.rampClick = this.onRampClick.bindAsEventListener(this);
      Event.observe(this.slider.e_base, 'mousedown', this.rampClick);
      
      this.handleClick = this.onHandleClick.bindAsEventListener(this);
      Event.observe($("slidCtrl" + this.id), 'mouseup', this.handleClick);
      
      this.clickUp = this.onClickUp.bindAsEventListener(this);
      Event.observe($(this.id + "Up"), 'click', this.clickUp);

      this.clickDown = this.onClickDown.bindAsEventListener(this);
      Event.observe($(this.id + "Down"), 'click', this.clickDown);

      if (this.oldLvlDP >= 1 ) {
        this.clickOn = this.onClickOn.bindAsEventListener(this);
        Event.observe($(this.id + "On"), 'mousedown', this.clickOn);
      } else {
        this.clickOn = this.onClickOnA.bindAsEventListener(this);
        Event.observe($(this.id + "On"), 'mousedown', this.clickOn);
      }
      this.clickOff  = this.onClickOff.bindAsEventListener(this);
      Event.observe($(this.id + "Off"), 'mousedown', this.clickOff);
      
      this.percChange = this.onPercChange.bindAsEventListener(this);
      Event.observe($(this.id + "Perc"), 'change', this.percChange);
    }
    this.refresh(false);
  },

  onMouseOut: function(event)
  {
    var self = this;
    var e = event;
    if (!e) { e = window.event; }
    var relTarg = e.relatedTarget || e.fromElement;
    if( relTarg )
    {
      var b1 = (relTarg.id.indexOf("slider")!=-1);
      var b2 = (relTarg.id.indexOf("base")!=-1);
      var b3 = (relTarg.id.indexOf("green")!=-1);
      if( !b1 && !b2 && !b3 ) 
      {
        if( this.hasRampClicked )
        {
          conInfo( "iseDimmer: onMouseOut() ["+relTarg.id+"]"  );
          this.hasRampClicked = false;
          window.setTimeout(function() {
            self.state = self.slider.n_value;
            self.refresh();
          },100);
        }
      }
    }
  },

  onRampClick: function(ev)
  {
     conInfo( "iseDimmer: onRampClick()" );
     this.hasRampClicked = true;
     var pos = Position.page(this.slider.e_base);
     var offset = ev.clientX - pos[0];
     var val = ( offset * 100 ) / this.slider.n_controlWidth;  
     var oldstate = parseInt(this.state);
     this.state = Math.floor(val);
     if (this.state < (oldstate-3))
     {
       this.slider.f_setValue(val);     
     }     
     else if (this.state > (oldstate+3))
     {
       this.slider.f_setValue(val);     
     } 
     //conInfo("setting Dimmer DP "+this.lvlDP+" State --> " + this.state + " -- old State --> "+oldstate);   
     //window.setTimeout("ibd"+this.id+".refresh()",1000);
  },
  
  onHandleClick: function()
  {
    conInfo( "iseDimmer: onHandleClick()" );
    this.state = this.txtPerc.value;
    this.refresh();
  },
  
  onClickUp: function()
  {
    conInfo( "iseDimmer: onClickUp()" );
    this.state = this.slider.n_value;
    this.state += 10; 
    if (this.state > 100)
    {
      this.state = 100;
    }
    this.refresh();
  },
  
  onClickDown: function()
  {
    conInfo( "iseDimmer: onClickDown()" );
    this.state = this.slider.n_value;
    this.state -= 10; 
    if (this.state < 0)
      this.state = 0;
    this.refresh();
  },
  
  onClickOn: function()
  {
    conInfo( "iseDimmer: onClickOn()" );
    var url = "/esp/channels.htm?sid=" + SessionId;
    var pb = "integer chnId = "+this.id+";";
    pb += "string action = 'dimmerOldVal';";
    var t = this;
    new PeriodicalExecuter(function(pe)
    {
      if( t.state > 0 )
      {
        if( $(t.id + "On") ) { ControlBtn.on($(t.id + "On")); }
        if( $(t.id + "Off") ) {ControlBtn.off($(t.id + "Off")); }
      }
      else
      {
        if( $(t.id + "On") ) { ControlBtn.off($(t.id + "On")); }
        if( $(t.id + "Off") ) { ControlBtn.on($(t.id + "Off")); }
      }
      pe.stop();
    }, 1);
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(resp)
      {
        t.refresh(false);
      }
    };
    new Ajax.Request(url, opts);
    ControlBtn.pushed($(this.id + "On"));
  },

  onClickOnA: function()
  {
    conInfo( "iseDimmer: onClickOnA()" );
    var t = this;
    if (this.OnOffEqualsSwitch) {
      new PeriodicalExecuter(function (pe) {
        if (t.state > 0) {
          if ($(t.id + "On")) {
            ControlBtn.on($(t.id + "On"));
          }
          if ($(t.id + "Off")) {
            ControlBtn.off($(t.id + "Off"));
          }
        }
        else {
          if ($(t.id + "On")) {
            ControlBtn.off($(t.id + "On"));
          }
          if ($(t.id + "Off")) {
            ControlBtn.on($(t.id + "Off"));
          }
        }
        pe.stop();
      }, 1);
    }
    setDpState(this.lvlDP, 1);
    ControlBtn.pushed($(this.id + "On"));

    if (! this.OnOffEqualsSwitch) {
      window.setTimeout(function() {ControlBtn.off( $(t.id + "On")  );},1000);
    }

  },

  onClickOff: function()
  {
    conInfo( "iseDimmer: onClickOff()" );
    var t = this;
    if (this.OnOffEqualsSwitch) {
      new PeriodicalExecuter(function (pe) {
        if (t.state > 0) {
          if ($(t.id + "On")) {
            ControlBtn.on($(t.id + "On"));
          }
          if ($(t.id + "Off")) {
            ControlBtn.off($(t.id + "Off"));
          }
        }
        else {
          if ($(t.id + "On")) {
            ControlBtn.off($(t.id + "On"));
          }
          if ($(t.id + "Off")) {
            ControlBtn.on($(t.id + "Off"));
          }
        }
        pe.stop();
      }, 1);
    }
    setDpState(this.lvlDP, 0);
    ControlBtn.pushed($(this.id + "Off"));

    if (! this.OnOffEqualsSwitch) {
      window.setTimeout(function() {ControlBtn.off( $(t.id + "Off")  );},1000);
    }

  },
 
  
  onPercChange: function()
  {
    conInfo( "iseDimmer: onPercChange()" );
    if( isNaN(this.txtPerc.value) ) return;
    if( parseInt(this.txtPerc.value) > 100 ) this.txtPerc.value = 100;
    if( parseInt(this.txtPerc.value) < 0 ) this.txtPerc.value = 0;
    this.state = this.txtPerc.value;
    this.refresh();
  },
  
  update: function(newVal)
  {
    conInfo( "iseDimmer: update()" );
    this.state = newVal;
    this.refresh(newVal);
  },
  
  refresh: function(setstate)
  {
    conInfo( "iseDimmer: refresh()" );
    var self = this;
    this.slider.f_setValue(this.state, true);
    this.txtPerc.value = this.state;

    if (this.OnOffEqualsSwitch) {

      window.setTimeout(function() {
        if (self.state > 0) {
          ControlBtn.on($(self.id + "On"));
          ControlBtn.off($(self.id + "Off"));
        } else {
          ControlBtn.off($(self.id + "On"));
          ControlBtn.on($(self.id + "Off"));
        }
      },1000);

    } else {
      // This is for devices without a state, e. g. OSRAM-Lightify
      ControlBtn.off($(this.id + "On"));
      ControlBtn.off($(this.id + "Off"));
    }
    if(typeof setstate == "undefined")
    {
      conInfo("setting Dimmer DP "+this.lvlDP+" State -------> " + this.state);    
      setDpState(this.lvlDP, (this.state / 100));
    }
  }
};
