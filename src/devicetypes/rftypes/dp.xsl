<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!--

XSL-Stylesheet zur Ausgabe der Datenpunkte.

Dieses Stylesheet liefert zu einer Gerätebeschreibung sämtliche Datenpunkte,
die man per HomeMatic Script nutzen kann.
Dabei handelt es sich um die Parameter des Paramsets VALUES der einzelnen 
Kanäle. Ausgenommen ist der Wartungskanal vom Typ MAINTENANCE, da dieser in
HomeMatic Script nicht ohne weiteres nutzbar ist.

Dokumentationstags werden von diesem Stylesheet vollständig ignoriert. Bislang
wurde die Dokumentation auch nicht besonders eingesetzt.

-->

<!-- ####################################################################### -->
<!-- # Dokumentrahmen                                                      # -->
<!-- ####################################################################### -->

<!-- Gibt die technische Dokumentation aus -->
<xsl:template match="device">
  <html>
    <head>
      <title>
        <xsl:text>Datenpunkte</xsl:text>
      </title>
      <xsl:call-template name="WriteCSS" />
    </head>
    <body>
      <h1 id="title">
        <xsl:text>Datenpunkte</xsl:text>
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
    <xsl:text>Unterstützte Geräte</xsl:text>
  </h2>
  <ul>
      <xsl:for-each select="supported_types/type">
        <li><xsl:value-of select="@id" /></li>
      </xsl:for-each>
  </ul>
</xsl:template>

<!-- Gibt die Geräteparamter (Parameterset MASTER) aus -->
<xsl:template match="paramset" mode="device_master">
  <xsl:if test="@type='MASTER'">
    <h3>
      <xsl:text>Geräteparameter (MASTER)</xsl:text>
    </h3>    
    <xsl:call-template name="paramset_overview"/>
  </xsl:if>
</xsl:template>

<!-- ####################################################################### -->
<!-- # Kanalbeschreibung                                                   # -->
<!-- ####################################################################### -->

<!-- Gibt Detailinformationen eines Kanals an -->
<xsl:template match="channel" mode="detail">
  <xsl:if test="not(@type='MAINTENANCE')">
    <h2>
      <xsl:text>Kanal </xsl:text>
      <xsl:call-template name="WriteChannelIndex" />
      <xsl:text>: </xsl:text>
      <xsl:value-of select="@type" />
    </h2>
  
    <xsl:apply-templates select="paramset" mode="channel" />
  </xsl:if>
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
    <xsl:when test="@count_from_sysinfo">
      <xsl:text> bis ?</xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>


<xsl:template match="paramset" mode="channel">
  <xsl:if test="@type='VALUES'">
    <xsl:call-template name="paramset_overview"/>
  </xsl:if>
</xsl:template>

<!-- ####################################################################### -->
<!-- # Parameterset- und Subset-Beschreibung                               # -->
<!-- ####################################################################### -->

<!-- Liefert einen Überblick über einen Parametersatz -->
<xsl:template name="paramset_overview">
  <xsl:choose>
    <xsl:when test="not(count(parameter)+count(subset)=0)">
      <table border="1" cellspacing="0" cellpadding="5">
        <tr>
          <th><xsl:text>Name</xsl:text></th>
          <th><xsl:text>Datentyp</xsl:text></th>
          <th><xsl:text>Zugriff</xsl:text></th>
          <th><xsl:text>Minimalwert</xsl:text></th>
          <th><xsl:text>Maximalwert</xsl:text></th>
          <th><xsl:text>Standardwert</xsl:text></th>
          <th><xsl:text>Einheit</xsl:text></th>
          <th><xsl:text>Spezielle Werte</xsl:text></th>
          <th><xsl:text>Werteliste</xsl:text></th>
        </tr>
        <xsl:apply-templates select="parameter" mode="overview" />
        <xsl:apply-templates select="subset" mode="overview" />
      </table>
    </xsl:when>
    <xsl:otherwise>
      <p>
        <xsl:text>Es sind keine Datenpunkte verfügbar.</xsl:text>
      </p>
    </xsl:otherwise>
  </xsl:choose>  
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

<!-- ####################################################################### -->
<!-- # Parameterbeschreibung                                               # -->
<!-- ####################################################################### -->

<!-- Liefert eine Übersicht über einen Parameter -->
<xsl:template match="parameter" mode="overview">
  <tr>
    <td><xsl:value-of select="@id" /></td>
    <td><xsl:value-of select="logical/@type" /></td>
    <td><xsl:call-template name="WriteParameterAccess" /></td>
    <!-- Minimaler Wert -->
    <td>
      <xsl:choose>
        <xsl:when test="logical/@min">
          <xsl:value-of select="logical/@min" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>&#160;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </td>
    <!-- Maximaler Wert -->
    <td>
      <xsl:choose>
        <xsl:when test="logical/@max">
          <xsl:value-of select="logical/@max" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>&#160;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </td>
    <!-- Standardwert -->
    <td>
      <xsl:choose>
        <xsl:when test="logical/@default">
          <xsl:value-of select="logical/@default" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>&#160;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </td>
    <!-- Einheit -->
    <td>
      <xsl:choose>
        <xsl:when test="logical/@unit">
          <xsl:value-of select="logical/@unit" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>&#160;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </td>
    <!-- Spezielle Werte -->
    <td>
      <xsl:choose>
        <xsl:when test="not(count(logical/special_value)=0)">
          <ul>
          <xsl:for-each select="logical/special_value">
            <li>
            <xsl:value-of select="@id" />
            <xsl:text> (</xsl:text>
            <xsl:value-of select="@value" />
            <xsl:text>)</xsl:text>
            </li>
          </xsl:for-each>
          </ul>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>&#160;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </td>
    <!-- Werteliste -->
    <td>
      <xsl:choose>
        <xsl:when test="not(count(logical/option)=0)">
          <ol start="0">
          <xsl:for-each select="logical/option">
            <li>
            <xsl:value-of select="@id" />
            <xsl:if test="@default='true'">
              <b><xsl:text> (Standard)</xsl:text></b>
            </xsl:if>
            </li>
          </xsl:for-each>
          </ol>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>&#160;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </td>
  </tr>
</xsl:template>

<xsl:template name="WriteParameterAccess">
  <xsl:choose>
    <xsl:when test="@operations">
      <xsl:value-of select="@operations" />
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>read,write</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>
  
</xsl:stylesheet>
 