<%@ page errorPage="/ui/error/error.jsp" %>
<%
	String userID = request.getParameter("userid");
        String deviceTypeID = request.getParameter("devicetypeid");
        String mfgID = request.getParameter("mfgid");
        String userType = request.getParameter("usertype");
%>
<html>
    <head>
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="expires" content="-1">
        <script src="../script/containers.js"></script>
        <script src="../script/jsDetails.js"></script>
        <title>User Frameset</title>
        <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
    </head>
  <frameset rows="100,*,0" frameborder="NO" border="0" framespacing="0" cols="*" onunload="saveOnExit()">
    <frame name="topFrame"
        src="user_menu.jsp?userid=<%=userID%>&stylesheet=detailsmenu.xslt&mfgid=<%=mfgID%>&devicetypeid=<%=deviceTypeID%>&usertype=<%=userType%>">
    <frame name="mainFrame"
        scrolling="YES"
        noresize
        src="user_details.jsp?userid=<%=userID%>&stylesheet=details.xslt&refpropertygroupid=General&mfgid=<%=mfgID%>&devicetypeid=<%=deviceTypeID%>&usertype=<%=userType%>">
  </frameset>
  <noframes>
    <body bgcolor="#FFFFFF" text="#000000">
    </body>
  </noframes>
</html>