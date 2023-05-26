<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Edited by XMLSpy� -->
<html xsl:version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <body style="font-family:Arial;font-size:12pt;background-color:#EEEEEE">
    <xsl:for-each select="ehr/patient">
      <div style="background-color:teal;color:white;padding:4px">
        <span style="font-weight:bold"><xsl:value-of select="name"/></span>
        Age: <xsl:value-of select="age"/>
      </div>
      <div style="margin-left:20px;margin-bottom:1em;font-size:12pt">
        <p><xsl:value-of select="description"/>.
        </p>
      </div>
    </xsl:for-each>
  </body>
</html>