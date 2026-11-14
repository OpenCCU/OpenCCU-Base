/**
 * ic_partyendtime.js
 **/
 
//pet = Party End Time Module

PartyEndTimeManager = Class.create();

PartyEndTimeManager.prototype = Object.extend(new MsgBox(), {
  
  initialize: function (iface, address, htmlcontainer, name, id) {

    this.iface    = iface;
    this.address  = address;
    this.htmlcont = htmlcontainer;
    this.name     = name;
    this.id       = id;
  },

  pet_Update: function () {

    var d = $F('pet_d');
    if (isNaN(d)) d = 0;

    var h = parseInt($F('pet_h'));
    var m = parseFloat($F('pet_m'));

    var endtime = d * 24 + h + m;

    if (endtime >= 24)
    {
      $('pet_m').disabled = true;
      $('pet_m').value = 0;

      $('pet_h').disabled = true;
      $('pet_h').value = 0;

      endtime = d * 24;
    }
    else 
    {
      $('pet_m').disabled = false;
      $('pet_h').disabled = false;
    }
		
		if (endtime > 3048) {endtime = 3048;}
		
    $(this.id).value = endtime;
  },

  writeControl: function (endtime) {

  var msg = "";
  var d, h, m;

  d = Math.floor(endtime / 24);
  
  tmp  = endtime - (d * 24);
  h = Math.floor(tmp);

  m = tmp-h;


  msg += "<input onkeyup=\"pet.pet_Update();\" id=\"pet_d\" type=\"text\" value=\""+d+"\" size=\"2\"/>d&nbsp; ";

  msg += "<select onchange=\"pet.pet_Update();\" id=\"pet_h\">";
  for (var i=0; i<24; i++)
  {
		if (i < 10) 
		{
			var zero = "0";
		} else 
		{
			var zero = "";
		}
    msg += "<option " +(i==h?'selected=\"selected\"':'') +">"+zero+i+"</option>";
  }
  msg += "</select>h&nbsp;";

  msg += "<select id=\"pet_m\" onchange=\"pet.pet_Update();\">";
  msg += "<option " +( m === 0 ? 'selected=\"selected\"' : '') +" value=\"0\">00</option>";
  msg += "<option " +( m >   0 ? 'selected=\"selected\"' : '')  +" value=\"0.5\">30</option>";
  msg += "</select>m<br/>";
  //msg += "max. 127 Tage";
  msg += translateKey("partyMaxDays127");

  //Sammelobjekt für richtigen Datenwert:
  msg += "<span style=\"visibility: hidden; display: none;\"><input id=\""+this.id+"\" type=\"text\" value=\""+endtime+"\" name=\""+this.name+"\"/></span>";

  $(this.htmlcont).innerHTML = msg;

  this.pet_Update();
  }
});
