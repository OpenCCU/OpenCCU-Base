/**
 * ise/programPopupLoader.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class
 **/
programPopupLoader = Class.create();
programPopupLoader.prototype =
{
  
  initialize: function(path,colName)
  {
    this.currentIndex = 0;
    this.path = path;
    this.colName = colName;
    this.loadPopupProgram(this.currentIndex);
  },

  loadPopupProgram: function(index)
  {      
    var t = this;    
    var url = t.path+"?sid="+SessionId;
    var pb = '';
    pb += 'integer iCurrentIndex = '+this.currentIndex+';';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(transParams)
      {
        transParams.responseText.evalScripts();
        var resp = iseStripAll(transParams.responseText.stripScripts());
        if( resp == "true" )
        {
          t.currentIndex+=5;
          t.loadPopupProgram(t.currentIndex);
        }
        else
        {          
          t.updateTable();
        }
      }
    };
    new Ajax.Updater("dummy",url,opts);
  },
 
  updateTable: function()
  {
    var tableBody = $("prgBody");
    if( tableBody )
    {
      var count = tableBody.rows.length-1;
      for(var x=count; x > -1; x--)
      {
        tableBody.deleteRow(x);
      }

      IseSort(arProgs,this.colName,false);
      for (var i = 0; i < arProgs.length; i++) 
      {
        var tr = Builder.node('tr', {id: arProgs[i]['trid']});
        //var td = Builder.node('td', {className: 'SysVarsTblPopup'}, strCut(translateString(arProgs[i]['name']) , true) );
        var td = Builder.node('td', {className: 'SysVarsTblPopup'}, translateString(arProgs[i]['name']));
        tr.appendChild(td);
        td = Builder.node('td', {className: 'SysVarsTblPopup'}, arProgs[i]['prginfo']);
        tr.appendChild(td);
        td = Builder.node('td', {className: 'SysVarsTblPopup'},  translateString(arProgs[i]['active']) );
        tr.appendChild(td);
      
        td = Builder.node('td', {className: 'WhiteBkg'}, [
          Builder.node('div', {className: 'StdButton', onclick:"saveDlgResult(" + arProgs[i]['id'] + ");PopupClose();"}, translateKey('btnSelect'))
         ]);
        tr.appendChild(td);

        tableBody.appendChild(tr);       
      }
      Cursor.set(Cursor.NORMAL);   
   } 
  }
  
};
