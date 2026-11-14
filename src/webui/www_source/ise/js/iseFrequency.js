/**
 * ise/iseFrequency.js
 **/

/**
 * @fileOverview ?
 * @copy of iseThermostat.js
 **/

 /**
 * @class
 **/
iseFrequency = Class.create();

iseFrequency.prototype = {
  /*
   * id = DOM-ID of switch
   * initState = Creation State 
   */
  initialize: function(id, initState, lvlDP, min, max, iViewOnly, bSliderPosFlag)
  {
    conInfo( "iseFrequency: initialize()" );
    conInfo ("value(: " + initState + ") min(" + min + ") max(" + max + ")");
    this.id = id;
    this.state = initState;
    this.lvlDP = lvlDP;
    this.min = min;
    this.max = max;
    this.factor = 100/(max-min);
    this.unit = "&nbsp;mHz";
    if(bSliderPosFlag) 
    {
        this.bSliderPosFlag = bSliderPosFlag;
    }
    else
    {
        this.bSliderPosFlag = false;
    }
    this.slider = new sliderControl( "thermo", this.id, initState, iViewOnly, this.bSliderPosFlag,this.min, this.max, this.factor, this.unit);
    this.hasRampClicked = false;
    this.txtDeg = $(this.id + "Deg");
    // Add event handlers
    if (iViewOnly === 0)
    {
      this.mouseOut = this.onMouseOut.bindAsEventListener(this);
      Event.observe($("slidCtrl" + this.id), 'mouseout', this.mouseOut);
    
      this.rampClick = this.onRampClick.bindAsEventListener(this);
      Event.observe(this.slider.e_base, 'mousedown', this.rampClick);
      
      this.handleClick = this.onHandleClick.bindAsEventListener(this);
      Event.observe($("slidCtrl" + this.id), 'mouseup', this.handleClick);
      
      this.clickUp = this.onClickUp.bindAsEventListener(this);
      Event.observe($(this.id + "Up"), 'click', this.clickUp);

      this.clickDown = this.onClickDown.bindAsEventListener(this);
      Event.observe($(this.id + "Down"), 'click', this.clickDown);

      this.percChange = this.onPercChange.bindAsEventListener(this);
      Event.observe($(this.id + "Deg"), 'change', this.percChange);
    }
    this.refresh(false);
  },
  
  onMouseOut: function(event)
  {
    var e = event;
    if (!e) { e = window.event; }
    var relTarg = e.relatedTarget || e.fromElement;
    if( relTarg )
    {
      var b1 = (relTarg.id.indexOf("slider")!=-1);
      var b2 = (relTarg.id.indexOf("base")!=-1);
      var b3 = (relTarg.id.indexOf("green")!=-1);
      var b4 = (relTarg.id.indexOf("spec")!=-1);
      if( !b1 && !b2 && !b3 && !b4) 
      {
        if( this.hasRampClicked )
        {
          conInfo( "iseFrequency: onMouseOut() ["+relTarg.id+"], wanna set: " + ( (this.slider.n_value/this.factor) + this.min)  );
          this.hasRampClicked = false;
          
          this.state = (this.slider.n_value/this.factor) + this.min;
          this.refresh();
        }
      }
    }
  },
 
  onRampClick: function(ev)
  {
     this.hasRampClicked = true;
     var pos = Position.page(this.slider.e_base);
     var offset = ev.clientX - pos[0];
     var val = ( offset * 100 ) / this.slider.n_controlWidth;  
     this.slider.f_setValue(val);
     this.state = (Math.floor(val)/ this.factor) + this.min;
     conInfo( "iseFrequency: onRampClick() at ("+val + ") set-> " + this.state );
     // this.refresh();
     //window.setTimeout("ibd"+this.id+".refresh()",1000);
  },
  
  onHandleClick: function()
  {
    conInfo( "iseFrequency: onHandleClick() perc: "  + this.txtDeg.value);
    this.hasRampClicked = false;
    this.state = this.txtDeg.value;
    this.refresh();
  },
  
  onClickUp: function()
  {
    conInfo( "iseFrequency: onClickUp()" );
    // this.state = (this.slider.n_value/this.factor);
    this.state = Math.round(this.state  + 100); 
    if (this.state > this.max)
      this.state = this.max;

    this.refresh();
  },
  
  onClickDown: function()
  {
    conInfo( "iseFrequency: onClickDown()" );
    // this.state = (this.slider.n_value/this.factor);
    this.state = Math.round(this.state - 100); 
    if (this.state < this.min)
      this.state = this.min;

    this.refresh();
  },
  
  onPercChange: function()
  {
    conInfo( "iseFrequency: onPercChange()" );
    if( isNaN(this.txtDeg.value) ) return;
    if( parseInt(this.txtDeg.value) > this.max ) this.txtDeg.value = this.max;
    if( parseInt(this.txtDeg.value) < this.min ) this.txtDeg.value = this.min;
    this.state = this.txtDeg.value;
    this.refresh();
  },
  
  update: function(newVal)
  {
    conInfo( "iseFrequency: update()" );
    this.state = newVal;
    this.refresh(newVal);
  },
  
  refresh: function(setstate)
  {
    conInfo( "iseFrequency: refresh()"+this.state );
    if(this.state < 0){ this.state = 0; }
    if (this.state > this.max) this.state = this.max;
    this.slider.f_setValue((this.state -this.min) * this.factor, true);
    this.txtDeg.value = round(this.state,2);
    if(typeof setstate == "undefined")
    {
      conInfo("setting DP "+this.lvlDP+" State -------> " + this.state);
      setDpState(this.lvlDP, (this.state));
    }
  }
};
