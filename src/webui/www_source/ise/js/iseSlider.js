/**
 * ise/iseSlider.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

// dpID : id des Datenpunkts mit dem der Slider verknüpft wird
// BoundToBox : id der InputBox die dem Slider zugewiesen wird
// idHandle, idTrack : id der <div>-Tags die den Slider darstellen
// opts: falls null   -> setDatapointState
//          wenn gesetzt -> setDpDestState

/**
 * @class
 **/
dpSlider = Class.create();

dpSlider.prototype = {
  
  initialize: function (dpID, BoundToBox, idHandle, idTrack, imode, Opts ) {
    var dpThis = this;
    this.BoundTo = BoundToBox;
    this.DpId = dpID;
    this.iMode = imode;
    this.bMoving = false;
    this.bCreating = true;
    this.opts = Opts;
    
    this.dimmer = new Control.Slider(idHandle,idTrack,
    {
       range:$R(0,100),
       increment:10,
       step: 1,
       onSlide:function(v)
       {
          dpThis.bMoving = true;
          var tmpVal = parseInt((v*100)/100);
          $(dpThis.BoundTo).value = tmpVal; 
       },
       onChange:function(v)
       {
         v = parseInt((v*100)/100);
         if ($(dpThis.BoundTo)) { 
           $(dpThis.BoundTo).value = v;
         }
         if (dpThis.iMode == 1) {
           setDatapointState(dpThis.DpId, (v / 100));
         }
         if (dpThis.iMode == 2) {
         if (opts !== null) {
           if(dbg)alert("SetState("+(v / 100)+");");
           setDpDestState(dpThis.opts.iPrgId, dpThis.opts.iDstIx, (v / 100));
         } 
         }
         if (dpThis.iMode == 3) {
           if (dpThis.opts !== null) {
             setDpRightVal(dpThis.opts.iPrgId, dpThis.opts.iCondIndex, dpThis.opts.iRightVal, (v / 100));
           }
         }
         if (dpThis.iMode == 4) {
           if (dpThis.opts !== null) {
             setSceneDestVal(dpThis.opts.iSceneId, dpThis.opts.iDPID, (v / 100));
           }
         }
         if (dpThis.iMode == 5) {
           if (dpThis.opts !== null) {
             setSceneOpDestVal(dpThis.opts.iSceneId, dpThis.opts.iDPID, (v / 100));
           }
         }
         if (dpThis.iMode == 6) {
           if (!dpThis.bCreating) { // Verhindert Ausführung von opts.sFunc während der Erstellung
             if (dpThis.opts !== null) {
               EvalValue(dpThis.opts.sFunc, v);
             }
           }
           else {
             dpThis.bCreating = false;
           }
         }
         dpThis.bMoving = false;
       }
    });
  },
  
  SetValue : function(val) {
    if (val != this.dimmer.value) {
      if (!this.bMoving) {
        this.dimmer.setValue(val, 0);
      }
    }
  },
  
 NewTextValue : function() {
    var valEd = $(this.BoundTo);
    if (isNaN(valEd.value)) {
      valEd.value = 0;
    }
    this.dimmer.setValue(valEd.value);
  },

  EvalValue: function (s, value) {
    eval(s);
  }
};
