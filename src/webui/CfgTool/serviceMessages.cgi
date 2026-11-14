#!/bin/tclsh

source [file join $env(DOCUMENT_ROOT) cgi.tcl]
source [file join $env(DOCUMENT_ROOT) once.tcl]
#sourceOnce $env(DOCUMENT_ROOT)/session.tcl
#sourceOnce $env(DOCUMENT_ROOT)/common.tcl
sourceOnce [file join $env(DOCUMENT_ROOT) api/eq3/common.tcl]
loadOnce tclrpc.so


set iface_url $env(BIDCOS_SERVICE)

proc put_page {} {
    global iface_url

    set service_messages [xmlrpc $iface_url getServiceMessages]
    set service_count [llength $service_messages]

    puts {<script type="text/javascript">}
    puts {ResetGAC();}
    puts {iseInitUpdateArrays();}

    puts {setPath("Servicemeldungen");}
    puts {WebUI.resize();}
    
   # puts {$("msgServices").style.backgroundColor = "#5C7287";}
    puts {fltSM = new iseFilter("fltSM");}
    puts {arServices = new Array();}
    puts {</script>}

    puts {<div id="SvOffset"></div>}
    
    puts {
        <div id="NoServices" style="display:none;margin-left:15px;margin-top:10px;">Es liegen zur Zeit keine Servicemeldungen vor.</div>
    }
    puts {
        <script type="text/javascript">
            document.body.style.cursor = "auto";
        </script>
        <table id="tblServices" class="filterTable tTable" border="0" cellpadding="0" cellspacing="0" style="display:none;text-align:center; margin-left:20px;width:97%;">
            <tr>
                <th style="height:44px;"><div id="name" class="pt11 Sort">Name</div></th>
                <th align="center" valign="middle"><div class="pt11 Sort">Bild</div></th>
                <th style="height:44px;"><div id="sn" class="pt11 Sort">Seriennummer</div></th>
                <th style="height:44px;"><div id="desc" class="pt11 Sort">Servicemeldung</div></th>
                <th align="center" valign="middle"><div class="pt11 Sort">Aktion</div></th>
            </tr>
    }
    foreach message $service_messages {
        set channel [lindex $message 0]
        set id [lindex $message 1]
        set value [lindex $message 2]
        
        array set chan_descr [xmlrpc $iface_url getDeviceDescription [list string $channel]]
        array set dev_descr [xmlrpc $iface_url getDeviceDescription [list string $chan_descr(PARENT)]]
        
        # use device instead of Maintenance channel
        if { "[string range $channel end-1 end]" == ":0" } {
            set ui_channel [string range $channel 0 end-2]
            set index -1
        } else {
            set ui_channel $channel
            set index $chan_descr(INDEX)
        }
        set dev_type $dev_descr(TYPE)
        
        array_clear metadata
        set name $ui_channel
        catch { 
            array set metadata [xmlrpc $iface_url getAllMetadata [list string $ui_channel]] 
            catch { set name $metadata(NAME) }
        }
        array_clear values_descr
        array set values_descr [xmlrpc $iface_url getParamsetDescription [list string $channel] [list string VALUES] ]
        array set value_descr $values_descr($id)
        
        if { "$value_descr(TYPE)" == "ENUM" } {
            set symbolic_value "=[lindex $value_descr(VALUE_LIST) $value]"
        } else {
            set symbolic_value ""
        }
        set message_text "$chan_descr(TYPE)|$id$symbolic_value"
        set writeable [expr $value_descr(OPERATIONS) & 2]
        
        set sId "$channel;$id"
        puts "<tr id='tr_$sId' style='height: 58px;'>"
        puts "<td id=\"tdn_$sId\"  style=\"text-align:center; vertical-align: middle;\" valign=\"middle\" style=\"width: 240px;background-color: #DCDCDC; border: solid 1px Black; color:Black; padding: 4px;\">$name</td>"
        puts "<td style=\"text-align:center;\" valign=\"top\" style=\"width:55px;background-color: #FFFFFF; border: solid 1px Black; color:Black; padding: 4px;\">"
        set sPicDivId "picDiv_$sId"
        puts "<div id='$sPicDivId' style='position:relative; text-align:left; width:50px; height:50px;'"
        puts " onmouseover='picDivShow(jg_250, \"$dev_type\", 250, $index, this);'"
        puts " onmouseout='picDivHide(jg_250);'"
        puts "></div>"
        puts "<script type='text/javascript'>"
        puts "var jg_0 = new jsGraphics('$sPicDivId');"
        puts "InitGD(jg_0, 50);"
        puts "Draw(jg_0, '$dev_type', 50, $index);"
        puts "</script>"
        puts "<td style=\"text-align:center;\" valign=\"middle\" style=\"width: 240px;background-color: #DCDCDC; border: solid 1px Black; color:Black; padding: 4px;\">$ui_channel</td>"
        puts "<td id=\"tdm_$sId\" valign=\"middle\" style=\"text-align:center; width: 240px;background-color: #DCDCDC; border: solid 1px Black; color:Black; padding: 4px;\">"
        puts "  <span class=\"stringtable_value\">$message_text</span>"
        puts "</td>"

        if { $writeable } {
            puts "<td valign=\"middle\" style=\"text-align:center; background-color: #FFFFFF; border: solid 1px Black; color:Black; padding: 4px;\"><input type=\"button\" value=\"Best&auml;tigen\" onclick=\"ResetMessage('$channel', '$id');\" style=\"background-color: #999999; color: White; font-weight: bold; border: solid 1px Black;\" /></td>"
        } else {
            puts "<td valign=\"middle\" style=\"text-align:center; background-color: #DCDCDC; border: solid 1px Black; color:Black; padding: 4px;\"><input type=\"button\" value=\"Best&auml;tigen\" disabled style=\"background-color: #DDDDDD; color: White; font-weight: bold; border: solid 1px Black;\" /></td>"
        }
        puts "</tr>"
    }
    puts "<table>"
    puts "<span id=\"svView\"></span>"
    puts {<script type="text/javascript">}
    puts {st_setStringTableValues();}
    puts "var serviceMessageCount = $service_count;"
    puts {
        if(serviceMessageCount){
            show("tblServices");
        }else{
            show("NoServices");
        }
        ResetMessage = function(channel,id,reload){
            configMetadata.saveProfileParam( channel, "VALUES", id, 0 );
            hide( "tr_"+channel+";"+id );
            serviceMessageCount--;
            if( serviceMessageCount == 0 )loadServiceMessages();
        }
        updateMessageView = function(newCount){
            if(newCount != serviceMessageCount)loadServiceMessages();
        }
    }

    puts {
        var s = "";
  
        s += "<table style='background-color:transparent;' cellspacing='8'>";
            s += "<tr>";
                s += "<td style='text-align:center;' valign='middle'><div class='FooterButton' onclick='WebUI.goBack();'>Zur&uuml;ck</div></td>";
            s += "</tr>";
        s += "</table>";
  
        setFooter(s);
    }
  
    puts {</script>}

}

cgi_eval {
    #cgi_debug -on
    cgi_input
    catch {
        import debug
        cgi_debug -on
    }
    
    content_type "text/html; charset=UTF-8"
    put_page

}

