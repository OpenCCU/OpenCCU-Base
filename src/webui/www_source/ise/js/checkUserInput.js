/**
 * ise/checkUserInput.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/ 
 
/* 
 * * * checkUserInput.js
 * * * Enthaltene Funktionen
 *
 * checkTemperatureMinMax(input, MinVal, MaxVal)
 *    Überprüft ob Temperatur gültig und innerhalb von MinVal und MaxVal ist
 *
 * checkTemperature(input)
 *    Überprüft Temperatur ohne Min- und Max-Werte
 *    Erlaubt sind Eingaben wie 12, 13, 12C, 12°C, 87F, 87°F (sowie Dezimalwerte)
 * 
 * checkCharOnly(input)
 *
 * checkDigitsOnly(input)
 *
 * checkDecimal(input)
 *    Prüft auf eine Dezimalzahl
 *
 * checkPercentage(input)
 *   Erlaubte Eingaben: 50, 12, 13%, 13 %
 * 
 * convertPercentage(input)
 *   Entfernt das Prozentzeichen der Eingabe und gibt (input / 100) zurück
 *
 * checkInput(input, type)
 *   Kann mit einer der globalen Konstanten aufgerufen werden
 *
 * checkTime(val)
 *   gültige Eingabe  --> 12:54
 *
 * checkDate(val)
 *   gültige Eingabe  -->  18.09.2007 oder 18.09.07
 */

 
// * * *  Global Constants
CHK_TEMP = 1;
CHK_CHAR_ONLY = 2;
CHK_DIGITS_ONLY = 3;
CHK_PERCENT = 4;
CHK_DECIMAL = 5;


// * * * * * * * * * * * * * * * * 
// * * *       FUNCTIONS         *
// * * * * * * * * * * * * * * * * 

// Temperatur: erlaubt sind Zahlen sowie '.' und ','
// ausserdem: 'C', 'F', '°C'und '°F' als letzte Zeichen
// Bei Fahrenheit-Werten wird ausserdem nach Celsius umgerechnet
// und mit MinVal und MaxVal verglichen
//
// Wenn kein Vergleich mit Minimal- und Maximal-Werten gewünscht ist
// müssen MinVal und MaxVal auf 0 gesetzt werden
checkTemperatureMinMax = function(input, MinVal, MaxVal) {
  var doMinMaxCheck = ((MinVal !== 0) || (MaxVal !== 0));
  var tmpInput = input;
  var isFarVal = false;
  if (!checkDecimal(tmpInput)) {
    // Zeichen für Temperaturangaben rausfiltern
    if (tmpInput.indexOf('&deg;C') != -1) {
      tmpInput = tmpInput.substr(0, tmpInput.indexOf('&deg;C'));
    }
    if (tmpInput.indexOf('°C') != -1) {
      tmpInput = tmpInput.substr(0, tmpInput.indexOf('°C'));
    }
    if (tmpInput.indexOf('C') != -1) {
      tmpInput = tmpInput.substr(0, tmpInput.indexOf('C'));
    }
    if (tmpInput.indexOf('&deg;F') != -1) {
      tmpInput = tmpInput.substr(0, tmpInput.indexOf('&deg;F'));
      isFarVal = true;      
    }
    if (tmpInput.indexOf('°F') != -1) {
      tmpInput = tmpInput.substr(0, tmpInput.indexOf('°F'));
      isFarVal = true;      
    }
    if (tmpInput.indexOf('F') != -1) {
      tmpInput = tmpInput.substr(0, tmpInput.indexOf('F'));
      isFarVal = true;      
    }
    
    // Fall Abgleich mit MinValue und MaxValue gewünscht wird und ein
    // Fahrenheit-Wert eingegeben wurde...
    if (doMinMaxCheck) {
      if (isFarVal) { 
        if (checkDecimal(tmpInput)) {
          var celVal = farToCel(tmpInput); // Umrechnen und prüfen ob innerhalb MinVal und MaxVal
          return ((celVal > MinVal) && (celVal < MaxVal));
        }
        else {
          return false;
        }
      }
    }
  }
  // Falls Chars entfernt wurden nochmal auf Dezimalwert testen
  if (checkDecimal(tmpInput)) {
    if (doMinMaxCheck) {
      tmpInput = tmpInput.replace(/,/, "."); // für WerteVergleich
      return ((tmpInput >= MinVal) && (tmpInput <= MaxVal ));
    } 
    else {
      return true;
    }
  } 
  else {
   return false;
  }     
};

// Temperatur-Überprüfung ohne Beachtung von Minimal- und Maximal-Werten
checkTemperature = function(input) {
  return checkTemperatureMinMax(input, 0, 0);
};

checkCharOnly = function(input) {
  ok = true;
  for(var i = 0; i < input.length; i++) {
    if (!isChar(input.charAt(i))) {
      ok = false;
      break;
    }
  }
  return ok;
};

checkDigitsOnly = function(input) {
  var ok = true;
  for (var i = 0; i < input.length; i++) {
    if (isNaN(input.charAt(i))) {
      ok = false;
      break;
    }
  }
  return ok;
};

// CheckDecimal: erlaubt sind [0..9] und ' und .
// True wird auch bei ganzen oder negativen Zahlen zurückgegeben 
checkDecimal = function(input) {
  var ok = true;
  var i = 0;
  if (input.charAt(0) == '-') // für negative Zahlen
    i = 1;
  for (i; i < input.length; i++) {
    tmp = input.charAt(i);
    if (isNaN(tmp)) {
      // Ausnahmen für '.' und ','
      if (tmp != '.' && tmp != ',') {
        ok = false;
        break;
      }
    }
  }
  return ok;
};

// Percentage: Erlaubt sind Zahlen zwischen 0 und 100, sowie
// Eingaben mit Prozentzeichen mit evtl. Leerzeichen zwischen
// Zahl und Prozentzeichen. Keine Dezimalwerte.
checkPercentage = function(input) {
  var tmpInput = "";
  // Evtl. vorhandenes Prozentzeichen zuerst entfernen
  if (input.indexOf('%') != -1) {
    tmpInput = input.substr(0, input.indexOf('%'));
    if(tmpInput.length < 1) { // falls nur '%' übergeben wurde
      return false;
    }
  } 
  else {
    tmpInput = input;
  }
  if (checkDigitsOnly(tmpInput)) {
    if (tmpInput < 0) 
      return false;
    if (tmpInput > 100)
      return false;
  } 
  else {
    return false;
  }
  return true;
};

// Von einem Prozentwert (string: 50, 50%, 50 % usw.) wird das
// Prozentzeichen entfernt und der Wert geteilt durch 100 zurückgegeben
convertPercentage = function(input) {
  var tmpInput = "";
  // Evtl. vorhandenes Prozentzeichen zuerst entfernen
  if (input.indexOf('%') != -1) {
    tmpInput = input.substr(0, input.indexOf('%'));
    if(tmpInput.length < 1) { // falls nur '%' übergeben wurde
      return false;
    }
  } 
  else {
    tmpInput = input;
  }
  return  parseInt(tmpInput,10) / 100;
  
};

checkInput = function(input, type) {
  if (type == CHK_TEMP){
    return checkTemperature(input);
  }
  if (type == CHK_CHAR_ONLY) {
    return checkCharOnly(input);
  }
  if (type == CHK_DIGITS_ONLY) {
    return checkDigitsOnly(input);
  }
  if (type == CHK_PERCENT) {
    return checkPercentage(input);
  }
  if (type == CHK_DECIMAL) {
    return checkDecimal(input);
  }
  // Wenn man hier ankommt wurde der typ nicht erkannt
  alert("checkInput: type unknown");
  return null;
};

// * * * * * * * * * * * * * * * * 
// * * *   DATUMS-FUNKTIONEN     *
// * * * * * * * * * * * * * * * * 
checkDay = function(val) {
  var ret = false;
  if (checkDigitsOnly(val)) {
    if ( (val >= 1) && (val <=31) )
      ret = true;
  }
  return ret;
};

checkMonth = function(val) {
  var ret = false;
  if (checkDigitsOnly(val)) {
    if ( (val >= 1) && (val <=12) )
      ret = true;
  }
  return ret;
};

checkYear = function(val) {
  var ret = false;
  if (checkDigitsOnly(val)) {
    if ( (val > 0) )
      ret = true;
  }
  return ret;
};

checkHours = function(val)
{
  if( checkDigitsOnly(val) )
  {
    if( (val >= 0) && (val <= 23) ) return true;
  }
  return false;
};

checkMinutes = function(val)
{
  if( checkDigitsOnly(val) )
  {
    if( (val >= 0) && ( val <= 59) ) return true;
  }
  return false;
};

checkSeconds = function(val)
{
  if( checkDigitsOnly(val) )
  {
    if( (val >= 0) && ( val <= 59) ) return true;
  }
  return false;
};

// Eingabe abhängig von Parameter bSeparator 
checkTime = function(val)
{
  var sSplit = new Array(2);
  sSplit = val.split(":");
  if( val === "" ) return false;
  if( sSplit.length != 2 ) return false;
  var tHours =  parseInt(sSplit[0],10);
  var tMinutes = parseInt(sSplit[1],10);
  if( isNaN(tHours) || isNaN(tMinutes) ) return false;
  return ( checkHours(sSplit[0]) && checkMinutes(sSplit[1]) );
};


// Eingabe abhängig von Parameter bSeparator 
checkDate = function(val) {
  var ret = false;
  var sSplit = new Array(3);
  
  if (val === "")
    return false;
  sSplit = val.split(".");
  
  if (sSplit.length != 3) 
    return false;
  if (checkDay(sSplit[0])) {
    if (checkMonth(sSplit[1])) {
      if (checkYear(sSplit[2])) {
        ret = true;
      }
    }
  }
  return ret;
};


// * * * * * * * * * * * * * * * * 
// * * *    HILFS-FUNKTIONEN     *
// * * * * * * * * * * * * * * * * 

// Konvertiert einen Fahrenheit- in einen Celsius-Wert
farToCel = function(farVal) { 
  return ((farVal - 32) / 1.8);
};

// Gibt true zurück falls ein Character übergeben wurde
isChar = function(Data) {
  var varChars = "éèàùûôoöë-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ";
  return (varChars.indexOf(Data) != -1);
};
