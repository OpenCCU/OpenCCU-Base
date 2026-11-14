<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- ####################################################################### -->
<!-- # Dokumentrahmen                                                      # -->
<!-- ####################################################################### -->

<!-- Gibt die technische Dokumentation aus -->
<xsl:template match="device">
  <html>
    <head>
      <title>
        <xsl:text>Technische Dokumentation</xsl:text>
      </title>
      <xsl:call-template name="WriteCSS" />
    </head>
    <body>
      <h1 id="title">
        <xsl:text>Technische Dokumentation:</xsl:text>
        <div class="docu">
          <xsl:call-template name="docu_brief" />
        </div>
      </h1>
      <xsl:call-template name="WriteDeviceDocumentation" />
      <xsl:apply-templates select="channels/channel" mode="detail" />
    </body>
  </html>
</xsl:template>
 
<!-- Gibt das CSS für die HTML-Dokumentation aus --> 
<xsl:template name="WriteCSS"> 
  <style type="text/css">
    <xsl:text disable-output-escaping="yes">
      #title {
        text-align: center;
      }
                
      .th_left {
        text-align: left;
        vertical-align: top;
      }        
    </xsl:text>
  </style>
</xsl:template>

<!-- ####################################################################### -->
<!-- # Gerätebeschreibung                                                  # -->
<!-- ####################################################################### -->

<!-- Gibt die Gerätebeschreibung aus -->
<xsl:template name="WriteDeviceDocumentation">
  <h2>
    <xsl:text>Gerätebeschreibung</xsl:text>
  </h2>
  
  <table border="0">
    <xsl:call-template name="WriteAesSupport" />
    <xsl:call-template name="WriteSupportedTypes" />
  </table>
  
  <xsl:call-template name="docu_developer" />
  
  <xsl:apply-templates select="docu" /> 
  <xsl:apply-templates select="paramset" mode="device_master"/>
  <xsl:call-template name="WriteChannelsOverview" />
</xsl:template>

<!-- Gibt an, ob das Gerät AES unterstützt -->
<xsl:template name="WriteAesSupport">
  <tr>
    <th class="th_left">
      <xsl:text>AES:</xsl:text>
    </th>
    <td>
      <xsl:choose>
        <xsl:when test="@supports_aes='true'">
          <xsl:text>ja</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>nein</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </td>
  </tr>
</xsl:template>

<!-- Gibt die unterstützten Gerätetypen (Kurzbezeichnungen) aus -->
<xsl:template name="WriteSupportedTypes">
  <tr>
    <th class="th_left">
      <xsl:text>Unterstützte Geräte:</xsl:text>
    </th>
    <td>
      <xsl:for-each select="supported_types/type">
        <xsl:value-of select="@id" />
        <xsl:if test="not(position()=last())">
          <xsl:text>, </xsl:text>
        </xsl:if>
      </xsl:for-each>
    </td>
  </tr>
</xsl:template>

<!-- Gibt die Geräteparamter (Parameterset MASTER) aus -->
<xsl:template match="paramset" mode="device_master">
  <xsl:if test="@type='MASTER'">
    <h3>
      <xsl:text>Geräteparameter (MASTER)</xsl:text>
    </h3>    
    <xsl:call-template name="paramset_overview"/>
    <xsl:call-template name="paramset_detail" />
  </xsl:if>
</xsl:template>

<!-- ####################################################################### -->
<!-- # Kanalbeschreibung                                                   # -->
<!-- ####################################################################### -->

<xsl:template name="WriteChannelsOverview">
  <h3>
    <xsl:text>Kanalübersicht</xsl:text>
  </h3>
  
  <xsl:choose>
    <xsl:when test="not(count(channels/channel)=0)">
      <table border="1">
        <tr>
          <th>            
            <xsl:text>Name</xsl:text>
          </th>
          <th>            
            <xsl:text>Index</xsl:text>
          </th>
          <th>            
            <xsl:text>Kurzbeschreibung</xsl:text>
          </th>
        </tr>
        <xsl:apply-templates select="channels/channel" mode="overview" />
      </table>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>Das Gerät besitzt keine Kanäle</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="channel" mode="overview">
  <tr>
    <td>
      <xsl:value-of select="@type" />
    </td>
    <td>
      <xsl:call-template name="WriteChannelIndex" />
    </td>
    <td class="docu">
      <xsl:call-template name="docu_brief" />
    </td>
  </tr>
</xsl:template>

<!-- Gibt Detailinformationen eines Kanals an -->
<xsl:template match="channel" mode="detail">
  <h2>
    <xsl:text>Kanal </xsl:text>
    <xsl:value-of select="@type" />
  </h2>
  
  <xsl:call-template name="WriteChannelOverview" />
  <xsl:apply-templates select="paramset" mode="channel" />
</xsl:template>

<!-- Gibt einen Überblick über einen einzelnen Kanal -->
<xsl:template name="WriteChannelOverview">
  <table>
    <tr>
      <th class="th_left">
        <xsl:text>Index:</xsl:text>
      </th>
      <td>
        <xsl:call-template name="WriteChannelIndex" />
      </td>
    </tr>
    <xsl:call-template name="WriteChannelLinkRoles" />
  </table>
  
  <xsl:apply-templates select="docu" />
</xsl:template>

<!-- Gibt den Index des Kanals aus -->
<xsl:template name="WriteChannelIndex">
  <xsl:value-of select="@index" />
  <xsl:choose>
    <xsl:when test="@count=1">
      <!-- leer -->
    </xsl:when>
    <xsl:when test="@count">
      <xsl:text> bis </xsl:text>
      <xsl:value-of select="(@count+@index)-1" />
    </xsl:when>
    <xsl:otherwise>
      <xsl:text> bis ?</xsl:text>          
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- Gibt an, welche Rollen der Kanal in einer Verknüpfung einnehmen kann -->
<xsl:template name="WriteChannelLinkRoles">
  <xsl:choose>
    <xsl:when test="not(count(link_roles/target)+count(link_roles/source)=0)">
      <xsl:if test="not(count(link_roles/target)=0)">
        <tr>
          <th class="th_left">
            <xsl:text>Rollen (Empfänger):</xsl:text>        
          </th>
          <td>
            <xsl:for-each select="link_roles/target">
              <xsl:value-of select="@name" />
              <xsl:if test="not(position()=last())">
                <xsl:text>, </xsl:text>
              </xsl:if>
            </xsl:for-each>
          </td>
        </tr>
      </xsl:if>
      <xsl:if test="not(count(link_roles/source)=0)">
        <tr>
          <th class="th_left">
            <xsl:text>Rollen (Sender):</xsl:text>        
          </th>
          <td>
            <xsl:for-each select="link_roles/source">
              <xsl:value-of select="@name" />
              <xsl:if test="not(position()=last())">
                <xsl:text>, </xsl:text>
              </xsl:if>
            </xsl:for-each>
          </td>
        </tr>
      </xsl:if>
    </xsl:when>
    <xsl:otherwise>
      <th class="th_left">
        <xsl:text>Rollen:</xsl:text>        
      </th>
      <td>
        <xsl:text>nicht verknüpfbar</xsl:text>        
      </td>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="paramset" mode="channel">
  <xsl:choose>
    <xsl:when test="@type='MASTER'">
      <xsl:call-template name="WriteChannelMasterParamset" />
    </xsl:when>
    <xsl:when test="@type='VALUES'">
      <xsl:call-template name="WriteChannelValuesParamset" />
    </xsl:when>
    <xsl:when test="@type='LINK'">
      <xsl:call-template name="WriteChannelLinkParamset" />
    </xsl:when>
  </xsl:choose>
</xsl:template>

<!-- Gibt die Kanalparameter (Parameterset MASTER) aus -->
<xsl:template name="WriteChannelMasterParamset">
  <h3>
    <xsl:text>Kanalparameter (MASTER)</xsl:text>
  </h3>    
  <xsl:call-template name="paramset_overview"/>
  <xsl:call-template name="paramset_detail" />
</xsl:template>

<!-- Gibt den Kanalzustand (Parameterset VALUES) aus -->
<xsl:template name="WriteChannelValuesParamset">
  <h3>
    <xsl:text>Kanalzustand (VALUES)</xsl:text>
  </h3>    
  <xsl:call-template name="paramset_overview"/>
  <xsl:call-template name="paramset_detail" />
</xsl:template>

<!-- Gibt die Verknüpfungsparameter (Parameterset LINK) aus -->
<xsl:template name="WriteChannelLinkParamset">
  <h3>
    <xsl:text>Verknüpfungsparameter (LINK)</xsl:text>
  </h3>    
  <xsl:call-template name="paramset_overview"/>
  <xsl:call-template name="paramset_detail" />
</xsl:template>

<!-- ####################################################################### -->
<!-- # Parameterset- und Subset-Beschreibung                               # -->
<!-- ####################################################################### -->

<!-- Liefert einen Überblick über einen Parametersatz -->
<xsl:template name="paramset_overview">
  <xsl:choose>
    <xsl:when test="not(count(parameter)+count(subset)=0)">
      <table border="1">
        <tr>
          <th>
            <xsl:text>Name</xsl:text>
          </th>
          <th>
            <xsl:text>Datentyp</xsl:text>
          </th>
          <th>
            <xsl:text>Zugriff</xsl:text>
          </th>
          <th>
            <xsl:text>Kurzbeschreibung</xsl:text>
          </th>
        </tr>
        <xsl:apply-templates select="parameter" mode="overview" />
        <xsl:apply-templates select="subset" mode="overview" />
      </table>
    </xsl:when>
    <xsl:otherwise>
      <p>
        <xsl:text>Es sind keine Geräteparameter verfügbar.</xsl:text>
      </p>
    </xsl:otherwise>
  </xsl:choose>  
</xsl:template>

<!-- Liefert Detailinformationen über ein Parameterset -->
<xsl:template name="paramset_detail" >
  <xsl:apply-templates select="parameter" mode="detail" />
  <xsl:apply-templates select="subset" mode="detail" />
</xsl:template>

<!-- Liefer einen Überblick über die Parameter eines Subsets -->
<xsl:template match="subset" mode="overview">
  <xsl:variable name="ref_id" select="@ref" />
  <xsl:for-each select="/device/paramset_defs/paramset">
    <xsl:if test="$ref_id=@id">
      <xsl:apply-templates select="parameter" mode="overview" />    
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<!-- Liefer Detailinformationen über die Parameter eines Subsets -->
<xsl:template match="subset" mode="detail">
  <xsl:variable name="ref_id" select="@ref" />
  <xsl:for-each select="/device/paramset_defs/paramset">
    <xsl:if test="$ref_id=@id">
      <xsl:apply-templates select="parameter" mode="detail" />    
    </xsl:if>
  </xsl:for-each>
</xsl:template>


<!-- ####################################################################### -->
<!-- # Parameterbeschreibung                                               # -->
<!-- ####################################################################### -->

<!-- Liefert eine Übersicht über einen Parameter -->
<xsl:template match="parameter" mode="overview">
  <tr>
    <td>
      <xsl:value-of select="@id" />
    </td>
    <td>
      <xsl:value-of select="logical/@type" />
    </td>
    <td>
      <xsl:call-template name="WriteParameterAccess" />
    </td>
    <td class="docu">
      <xsl:call-template name="docu_brief" />
    </td>
  </tr>
</xsl:template>

<!-- Liefert eine Übersicht über einen Parameter -->
<xsl:template match="parameter" mode="detail">
  <h4>
    <xsl:value-of select="@id" />
  </h4>
  
  <table border="0">
    <tr>
      <th>
        <xsl:text>Zugriff</xsl:text>
      </th>
      <td>
        <xsl:call-template name="WriteParameterAccess" />
      </td>
    </tr>
    <xsl:apply-templates select="logical" mode="details"/>
  </table>
  
  <xsl:apply-templates select="docu" />
</xsl:template>

<xsl:template name="WriteParameterAccess">
  <xsl:choose>
    <xsl:when test="@operations">
      <xsl:value-of select="@operations" />
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>nicht angegeben</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- ####################################################################### -->
<!-- # Datentypen                                                          # -->
<!-- ####################################################################### -->

<!-- Liefert Details zu einem Datentyp (generisch) -->
<xsl:template match="logical" mode="details" >
  <!-- Typname -->
  <tr>
    <th class="th_left">
      <xsl:text>Datentyp:</xsl:text>
    </th>
    <td>
      <xsl:value-of select="@type" />
    </td>
  </tr>
  <!-- Minimaler Wert -->
  <xsl:if test="@min">
    <tr>
      <th class="th_left">
        <xsl:text>Minimaler Wert:</xsl:text>
      </th>
      <td>
        <xsl:value-of select="@min" />
      </td>
    </tr>
  </xsl:if>
  <!-- Maximaler Wert -->
  <xsl:if test="@max">
    <tr>
      <th class="th_left">
        <xsl:text>Maximaler Wert:</xsl:text>
      </th>
      <td>
        <xsl:value-of select="@max" />
      </td>
    </tr>
  </xsl:if>
  <!-- Standardwert -->
  <xsl:if test="@default">
    <tr>
      <th class="th_left">
        <xsl:text>Standardwert:</xsl:text>
      </th>
      <td>
        <xsl:value-of select="@default" />
      </td>
    </tr>
  </xsl:if>
  <!-- Einheit -->
  <xsl:if test="@unit">
    <tr>
      <th class="th_left">
        <xsl:text>Einheit:</xsl:text>
      </th>
      <td>
        <xsl:value-of select="@unit" />
      </td>
    </tr>
  </xsl:if>
  <!-- Spezielle Werte -->
  <xsl:if test="not(count(special_value)=0)">
    <tr>
      <th class="th_left">
        <xsl:text>Spezielle Werte:</xsl:text>
      </th>
      <td>
        <xsl:for-each select="special_value">
          <xsl:value-of select="@id" />
          <xsl:text> (</xsl:text>
          <xsl:value-of select="@value" />
          <xsl:text>)</xsl:text>
          <xsl:if test="not(position()=last())">
            <xsl:text>, </xsl:text>            
          </xsl:if>
        </xsl:for-each>
      </td>
    </tr>
  </xsl:if>
  <!-- Werteliste -->
  <xsl:if test="not(count(option)=0)">
    <tr>
      <th class="th_left">
        <xsl:text>Gültige Werte:</xsl:text>
      </th>
      <td>
        <xsl:for-each select="option">
          <xsl:value-of select="@id" />
          <xsl:if test="@default='true'">
            <xsl:text> (Standard)</xsl:text>
          </xsl:if>
          <xsl:if test="not(position()=last())">          
            <xsl:text>, </xsl:text>
          </xsl:if>
        </xsl:for-each>
      </td>
    </tr>
  </xsl:if>
</xsl:template>

<!-- ####################################################################### -->
<!-- # Dokumentation                                                       # -->
<!-- ####################################################################### -->

<xsl:template match="docu">
  <p class="docu">
    <xsl:copy-of select="*|text()" />
  </p>
</xsl:template>

<xsl:template name="docu_brief">
  <xsl:choose>
    <xsl:when test="docu/@brief">
      <xsl:value-of select="docu/@brief" />
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>-</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- Liefet Informationen über den verantwortlichen Entwickler -->
<xsl:template name="docu_developer">
  <h3>Verantwortlicher Entwickler</h3>
  <table border="0" cellspacing="0" >
   <!-- Name -->
    <tr>
      <th><xsl:text>Name:</xsl:text></th>
      <td>
        <xsl:choose>
          <xsl:when test="docu/@developer">
            <xsl:value-of select="docu/@developer" />
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>unbekannt</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </td>
    </tr>
    <!-- E-Mail -->
    <tr>
      <th><xsl:text>E-Mail:</xsl:text></th>
      <td>
        <xsl:choose>
          <xsl:when test="docu/@developer_email">
            <xsl:value-of select="docu/@developer_email" />
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>unbekannt</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </td>
    </tr>
    <!-- Telefonnummer -->
    <tr>
      <th><xsl:text>Telefon:</xsl:text></th>
      <td>
        <xsl:choose>
          <xsl:when test="docu/@developer_telephone_number">
            <xsl:value-of select="docu/@developer_telephone_number" />
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>unbekannt</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </td>
    </tr>
  </table>
</xsl:template>
  
</xsl:stylesheet>
 