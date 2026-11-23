#!/bin/tclsh
source once.tcl
sourceOnce cgi.tcl
sourceOnce session.tcl

load tclrega.so
load tclrpc.so

set portnumber 2001
catch { source "/etc/eq3services.ports.tcl" }
if { [info exists EQ3_SERVICE_RFD_PORT] } {
  set portnumber $EQ3_SERVICE_RFD_PORT
}
set RFD_URL "bin://127.0.0.1:$portnumber"

set portnumber 2000
catch { source "/etc/eq3services.ports.tcl" }
if { [info exists EQ3_SERVICE_HS485D_PORT] } {
  set portnumber $EQ3_SERVICE_HS485D_PORT
}
set HS485D_URL "bin://127.0.0.1:$portnumber"

set KEYSERVER_CONF "/var/etc/crRFD.conf"
set KEYSERVER_HOST "secgtw.homematic.com"
set KEYSERVER_PORT 8443


proc getButton {class onclick title} {
  set html ""
  append html "<div class='$class' style='display:table; text-align:center; height:50px; width:110px' onclick='$onclick'>"
    append html "<span style='font-size: 11.9px !important; display:table-cell; vertical-align:middle'>$title</span>"
  append html "</div>"

  return $html
}

proc getButtonWired {class onclick title} {
  set html ""
  append html "<div class='$class' style='display:table; text-align:center; height:50px; width:110px' onclick='$onclick'>"
    append html "<span style='font-size:11px! important; display:table-cell; vertical-align:middle'>$title</span>"
  append html "</div>"

  return $html
}

proc initJavascript {} {
  global env
  cgi_javascript {
    puts "var keyserverUrl = \"$env(SCRIPT_NAME)?sid=\" + SessionId;"
    puts {
      keyserverCheckStarted = false;
      checkKeyserver = function() {
        var states = {
          "reachable": ["dialogNewDevicesKeyserverReachable", "keyserver-reachable"],
          "unreachable": ["dialogNewDevicesKeyserverUnreachable", "keyserver-unreachable"]
        };
        var showStatus = function(result) {
          var state = states.hasOwnProperty(result) ? states[result] : ["dialogNewDevicesKeyserverCheckFailed", ""];
          // looked up only now: translatePage() rewrites the cells of the
          // dialog after it was inserted and so replaces the elements
          jQuery(".keyserver-status")
            .removeClass("keyserver-reachable keyserver-unreachable")
            .addClass(state[1])
            .text(translateKey(state[0]));
          dlgPopup.readaptSize();
        };
        new Ajax.Request(keyserverUrl, {
          method: "post",
          postBody: "action=check_keyserver",
          onSuccess: function(transport) {
            showStatus(transport.responseText.trim());
          },
          onFailure: function() {
            showStatus("");
          }
        });
      };
      sectionShown = function(section) {
        if ((section == "HmIP-RF" || section == "HmIP-Wired") && !keyserverCheckStarted) {
          keyserverCheckStarted = true;
          checkKeyserver();
        }
      };
      showSection = function(section){
        var thisSection = jQuery("body").data(section);
        if(thisSection) {
          jQuery('#section'+section).show();
          jQuery('#sectionUNAVAIL').hide();
          sectionShown(section);
        } else {
          if (typeof thisSection == "undefined") {
            homematic('Interface.isPresent', {"interface": section}, function(result, error) {
              if (result == true)
              {
                jQuery("body").data(section, true);
                jQuery('#section'+section).show();
                jQuery('#sectionUNAVAIL').hide();
                sectionShown(section);
                dlgPopup.readaptSize();
              }
            });
          }
        }
      }
    }
  }
}

proc putSectionUNAVAIL {} {
  set html ""
    append html "<tr id='sectionUNAVAIL' class='CLASS21202'>"
      append html "<td class='CLASS21207' colspan='3'>"
      append html "\${dialogNewDevicesUNAVAILWarn}"
      append html "</td>"
    append html "</tr>"
  return $html
}

proc putSectionBidCosRF {} {

  set html ""
    append html "<tr id='sectionBidCos-RF' class='CLASS21202 hidden'>"
      append html "<td class='CLASS21207'>"
        #append html "\${dialogNewDevicesTDBidCosRF}"
        append html "<img src='/ise/img/homematic90Deg.png'>"
      append html "</td>"
      append html "<td class='CLASS21208' align='left'>"
        append html "\${dialogNewDevicesBidCosRFLbl1}"
        append html "\${dialogNewDevicesBidCosRFLbl2}"
        append html "<br /><br />"

        append html "<div class='popupControls CLASS21205'>"
          append html "<table>"
            append html "<tr>"
              append html "<td class='CLASS21206'>"
                append html "<div id='time_bar' class='CLASS21215' style='width:100%;'>"
                  append html "\${dialogNewDevicesBidCosRFFetchmodeNotActive}"
                append html "</div>"
              append html "</td>"
              append html "<td class='CLASS21209'>"
                append html [getButton {CLASS21210 colorGradient50px} {buttonPressed(this); rf_install_mode(true)} \${dialogNewDevicesAddDeviceBtn}]
              append html "</td>"
            append html "</tr>"
          append html "</table>"
        append html "</div>"
      append html "</td>"

      append html "<td  style='text-align:left;' class='CLASS21208'>"
        append html "\${dialogNewDevicesBidCosRFLbl3}"
        append html "\${dialogNewDevicesBidCosRFLbl5}"

        append html "<table style='table-layout:fixed; width: 100%; padding-right:15px'>"
        append html "<colgroup>"
          append html "<col style='width: 23%;'>"
          append html "<col style='width: auto;'>"
          append html "<col style='width: 110px;'>"
        append html "</colgroup>"

          append html "<tr>"
            append html "<td>\${dialogNewDevicesBidCosRFLbl4}</td>"
            append html "<td>"
              append html "<input id='text_serial' style='width: 100%; box-sizing: border-box;' name='serial' oninput='filterTeachInInput(this, TEACHIN_FIELD_BIDCOS_SERIAL, event);'>"
            append html "</td>"

            append html "<td>"
              append html "<div class='popupControls CLASS21205'>"
                append html "<table>"
                  append html "<tr>"
                    append html "<td class='CLASS21209'>"
                    append html [getButton {CLASS21210 colorGradient50px} {buttonPressed(this);  rf_serial()} \${dialogNewDevicesAddDeviceBtn}]
                    append html "</td>"
                  append html "</tr>"
                append html "</table>"
              append html "</div>"
            append html "</td>"


          append html "</tr>"
        append html "</table>"
      append html "</td>"
    append html "</tr>"
    append html "<script type='text/javascript'>"
      append html "showSection('BidCos-RF');"
    append html "</script>"
  return $html
}

proc putSectionWiredRF {} {
  set html ""
  append html "<tr id='sectionBidCos-Wired' class='CLASS21202 hidden'>"

    append html "<td class='CLASS21212'>"
      append html "<img src='/ise/img/homematicWired90Deg.png'>"
    append html "</td>"
    append html "<td  style='text-align:left;' class='CLASS21208'>"
      append html "\${dialogNewDevicesBidCosWiredLbl3}"
      append html "\${dialogNewDevicesBidCosWiredLbl4}"

      append html "<div class='popupControls CLASS21205'>"
        append html "<table>"
          append html "<tr>"
            append html "<td class='CLASS21206'></td>"
            append html "<td class='CLASS21209'>"
              append html [getButtonWired {CLASS21210 colorGradient50px} {buttonPressed(this);  wir_search()} \${dialogNewDevicesBidCosWiredBtn}]
            append html "</td>"
           append html "</tr>"
        append html "</table>"
      append html "</div>"

    append html "</td>"
    append html "<td  style='text-align:left;' class='CLASS21208'>"
      append html "\${dialogNewDevicesBidCosWiredLbl1}"
      append html "\${dialogNewDevicesBidCosWiredLbl2}"
    append html "</td>"

  append html "</tr>"

  append html "<script type='text/javascript'>"
    append html "showSection('BidCos-Wired');"
  append html "</script>"

}

# Creates the HmIP Section
proc putSectionHMIP {} {
  set html ""
  set iFace "HmIP-RF"

  append html "<tr id='section$iFace' class='CLASS21202 hidden'>"

   append html "<td class='CLASS21207'>"
   #append html "HmIP"
   append html "<img src='/ise/img/homematicIP90Deg.png'>"
   append html "</td>"

    append html "<td class='CLASS21208' align='left'>"
    append html "\${dialogNewDevicesHmIPWithInternet}"
    append html "<div class='popupControls CLASS21205'>"
      append html "<table>"
        append html "<tr>"
          append html "<td colspan='2'>"
            append html "\${dialogNewDevicesHmIPRFLbl1}"
          append html "</td>"
         append html "</tr>"

        append html "<tr>"
          append html "<td class='CLASS21206'><div id='teachCounter_$iFace' class='CLASS21215' style='width:100%;'>\${dialogNewDevicesBidCosRFFetchmodeNotActive}</div><span class='keyserver-status'>\${dialogNewDevicesKeyserverChecking}</span></td>"
          append html "<td>"
          append html [getButton {CLASS21210 colorGradient50px} {buttonPressed(this); hmip_install_mode("HmIP-RF")} \${dialogNewDevicesHmIPAddDeviceBtn}]
          append html "</td>"
        append html "</tr>"
      append html "</table>"
    append html "</div>"
   append html "</td>"

   append html "<td class='CLASS21208' align='left'>"
    append html "\${dialogNewDevicesHmIPWithoutInternet}"
    append html "<div class='popupControls CLASS21205'>"
      append html "<table>"

        append html "<tr>"
          append html "<td colspan='2'>"
            append html "\${dialogNewDevicesHmIPRFLbl2}"
          append html "</td>"
        append html "</tr>"

        append html "<tr>"
          append html "<td class='CLASS21206'>"

            append html "<table>"
              append html "<colgroup>"
                append html "<col style='width: 50px;'>"
                append html "<col style='width: auto;'>"
              append html "</colgroup>"

              append html "<tr>"
                append html "<td style='text-align: right'>"
                  append html "<span>\${lblTeachInKEY}</span>"
                append html "</td>"
                append html "<td style='white-space: nowrap;'>"
                  append html "<input id='keyHmIPLocal_$iFace' type='text' style='width:360px' placeholder='XXXXX-XXXXX-XXXXX-XXXXX-XXXXXX' oninput='filterTeachInInput(this, TEACHIN_FIELD_HMIP_KEY, event);'>"
                  append html "<img class='j_hmIPTeachInQrCodeHelp' src='/ise/img/help.png' height='24' width='24' style='vertical-align: middle; margin-left: 5px; cursor: help;'>"
                append html "</td>"
              append html "</tr>"

              append html "<tr>"
                append html "<td style='text-align: right'>"
                  append html "<span>\${lblTeachInSGTIN}</span>"
                append html "</td>"
                append html "<td>"
                  append html "<input id='serialHmIPLocal_$iFace' type='text' style='width:360px' placeholder='XXXX-XXXX-XXXX-XXXX-XXXX-XXXX' oninput='filterTeachInInput(this, TEACHIN_FIELD_HMIP_SGTIN, event);'>"
                append html "</td>"
              append html "</tr>"

            append html "</table>"

            append html "<table style='table-layout:fixed; width: 100%; padding-right:15px'>"
            append html "<colgroup>"
              append html "<col style='width: auto;'>"
              append html "<col style='width: 105px;'>"
            append html "</colgroup>"
              append html "<tr>"
                append html "<td><div id='teachCounterLocal_$iFace' class='CLASS21215'>\${dialogNewDevicesBidCosRFFetchmodeNotActive}</div></td>"
                append html "<td class='CLASS21209'>"
                  append html [getButton {CLASS21210 colorGradient50px} {buttonPressed(this); hmip_install_mode("HmIP-RF", "local")} {${dialogNewDevicesHmIPAddDeviceBtn}${lblLocal}}]
                append html "</td>"

              append html "</tr>"
            append html "</table>"

          append html "</td>"
        append html "</tr>"

      append html "</table>"
    append html "</div>"
   append html "</td>"
  append html "</tr>"

  append html "<script type='text/javascript'>"
    append html "showSection('HmIP-RF');"
  append html "</script>"

  return $html
}

# Creates the HmIP-Wired Section
proc putSectionHMIPWired {} {
  set html ""

  set iFace "HmIP-Wired"

  append html "<tr id='section$iFace' class='CLASS21202 hidden'>"

   append html "<td class='CLASS21207'>"
   append html "HmIP-Wired"
   #append html "<img src='/ise/img/homematicIP90Deg.png'>"
   append html "</td>"

    append html "<td class='CLASS21208' align='left'>"
    append html "\${dialogNewDevicesHmIPWithInternet}"
    append html "<div class='popupControls CLASS21205'>"
      append html "<table>"
        append html "<tr>"
          append html "<td colspan='2'>"
            append html "\${dialogNewDevicesHmIPRFLbl1}"
          append html "</td>"
         append html "</tr>"

        append html "<tr>"
          append html "<td class='CLASS21206'><div id='teachCounter_$iFace' class='CLASS21215' style='width:100%;'>\${dialogNewDevicesBidCosRFFetchmodeNotActive}</div><span class='keyserver-status'>\${dialogNewDevicesKeyserverChecking}</span></td>"
          append html "<td>"
          append html [getButton {CLASS21210 colorGradient50px} {buttonPressed(this); hmip_install_mode("HmIP-Wired")} \${dialogNewDevicesHmIPAddDeviceBtn}]
          append html "</td>"
        append html "</tr>"
      append html "</table>"
    append html "</div>"
   append html "</td>"

   append html "<td class='CLASS21208' align='left'>"
    append html "\${dialogNewDevicesHmIPWithoutInternet}"
    append html "<div class='popupControls CLASS21205'>"
      append html "<table>"

        append html "<tr>"
          append html "<td colspan='2'>"
            append html "\${dialogNewDevicesHmIPRFLbl2}"
          append html "</td>"
        append html "</tr>"

        append html "<tr>"
          append html "<td class='CLASS21206'>"

            append html "<table>"
              append html "<colgroup>"
                append html "<col style='width: 50px;'>"
                append html "<col style='width: auto;'>"
              append html "</colgroup>"

              append html "<tr>"
                append html "<td style='text-align: right'>"
                  append html "<span>\${lblTeachInKEY}</span>"
                append html "</td>"
                append html "<td style='white-space: nowrap;'>"
                  append html "<input id='keyHmIPLocal_$iFace' type='text' style='width:360px' placeholder='XXXXX-XXXXX-XXXXX-XXXXX-XXXXXX' oninput='filterTeachInInput(this, TEACHIN_FIELD_HMIP_KEY, event);'>"
                  append html "<img class='j_hmIPTeachInQrCodeHelp' src='/ise/img/help.png' height='24' width='24' style='vertical-align: middle; margin-left: 5px; cursor: help;'>"
                append html "</td>"
              append html "</tr>"

              append html "<tr>"
                append html "<td style='text-align: right'>"
                  append html "<span>\${lblTeachInSGTIN}</span>"
                append html "</td>"
                append html "<td>"
                  append html "<input id='serialHmIPLocal_$iFace' type='text' style='width:360px;' placeholder='XXXX-XXXX-XXXX-XXXX-XXXX-XXXX' oninput='filterTeachInInput(this, TEACHIN_FIELD_HMIP_SGTIN, event);'>"
                append html "</td>"
              append html "</tr>"

            append html "</table>"

            append html "<table style='table-layout:fixed; width: 100%; padding-right:15px'>"
            append html "<colgroup>"
              append html "<col style='width: auto;'>"
              append html "<col style='width: 105px;'>"
            append html "</colgroup>"
              append html "<tr>"
                append html "<td><div id='teachCounterLocal_$iFace' class='CLASS21215'>\${dialogNewDevicesBidCosRFFetchmodeNotActive}</div></td>"
                append html "<td class='CLASS21209'>"
                  append html [getButton {CLASS21210 colorGradient50px} {buttonPressed(this); hmip_install_mode("HmIP-Wired", "local")} {${dialogNewDevicesHmIPAddDeviceBtn}${lblLocal}}]
                append html "</td>"

              append html "</tr>"
            append html "</table>"

          append html "</td>"
        append html "</tr>"

      append html "</table>"
    append html "</div>"
   append html "</td>"
  append html "</tr>"

  append html "<script type='text/javascript'>"
    append html "showSection('HmIP-Wired');"
  append html "</script>"
  return $html
}

proc action_put_page {} {
  global env RFD_URL
  
  set serial ""
  catch { import serial }


  division {class="popupTitle j_translate"} {
    #puts "Ger&auml;te anlernen"
    puts "\${dialogNewDevicesTitle}"
  }

puts "<div class='CLASS21200 j_translate'>"

  puts "<style>.CLASS21206 .keyserver-status { display:block; margin-top:5px; text-align:center; background:none; border:0; box-shadow:none; } .keyserver-status.keyserver-reachable { color: green; } .keyserver-status.keyserver-unreachable { color: red; }</style>"

  initJavascript

  puts "<table class='popupTable' border='1' style='table-layout:fixed; width:100%;'>"
    puts "<colgroup>"
      puts "<col style='width:90px;'>"
      puts "<col style='width:359px;'>"
      puts "<col style='width:462px;'>"
    puts "</colgroup>"

    # Start section UNAVAIL
    puts [putSectionUNAVAIL]
    # End section UNAVAIL

    # Start section BidCos-RF
    puts [putSectionBidCosRF]
    # End section BidCos-RF

    # Start section Wired-RF
    puts [putSectionWiredRF]
    # End section Wired-RF

    # Start section HM IP
    puts [putSectionHMIP]
    # End section HM IP

    # Start section HM IP Wired
    puts [putSectionHMIPWired]
    # End section HM IP Wired
  puts "</table>"

puts "</div>"

  division {class="popupControls"} {
    table {
      table_row {
        table_data {class="CLASS21209"} {
          division {class="CLASS21213 colorGradient50px"} {onClick="buttonPressed(this); OnBack();"} {
            #puts "Zur&uuml;ck"
            puts "\${dialogBack}"
          }
        }
        table_data {class="CLASS21209"} {
          division {id="cp_new_devices_button"} {class="CLASS21216 colorGradient50px"} {onClick="buttonPressed(this); OnInbox();"} {
            #puts "Posteingang"
            puts "\${dialogNewDevicesfooterBtnDeviceInputA}"
            br
            #puts "([get_new_device_count] neue Ger&auml;te)"
            puts "([get_new_device_count] \${dialogNewDevicesfooterBtnDeviceInputB})"
          }
        }
        table_data {class="CLASS21206"} {}
      }
    }
  }
  puts ""
  cgi_javascript {

    puts "var url = \"$env(SCRIPT_NAME)?sid=\" + SessionId;"
    puts "var installTimerHmIP = 0;"
    puts {
      // Input of the teach-in dialog: allowed characters, normalization and
      // validation of the HmIP SGTIN/KEY and the BidCos-RF serial number
      /**
       * SGTIN and KEY of a HmIP device as the local teach-in expects them: the
       * SGTIN as 24 hex digits, the KEY as 32 hex digits or as 26 digits of the
       * eQ-3 Base32 alphabet (HMIP_KEY_CHARS of convertHmIPKeyBase32ToBase16 in ic_common.js).
       * 26 Base32 digits carry 130 bits, so the first digit of a 128 bit key is 0-7.
       */
      HMIP_SGTIN_LENGTH = 24;
      HMIP_KEY_HEX_LENGTH = 32;
      HMIP_KEY_BASE32_LENGTH = 26;
      HMIP_HEX_DIGITS = /^[0-9A-F]+$/;
      HMIP_KEY_BASE32_DIGITS = /^[0-7][0-9A-CE-HJ-NP-UW-Z]*$/;

      /**
       * Serial number of a BidCos-RF device as the teach-in by serial number
       * expects it: 10 upper case letters and digits (the CONFIG_SERIAL field of
       * the rfd frame has a fixed length of 10 bytes)
       */
      BIDCOS_SERIAL_LENGTH = 10;
      BIDCOS_SERIAL_CHARS = /^[0-9A-Z]+$/;

      /**
       * Converts to upper case and removes everything but digits and letters, e.g.
       * hyphens, blanks and line breaks of a value copied from a label or OCR scan
       * @param value
       */
      normalizeTeachInValue = function(value) {
        // strip first: toUpperCase() turns some letters into A-Z (e.g. "\u00df" into "SS")
        return String(value).replace(/[^0-9A-Za-z]/g, "").toUpperCase();
      };

      /**
       * Input fields of the teach-in dialog: characters that cannot be typed
       * (the KEY field allows hex and Base32; eQ-3 Base32 has no D, I, O, V,
       * hex needs the D) and the groups the value is shown in, separated by
       * TEACHIN_GROUP_SEPARATOR, like on the device label. The KEY is shown
       * like the label up to its Base32 length, longer only as hex (e.g. the
       * KEY of the QR code). Length and exact format are checked on submit.
       */
      TEACHIN_GROUP_SEPARATOR = "-";
      TEACHIN_GROUPS_HMIP_SGTIN = [4, 4, 4, 4, 4, 4];
      TEACHIN_GROUPS_HMIP_KEY_BASE32 = [5, 5, 5, 5, 6];
      TEACHIN_GROUPS_HMIP_KEY_HEX = [4, 4, 4, 4, 4, 4, 4, 4];
      TEACHIN_FIELD_HMIP_SGTIN = {
        invalidChars: /[^0-9A-F]/g,
        groups: function(length) { return TEACHIN_GROUPS_HMIP_SGTIN; },
        qrCode: true
      };
      TEACHIN_FIELD_HMIP_KEY = {
        invalidChars: /[^0-9A-HJ-NP-UW-Z]/g,
        groups: function(length) {
          return (length > HMIP_KEY_BASE32_LENGTH) ? TEACHIN_GROUPS_HMIP_KEY_HEX : TEACHIN_GROUPS_HMIP_KEY_BASE32;
        },
        qrCode: true
      };
      TEACHIN_FIELD_BIDCOS_SERIAL = {
        invalidChars: /[^0-9A-Z]/g,
        groups: null,
        qrCode: false
      };

      /**
       * Letters typed for the digit they look like. Replaced only in a field
       * where the letter itself is invalid (SGTIN and KEY), so a BidCos serial
       * number keeps its O and I.
       */
      TEACHIN_LOOKALIKE_DIGITS = {'O': '0', 'I': '1'};
      TEACHIN_LOOKALIKE_LETTERS = new RegExp("[" + Object.keys(TEACHIN_LOOKALIKE_DIGITS).join("") + "]", "g");

      /**
       * Content of the QR code of a HmIP device label (after
       * normalizeTeachInValue): "EQ01", "SG" + SGTIN, "DLK" + KEY as hex
       */
      HMIP_QR_CODE = /^EQ01SG([0-9A-F]{24})DLK([0-9A-F]{32})$/;
      // the same as template, "H" for a hex digit (to recognize a scan in progress)
      HMIP_QR_CODE_TEMPLATE = "EQ01SG" + "H".repeat(HMIP_SGTIN_LENGTH) + "DLK" + "H".repeat(HMIP_KEY_HEX_LENGTH);
      HMIP_TEACHIN_SGTIN_ID = "serialHmIPLocal_";
      HMIP_TEACHIN_KEY_ID = "keyHmIPLocal_";

      /**
       * Returns the value without invalid characters of the field, in upper
       * case; an invalid O or I becomes 0 or 1 (TEACHIN_LOOKALIKE_DIGITS)
       * @param value
       * @param field one of the TEACHIN_FIELD_* descriptions
       */
      cleanTeachInValue = function(value, field) {
        var isInvalid = function(c) { return c.replace(field.invalidChars, "") == ""; };
        return normalizeTeachInValue(value)
          .replace(TEACHIN_LOOKALIKE_LETTERS, function(c) { return isInvalid(c) ? TEACHIN_LOOKALIKE_DIGITS[c] : c; })
          .replace(field.invalidChars, "");
      };

      /**
       * Returns the cleaned value in the groups of the field; characters beyond
       * the last group stay in it (the length is checked on submit)
       * @param cleaned value of cleanTeachInValue()
       * @param field one of the TEACHIN_FIELD_* descriptions
       */
      formatTeachInValue = function(cleaned, field) {
        var groups = field.groups ? field.groups(cleaned.length) : [],
          parts = [],
          pos = 0;
        for (var i = 0; (i < groups.length - 1) && (pos + groups[i] < cleaned.length); i++) {
          parts.push(cleaned.substr(pos, groups[i]));
          pos += groups[i];
        }
        parts.push(cleaned.substr(pos));
        return parts.join(TEACHIN_GROUP_SEPARATOR);
      };

      /**
       * Position in the formatted value behind its count-th character
       * (separators not counted)
       */
      teachInCaretPosition = function(formatted, count) {
        var pos = 0;
        while ((count > 0) && (pos < formatted.length)) {
          if (formatted.charAt(pos) != TEACHIN_GROUP_SEPARATOR) {
            count--;
          }
          pos++;
        }
        return pos;
      };

      /**
       * True while the value can be the beginning of the QR code content, e.g.
       * while a scanner with keyboard output types it character by character
       * @param value value of normalizeTeachInValue()
       */
      isHmIPQrCodePrefix = function(value) {
        if ((value.length == 0) || (value.length > HMIP_QR_CODE_TEMPLATE.length)) {
          return false;
        }
        for (var i = 0; i < value.length; i++) {
          var expected = HMIP_QR_CODE_TEMPLATE.charAt(i);
          if ((expected == "H") ? !HMIP_HEX_DIGITS.test(value.charAt(i)) : (expected != value.charAt(i))) {
            return false;
          }
        }
        return true;
      };

      /**
       * If the value is the content of the QR code of a HmIP label, fills
       * the SGTIN and KEY field of the interface and returns true
       * @param input SGTIN or KEY input element, id HMIP_TEACHIN_*_ID + interface
       */
      fillHmIPTeachInFromQrCode = function(input) {
        var qrCode = HMIP_QR_CODE.exec(normalizeTeachInValue(input.value)),
          iFace = input.id.substr(input.id.indexOf("_") + 1);
        if (qrCode == null) {
          return false;
        }
        var sgtinInput = document.getElementById(HMIP_TEACHIN_SGTIN_ID + iFace),
          keyInput = document.getElementById(HMIP_TEACHIN_KEY_ID + iFace);
        sgtinInput.value = formatTeachInValue(qrCode[1], TEACHIN_FIELD_HMIP_SGTIN);
        keyInput.value = formatTeachInValue(qrCode[2], TEACHIN_FIELD_HMIP_KEY);
        sgtinInput.teachInCleaned = qrCode[1];
        keyInput.teachInCleaned = qrCode[2];
        input.setSelectionRange(input.value.length, input.value.length);
        return true;
      };

      /**
       * oninput handler of the teach-in fields: removes invalid characters
       * (TEACHIN_FIELD_*), converts to upper case and shows the value in the
       * groups of the label, keeping the caret behind the same character, so
       * typing and pasting work alike. A deleted separator deletes the
       * character next to it, otherwise it would only reappear. Pasting the
       * QR code content into the SGTIN or KEY field fills both, as does
       * a scanner typing it into the empty field.
       * @param input input element
       * @param field one of the TEACHIN_FIELD_* descriptions
       * @param event input event (for inputType), may be missing
       */
      filterTeachInInput = function(input, field, event) {
        if (field.qrCode && fillHmIPTeachInFromQrCode(input)) {
          return;
        }
        var normalized = normalizeTeachInValue(input.value);
        if (field.qrCode && isHmIPQrCodePrefix(normalized)) {
          // QR code content typed by a scanner, not complete yet: filtering now
          // would drop its letters, so it could never be recognized
          input.value = normalized;
          input.teachInCleaned = undefined;
          return;
        }
        var value = input.value,
          inputType = event ? event.inputType : "",
          cleaned = cleanTeachInValue(value, field),
          cleanCaret = cleanTeachInValue(value.slice(0, input.selectionStart), field).length;

        if ((cleaned === input.teachInCleaned) &&
            ((inputType == "deleteContentBackward") || (inputType == "deleteContentForward"))) {
          var at = (inputType == "deleteContentBackward") ? cleanCaret - 1 : cleanCaret;
          if ((at >= 0) && (at < cleaned.length)) {
            cleaned = cleaned.slice(0, at) + cleaned.slice(at + 1);
            cleanCaret = at;
          }
        }
        input.teachInCleaned = cleaned;

        var formatted = formatTeachInValue(cleaned, field);
        if (formatted != value) {
          var newCaret = teachInCaretPosition(formatted, cleanCaret);
          input.value = formatted;
          input.setSelectionRange(newCaret, newCaret);
        }
      };

      /**
       * Returns the serial number of a BidCos-RF device or null if it is invalid
       * @param value
       */
      getBidCosTeachInSerial = function(value) {
        var serial = normalizeTeachInValue(value);
        return (serial.length == BIDCOS_SERIAL_LENGTH && BIDCOS_SERIAL_CHARS.test(serial)) ? serial : null;
      };

      /**
       * Returns the SGTIN as 24 hex digits or null if the value is no valid SGTIN
       * @param value
       */
      getHmIPTeachInSgtin = function(value) {
        var sgtin = normalizeTeachInValue(value);
        return (sgtin.length == HMIP_SGTIN_LENGTH && HMIP_HEX_DIGITS.test(sgtin)) ? sgtin : null;
      };

      /**
       * Returns the KEY as 32 hex digits or null if the value is no valid KEY
       * @param value KEY as 32 hex digits or 26 eQ-3 Base32 digits
       */
      getHmIPTeachInKey = function(value) {
        var key = normalizeTeachInValue(value);
        if (key.length == HMIP_KEY_HEX_LENGTH && HMIP_HEX_DIGITS.test(key)) {
          return key;
        }
        if (key.length == HMIP_KEY_BASE32_LENGTH && HMIP_KEY_BASE32_DIGITS.test(key)) {
          return convertHmIPKeyBase32ToBase16(key);
        }
        return null;
      };
    }
    puts {
      update_new_device_count = function() {
        var pb = "action=get_new_device_count";
        var opts = {
          postBody: pb,
          sendXML: false,
          onSuccess: function(transport) {
            var devcount=transport.responseText.replace(/[^0-9]/g, "");
            for(var i=0;i<document.getElementById("cp_new_devices_button").childNodes.length;i++){
              var oldval=document.getElementById("cp_new_devices_button").childNodes[i].nodeValue;
              if(oldval)document.getElementById("cp_new_devices_button").childNodes[i].nodeValue=oldval.replace(/\d+/g, devcount);
            }
          }
        };
        new Ajax.Request(url, opts);
      }
    }
    puts {
      install_time_remain=0;

      buttonPressed = function(elem) {
        var jElem = jQuery(elem),
        classBtnPressed = "borderWidth2Px"
        jElem.addClass(classBtnPressed);
        var loop = 0;
        var detectChange = setInterval(function() {
          loop++;
          if (jElem.hasClass(classBtnPressed) || loop >= 20) {
            window.setTimeout(function() {jElem.removeClass("borderWidth2Px");},500);
            clearInterval(detectChange);
          }
        }, 10);

      }

      update_time_bar = function() {
        install_time_remain-=1;
        if((install_time_remain%5)==0){
          var pb = "action=get_install_status";
          var opts = {
            postBody: pb,
            sendXML: false,
            onSuccess: function(transport) {
              if (!transport.responseText.match(/^Success/g)){
                cp_adddev_updater.stop();
                return;
              }
              var result=transport.responseText.split(" ");
              var serial="";
              if(result.length >= 3)serial=result[2];
              if(serial != ""){
                cp_adddev_updater.stop();
                dlgPopup.hide();
                dlgPopup.setWidth(400);
                dlgPopup.LoadFromFile(url, "action=put_key_dialog&serial="+serial);
              }else{
                install_time_remain=result[1];
                if(install_time_remain<=0){
                  if ($("time_bar")) { document.getElementById("time_bar").firstChild.nodeValue= translateKey("dialogNewDevicesBidCosRFFetchmodeNotActive"); } /*"Anlernmodus nicht aktiv"*/
                  cp_adddev_updater.stop();
                  update_new_device_count();
                  install_time_remain=0;
                }
              }
            }
          };
          new Ajax.Request(url, opts);
        }
        if(install_time_remain<=0){
          if ($("time_bar")) { $("time_bar").firstChild.nodeValue= translateKey("dialogNewDevicesBidCosRFFetchmodeNotActive");} /*"Anlernmodus nicht aktiv"*/
          cp_adddev_updater.stop();
          update_new_device_count();
          install_time_remain=0;
        }else{
          //if ($("time_bar")) { $("time_bar").firstChild.nodeValue="Anlernmodus noch "+String(install_time_remain)+"s aktiv"; }
          if ($("time_bar")) { $("time_bar").firstChild.nodeValue=translateKey("dialogNewDevicesBidCosRFFetchmodeActiveA")+String(install_time_remain)+translateKey("dialogNewDevicesBidCosRFFetchmodeActiveB"); }
        }
      }


      getInstallModeTimer = function(interface, mode) {
        var remainingTimeElm = (mode == "local") ? jQuery("#teachCounterLocal_" + interface) : jQuery("#teachCounter_" + interface),
        iFace = interface;
        window.setTimeout(function() {
          homematic ('Interface.getInstallMode', {'interface': iFace}, function(result) {
            if (result >= 1) {
              remainingTimeElm.text(translateKey("dialogNewDevicesBidCosRFFetchmodeActiveA") + result + translateKey("dialogNewDevicesBidCosRFFetchmodeActiveB"));
              getInstallModeTimer(iFace, mode);
              if (result == 40 || result == 30 || result == 20) {
                update_new_device_count();
              }
            } else {
              remainingTimeElm.text(translateKey("dialogNewDevicesBidCosRFFetchmodeNotActive"));
              update_new_device_count();
            }
            installTimerHmIP = result;
          });
        }, 400);
      };

      hmip_install_mode_stop = function() {
         var iFace = 'HmIP-RF'
         if(jQuery("body").data("HmIP-RF")) {
          homematic('Interface.setInstallModeHMIP',{
            'installMode' : 'STOP',
            'interface': iFace,
            'on': 'false',
            'time' : 0,
            'address': '',
            'key' : '',
            'keymode' : ''
            }
          );
        }
      };

      // SGTIN and KEY of the local teach-in, or null after a message if one is invalid
      hmip_local_teachin_values = function(iFace) {
        var devAddress = getHmIPTeachInSgtin(jQuery("#serialHmIPLocal_" + iFace).val()),
         devKey = getHmIPTeachInKey(jQuery("#keyHmIPLocal_" + iFace).val());

        if (devAddress == null) {
          alert(translateKey("dialogNewDevicesErrorHmIPInvalidSgtin"));
          return null;
        }
        if (devKey == null) {
          alert(translateKey("dialogNewDevicesErrorHmIPInvalidKey"));
          return null;
        }
        return {'address': devAddress, 'key': devKey};
      };

      // localValues: SGTIN and KEY of hmip_local_teachin_values() for mode "local"
      hmip_install_mode_start = function(interface, mode, localValues) {
        var devAddress = "",
         devKey = "",
         devKeyMode = "",
         installTime = 60;

         var iFace = interface;

        if (mode == "local") {
          devAddress = localValues.address;
          devKey = localValues.key;
          devKeyMode = 'LOCAL';

          homematic('Interface.setInstallModeHMIP',{
            'installMode' : 'LOCAL',
            'interface': iFace,
            'on': 'true',
            'time' : installTime,
            'address': devAddress,
            'key' : devKey,
            'keymode' : devKeyMode
            }
          );
        } else {
          homematic('Interface.setInstallModeHMIP',{
            'installMode' : 'ALL',
            'interface': iFace,
            'on': 'true',
            'time' : installTime,
            'address': "",
            'key' : "",
            'keymode' : ""
          });
        }
        getInstallModeTimer(iFace, mode);
      };

      hmip_install_mode = function(interface, mode) {

        var iFace = interface,
          localValues = null;

        // SGTIN and KEY are checked once, before anything is stopped; the
        // start uses these values, not the fields when the STOP returns
        if (mode == "local") {
          localValues = hmip_local_teachin_values(iFace);
          if (localValues == null) {
            return;
          }
        }

        if(jQuery("body").data("BidCos-RF")) {
          rf_install_mode(false);
        }
        if (installTimerHmIP > 0) {
          // Stop a running hmip install mode so the timer is set to 0
          homematic('Interface.setInstallModeHMIP',{
            'installMode' : 'STOP',
            'interface': iFace,
            'on': 'false',
            'time' : 0,
            'address': '',
            'key' : '',
            'keymode' : ''
            }, function() {
              hmip_install_mode_start(iFace, mode, localValues);
            }
          );
        } else {
          hmip_install_mode_start(iFace, mode, localValues);
        }
      };

      rf_install_mode = function(activate) {
        if (activate) {
          hmip_install_mode_stop();
        }
        var pb = "action=rf_install_mode";
        pb += "&activate="+activate;
        if(!activate && cp_adddev_updater)cp_adddev_updater.stop();
        var opts = {
          postBody: pb,
          sendXML: false,
          onSuccess: function(transport) {
            if (!transport.responseText.match(/^Success/g)){
              //alert("BidCoS-RF Anlernmodus konnte nicht aktiviert werden.");
              alert(translateKey("dialogNewDevicesError4"));
            }
            result=transport.responseText.split(" ");
            install_time_remain=result[1];
            if(install_time_remain > 0){  
              install_time_remain++;
              if(!cp_adddev_updater){
                cp_adddev_updater=new PeriodicalExecuter(update_time_bar, 1);
              }else if(!cp_adddev_updater.timer){
                cp_adddev_updater.registerCallback();
              }
            }else{
              install_time_remain=0;
            }
            update_time_bar();
          },
          onFailure: function(transport) {
            //alert("BidCoS-RF Anlernmodus konnte nicht aktiviert werden.");
            alert(translateKey("dialogNewDevicesError4"));
          }
        };
        new Ajax.Request(url, opts);
      }
    }
    puts {
      rf_serial = function() {
        var serial = getBidCosTeachInSerial(document.getElementById("text_serial").value);
        if (serial == null) {
          alert(translateKey("dialogNewDevicesErrorBidCosInvalidSerial"));
          return;
        }
        hmip_install_mode_stop();
        var pb = "action=rf_serial&";
        pb += "serial="+encodeURIComponent(serial);
        Cursor.set(Cursor.WAIT);
        var opts = {
          postBody: pb,
          sendXML: false,
          onSuccess: function(transport) {
            Cursor.set(Cursor.NORMAL);
            if (transport.responseText.match(/^Success/g)){
              update_new_device_count();
              document.getElementById("text_serial").value="";
            } else if (transport.responseText.match(/^KeyMismatch/g)){
              rf_install_mode(false);
              if(cp_addev_updater)cp_adddev_updater.stop();
              dlgPopup.hide();
              dlgPopup.setWidth(400);
              dlgPopup.LoadFromFile(url, "action=put_key_dialog&serial="+encodeURIComponent(serial)+"&with_serial=1");
            } else {
              //alert("BidCoS-RF Anlernen mit Seriennummer "+document.getElementById("text_serial").value+ unescape(" fehlgeschlagen. Bitte %FCberpr%FCfen Sie die Seriennummer."));
              alert(translateKey("dialogNewDevicesError1a")+serial+ translateKey("dialogNewDevicesError1b"));
            }
          },
          onFailure: function(transport) {
            Cursor.set(Cursor.NORMAL); 
          }
        };
        new Ajax.Request(url, opts);
      }
    }
    puts {
      wir_search = function() {
        var pb = "action=wir_search";
        CreateCPPopup2(url, "action=wait_page_wir_search");
        var opts = {
          postBody: pb,
          sendXML: false,
          onSuccess: function(transport) {
            Popup2Close();
            if (transport.responseText.match(/^Success/g)){
              update_new_device_count();
            } else {
              //alert(unescape("BidCoS-Wired Ger%E4te suchen fehlgeschlagen."));
              alert(translateKey("dialogNewDevicesError2"));
            }
          },
          onFailure: function(transport) {
            Popup2Close();
          }
        };
        new Ajax.Request(url, opts);
      }
    }
    puts {
      OnInbox = function() {
        if (jQuery("body").data("BidCos-RF")) {
          rf_install_mode(false);
        }
        if (jQuery("body").data("HmIP-RF")) {
          hmip_install_mode_stop();
        }
        PopupClose();
        WebUI.enter(NewDeviceListPage, {"fromTeachIn":true});
      }
      OnBack = function() {
        if (jQuery("body").data("BidCos-RF")) {
          rf_install_mode(false);
        }
        if (jQuery("body").data("HmIP-RF")) {
          hmip_install_mode_stop();
        }
        PopupClose();
      }
    }
    catch { 
      set install_time_remain [xmlrpc $RFD_URL getInstallMode] 
      if { $install_time_remain } {
        puts "install_time_remain=$install_time_remain;"
        puts {
          if(!cp_adddev_updater)cp_adddev_updater=new PeriodicalExecuter(update_time_bar, 1);
          else if(!cp_adddev_updater.timer)cp_adddev_updater.registerCallback();
          update_time_bar();
        }
      }        
    }
    # Teach-in by serial number repeated after the security key dialog
    # (serial=...&call_js=rf_serial()): put the serial number back into its
    # field, only if it is a valid one (BIDCOS_SERIAL_LENGTH letters and digits)
    if {[regexp {^[A-Z0-9]{10}$} $serial]} {
      puts "jQuery('#text_serial').val('$serial');"
    }
    catch {
      import call_js
      puts "$call_js;"
    }

    puts "translatePage('#messagebox');"

    puts {
      // Tooltip of the help icon next to the HmIP KEY field (local teach-in).
      // Only after translatePage(): it rewrites the cells of the dialog and so
      // replaces the elements, the tooltip of an element set before is lost.
      jQuery(".j_hmIPTeachInQrCodeHelp").each(function() {
        // #powerTip does not wrap (white-space: nowrap), the text needs a width
        jQuery(this).data('powertip', "<div style='white-space: normal; width: 350px;'>" + translateKey("tooltipHmIPTeachInQrCode") + "</div>");
        jQuery(this).powerTip({placement: 'sw', followMouse: false});
      });
    }

    puts "dlgPopup.readaptSize();"

  }
}



proc action_put_key_dialog {} {
  global env
  
  import serial
  set with_serial 0
  catch { import with_serial }
  
  division {class="popupTitle"} {
    #puts "Ger&auml;te anlernen - Sicherheitsabfrage"
    puts "\${dialogNewDevicesErrorSecKeyTitle}"
  }
  division {class="CLASS21200"} {
    table {class="popupTable"} {border="1"} {
      table_row {class="CLASS21202"} {
        table_data {align="left"} {class="CLASS21203"} {
          #puts "Sie haben gerade versucht, das Ger&auml;t $serial"
          puts "\${dialogNewDevicesErrorSecKeyLbl1} $serial"
          if { $with_serial } {
            # durch Eingabe der Seriennummer
            puts "\${dialogNewDevicesErrorSecKeyLbl1a}"
          } else {
            # im Anlernmodus
            puts "\${dialogNewDevicesErrorSecKeyLbl1b}"
          }
          # anzulernen. Dieser Vorgang konnte nicht durchgef&uuml;hrt werden.
          puts "\${dialogNewDevicesErrorSecKeyLbl1c}"

          br
          #puts "Wahrscheinlich ist diesem Ger&auml;t ein dem System unbekannter System-Sicherheitsschl&uuml;ssel"
          #puts "zugeordnet. Bitte geben Sie den zum Ger&auml;t geh&ouml;renden System-Sicherheitsschl&uuml;ssel ein und"
          #puts "starten den Anlernvorgang erneut."

          puts "\${dialogNewDevicesErrorSecKeyLbl1d}"
          puts "\${dialogNewDevicesErrorSecKeyLbl1e}"
          puts "\${dialogNewDevicesErrorSecKeyLbl1f}"

          table {class="CLASS21204"} {width="100%"} {
            table_row {
              td {width="10"} {}
              #td "System-Sicherheitsschl&uuml;ssel:"
              td "\${dialogNewDevicesErrorSecKeyLbl1g}"
              table_data {
                cgi_text aes_key= {id="text_aes_key"} {width="50"} {type="password"}
              }
            }
          }
        }
      }
    }
  }
  division {class="popupControls"} {
    table {
      table_row {
        table_data {class="CLASS21209"} {
          division {class="CLASS21213"} {onClick="buttonPressed(this); cancel();"} {
            #puts "Abbrechen"
            puts "\${btnCancel}"
          }
        }
        table_data {class="CLASS21209"} {
          division {class="CLASS21214"} {onClick="buttonPressed(this); try_again();"} {
            #puts "Schl&uuml;ssel setzen und erneut versuchen"
            puts "\${dialogNewDevicesBtnSetKeyAndTryAgain}"
          }
        }
        table_data {class="CLASS21206"} {}
      }
    }
  }
  puts ""
  cgi_javascript {
    puts "var url = \"$env(SCRIPT_NAME)?sid=\" + SessionId;"
    
    puts {
      try_again = function() {
        var pb = "action=set_temp_key&";
        pb += "key="+encodeURIComponent(document.getElementById("text_aes_key").value.trim());
        var opts = {
          postBody: pb,
          sendXML: false,
          onSuccess: function(transport) {
            if (transport.responseText.match(/^Success/g)){
              go_back();
            } else {
              //alert(unescape("Tempor%E4rer System-Sicherheitsschl%FCssel konnte nicht gesetzt werden."));
              alert(translateKey('dialogNewDevicesError3'));
            }
          }
        };
        new Ajax.Request(url, opts);
      }
      cancel = function() {
        dlgPopup.hide();
        dlgPopup.setWidth(800);
        dlgPopup.LoadFromFile(url);
      }
    }
    
    puts "go_back = function() \{"
    if {$with_serial} {
      puts "  var pb=\"&serial=$serial&call_js=rf_serial()\";"
    } else {
      puts "  var pb=\"&call_js=rf_install_mode(true)\";"
    }
    puts {
      dlgPopup.hide();
      dlgPopup.setWidth(800);
      dlgPopup.LoadFromFile(url, pb);
    }
    puts "\}"
    puts "translatePage('#messagebox');"
  }
}

proc action_wait_page_wir_search {} {
  division {class="popupTitle j_translate"} {
    #puts "Ger&auml;te werden gesucht ... bitte warten"
    puts "\${dialogNewDevicesLblSearchDevices}"
  }
  puts ""

   cgi_javascript {
    puts "translatePage('.popupTitle');"
   }

}

proc get_new_device_count {} {
  array set result [rega_script {
    integer iNewCount = 0;
    object obj = dom.GetObject(ID_DEVICES);
    if ( obj ) {
      string tmp = "";
      foreach( tmp, obj.EnumEnabledIDs() ) {
      object elem = dom.GetObject(tmp);
      if (elem && (elem.ReadyConfig() == false) && (elem.Name() != 'Gateway')) {
        iNewCount = iNewCount + 1;
      }
      }
    }
  }]
  return $result(iNewCount);
}

proc action_rf_install_mode {} {
  global RFD_URL
  set activate true
  catch { import activate }
  catch {
    xmlrpc $RFD_URL setInstallMode [list bool $activate]
    set time [xmlrpc $RFD_URL getInstallMode]
    puts "Success $time "
    return
  } errMsg
  puts "$errMsg"
}

proc action_get_install_status {} {
  global RFD_URL
  catch {
    set time [xmlrpc $RFD_URL getInstallMode]
    set serial [xmlrpc $RFD_URL getKeyMismatchDevice [list bool true]]
    puts "Success $time $serial "
    return
  } errMsg
  puts "$errMsg"
}

proc action_rf_serial {} {
  global RFD_URL
  if {[catch {
    import serial
    xmlrpc $RFD_URL addDevice [list string $serial]
    puts "Success"
    return
  } errMsg] == -7 } {
    puts "KeyMismatch"
  } else {
#    puts "KeyMismatch"
    puts "$errMsg"
  }
}

proc action_set_temp_key {} {
  global RFD_URL
  if {[catch {
    import key
    xmlrpc $RFD_URL setTempKey [list string $key]
    puts "Success"
    return
  } errMsg]} {
    puts "$errMsg"
  }
}

proc action_wir_search {} {
  if {[catch {
    global HS485D_URL
    xmlrpc $HS485D_URL searchDevices
    puts "Success"
    return
  } errMsg]} {
    puts "$errMsg"
  }
}

proc action_get_new_device_count {} {
  puts [get_new_device_count]
}

# Reads the "key=value" lines of a properties file into the array arrName.
proc read_properties {file arrName} {
  upvar 1 $arrName props
  if {[catch {open $file r} fd]} { return }
  while {[gets $fd line] >= 0} {
    if {[regexp {^[ \t]*([A-Za-z0-9_.]+)[ \t]*=(.*)$} $line dummy key value]} {
      set props($key) [string trim $value]
    }
  }
  close $fd
}

# Host and port of the key server the HmIP server uses: KeyServer.Gateway.URL
# and KeyServer.Gateway.Port of its configuration /var/etc/crRFD.conf and of
# the files it includes (Config.Include, e.g. /etc/config/crRFD/hmip_user.conf),
# else the eQ-3 default.
proc get_keyserver_address {} {
  global KEYSERVER_CONF KEYSERVER_HOST KEYSERVER_PORT
  read_properties $KEYSERVER_CONF props
  if {[info exists props(Config.Dir)] && [info exists props(Config.Include)]} {
    foreach file [split $props(Config.Include) ", "] {
      if {$file != ""} { read_properties [file join $props(Config.Dir) $file] props }
    }
  }
  set host $KEYSERVER_HOST
  set port $KEYSERVER_PORT
  if {[info exists props(KeyServer.Gateway.URL)]} {
    # the host of a URL, without scheme, path, user info and port
    set value $props(KeyServer.Gateway.URL)
    regsub {^[A-Za-z][A-Za-z0-9+.-]*://} $value {} value
    regsub {[/?#].*$} $value {} value
    regsub {^.*@} $value {} value
    if {[regexp {^\[([0-9A-Fa-f:.]+)\]} $value dummy value] || [regexp {^[0-9A-Fa-f.]*:[0-9A-Fa-f.]*:[0-9A-Fa-f:.]*$} $value]} {
      # IPv6 address, in brackets or without a port
      if {[regexp {^[0-9A-Fa-f:.]+$} $value]} { set host $value }
    } else {
      regsub {:.*$} $value {} value
      # no leading "-", which nc would take as an option
      if {[regexp {^[A-Za-z0-9][A-Za-z0-9.-]*$} $value]} { set host $value }
    }
  }
  if {[info exists props(KeyServer.Gateway.Port)] && [regexp {^[0-9]{1,5}$} $props(KeyServer.Gateway.Port)]} {
    scan $props(KeyServer.Gateway.Port) %d value
    if {$value > 0 && $value < 65536} { set port $value }
  }
  return [list $host $port]
}

# Prints "reachable" or "unreachable", or "failed" if nc could not be run.
proc action_check_keyserver {} {
  global env errorCode
  # a CGI may run without PATH, which auto_execok needs and which also tells
  # timeout where to find nc
  if {![info exists env(PATH)] || $env(PATH) == ""} { set env(PATH) "/bin:/sbin:/usr/bin:/usr/sbin" }
  set address [get_keyserver_address]
  set cmd [list nc -z -w 3 [lindex $address 0] [lindex $address 1]]
  # timeout also limits the name lookup, which "nc -w" does not
  if {[auto_execok timeout] != ""} { set cmd [concat [list timeout 10] $cmd] }
  if {![catch {eval exec $cmd 2>/dev/null}]} {
    puts "reachable"
  } elseif {[lindex $errorCode 0] == "CHILDSTATUS" && [lindex $errorCode 2] != 127} {
    puts "unreachable"
  } else {
    puts "failed"
  }
}

cgi_eval {
  #cgi_debug -on
  cgi_input
  catch {
    import debug
    cgi_debug -on
  }

  set action "put_page"

  http_head
  catch { import action }
  if {[session_requestisvalid 8] > 0} then action_$action
}

