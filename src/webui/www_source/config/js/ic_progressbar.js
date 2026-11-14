/**
 * ic_progressbar.js
 **/

//Defines
PROGRESSBAR_HL_COLOR = WebUI.getColor("progressbarHighlight");  //Highlight
PROGRESSBAR_BG_COLOR = WebUI.getColor("progressbarBackground"); //Background
PROGRESSBAR_TX_COLOR = WebUI.getColor("progressbarText");       //Text-Color
PROGRESSBAR_HL_STROKE = 4;//Strichstärke
PROGRESSBAR_WIDTH = 300;//Breite des Balkens
PROGRESSBAR_HEIGHT = 20;//Höhe des Balkens
PROGRESSBAR_KNIGHTRIDER_WIDTH = parseInt(PROGRESSBAR_WIDTH/5);
PROGRESSBAR_KNIGHTRIDER_STEP = 61;
//-----

ProgressBarMsgBox = Class.create();

ProgressBarMsgBox.prototype = Object.extend(new MsgBox(), {
  
  initialize: function(caption, endcounter) {
  
  this.init(335, 240, 'progressbox');
  this.index = 0;
  this.endcounter = endcounter;

  this.knightrider_position = 0;
  this.knightrider_running = false;

  if (!endcounter || endcounter < 0)
  {
    this.stepwidth = PROGRESSBAR_WIDTH;
    this.steppercent = 100;
  }
  else
  {
    this.stepwidth = Math.round(PROGRESSBAR_WIDTH / endcounter);
    this.steppercent = Math.round( 100 / endcounter );
  }

  this.AddDivWrapper("id_progressbar_wrapper");
  $('id_progressbar_wrapper').style.border = "3px solid " +PROGRESSBAR_HL_COLOR;
  
  this.AddDivWrapper("id_progressbar_head", "id_progressbar_wrapper");
  this.AddDivWrapper("id_progressbar_body", "id_progressbar_wrapper");
  this.AddDivWrapper("id_progressbar_foot", "id_progressbar_wrapper");

  $('id_progressbar_foot').style.height = "50px";
  $('id_progressbar_foot').style.marginTop = "10px";
  $('id_progressbar_foot').style.textAlign = "center";
  $('id_progressbar_foot').style.fontSize = "smaller";

  $('id_progressbar_head').classname = "popupTitle";
  $('id_progressbar_head').style.fontWeight = "bold";
  $('id_progressbar_head').style.padding = "4px";
  $('id_progressbar_head').style.textAlign = "center";
  var textnode = document.createTextNode(unescape(translateKey(caption)));
  $('id_progressbar_head').appendChild(textnode);

  var newDiv = document.createElement("div");
  newDiv.id = "id_progressbar_bar";
  newDiv.style.width  = PROGRESSBAR_WIDTH + "px";
  newDiv.style.height = PROGRESSBAR_HEIGHT + "px";
  newDiv.style.position = "relative";
  newDiv.style.margin = "10px 0 0 10px";
  $('id_progressbar_body').style.padding = "7px";
  $('id_progressbar_body').appendChild(newDiv);

  this.jg_progressbar = new jsGraphics("id_progressbar_bar");
    this.InitGD();
  this.ClearCanvas();

    //this.SetMessage("Bitte warten...");
    this.SetMessage(translateKey("progressBarLblPleaseWait"));
  },

  StartKnightRiderLight: function ()
  {
    this.knightrider_position = 0;
    if (PROGRESSBAR_KNIGHTRIDER_STEP < 0) PROGRESSBAR_KNIGHTRIDER_STEP = -PROGRESSBAR_KNIGHTRIDER_STEP; //rechts lang!
    
    this.ClearCanvas();
    this.jg_progressbar.fillRect(parseInt(0), parseInt(0), parseInt(PROGRESSBAR_KNIGHTRIDER_WIDTH), parseInt(PROGRESSBAR_HEIGHT));
    this.jg_progressbar.paint();
  
      this.knightrider_running = true;
    window.setTimeout("ProgressBar.StepKnightRiderLight();", 250);
  },
  
  StopKnightRiderLight: function ()
  {
    this.knightrider_running = false;
  },

  StepKnightRiderLight: function ()
  {
  if (! this.knightrider_running) return;

  if (this.knightrider_position + PROGRESSBAR_KNIGHTRIDER_STEP + PROGRESSBAR_KNIGHTRIDER_WIDTH > PROGRESSBAR_WIDTH)
  {
    this.knightrider_position = PROGRESSBAR_WIDTH - PROGRESSBAR_KNIGHTRIDER_WIDTH; //Letzte mögliche Position
    PROGRESSBAR_KNIGHTRIDER_STEP = -PROGRESSBAR_KNIGHTRIDER_STEP; //anders herum
  }
  else if (this.knightrider_position + PROGRESSBAR_KNIGHTRIDER_STEP < 0)
  {
    this.knightrider_position = 0;
    PROGRESSBAR_KNIGHTRIDER_STEP = -PROGRESSBAR_KNIGHTRIDER_STEP; //anders herum
  }
  else
  {
    this.knightrider_position += PROGRESSBAR_KNIGHTRIDER_STEP;
  }
  
  this.ClearCanvas();
  this.jg_progressbar.fillRect(parseInt(this.knightrider_position), parseInt(0), parseInt(PROGRESSBAR_KNIGHTRIDER_WIDTH), parseInt(PROGRESSBAR_HEIGHT));
  this.jg_progressbar.paint();

  window.setTimeout("ProgressBar.StepKnightRiderLight();", 250);
  },

  ClearCanvas: function ()
  {
    this.jg_progressbar.clear();
      this.jg_progressbar.setColor(PROGRESSBAR_BG_COLOR);
    this.jg_progressbar.fillRect(parseInt(0), parseInt(0), parseInt(PROGRESSBAR_WIDTH), parseInt(PROGRESSBAR_HEIGHT));
    this.jg_progressbar.paint();
      this.jg_progressbar.setColor(PROGRESSBAR_HL_COLOR);
  },

  InitGD: function () {
     this.jg_progressbar.setColor(PROGRESSBAR_HL_COLOR);
    this.jg_progressbar.setStroke(parseInt(PROGRESSBAR_HL_STROKE));
  },

  SetMessage: function(msg) {

  if (! $('id_progressbar_foot')) return;
  
  $('id_progressbar_foot').innerHTML = "";
  var textnode = document.createTextNode(msg);
  $('id_progressbar_foot').appendChild(textnode);
  },

  OnFinish: function() {
  return;
  },

  IncCounter: function(msg) {
  
  this.index++;

  this.SetMessage(msg);

  if (! this.knightrider_running)
  {
    //Prozentangabe anschreiben

    percent = this.steppercent * this.index;

    var w = this.index * this.stepwidth;
    if (w > PROGRESSBAR_WIDTH) w = PROGRESSBAR_WIDTH;

    this.ClearCanvas();
    this.jg_progressbar.fillRect(parseInt(0), parseInt(0), w, parseInt(PROGRESSBAR_HEIGHT));

    this.jg_progressbar.setColor(PROGRESSBAR_TX_COLOR);
    this.jg_progressbar.drawString(percent + "%", parseInt(PROGRESSBAR_WIDTH/2), 2);
    this.jg_progressbar.setColor(PROGRESSBAR_HL_COLOR);

    this.jg_progressbar.paint();
   }

  if (this.index == this.endcounter)
  {
      this.StopKnightRiderLight();//wenn vorhanden
    this.OnFinish();
    this.hide();
  }
  }
});
