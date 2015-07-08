<%@ page import="java.util.ArrayList" %>
<%@ page import="db_schema.WikiPage" %>
<%--
  Created by IntelliJ IDEA.
  User: Bogdan
  Date: 11/17/2014
  Time: 4:37 PM
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>



<%
  String username = null;

  if (session.getAttribute("isAuth") != null && ((String)session.getAttribute("isAuth")).equals("yes"))
    username = (String)session.getAttribute("authenticatedUserName");
%>

<html>
  <head>
    <link type="text/css" rel="stylesheet" href="/stylesheets/main.css"/>
    <title>Wiki</title>
  </head>
<body>
<header>
  <table>
    <tr>
      <td>
        <a href="home.jsp"><img src="images/pingU.jpg" HEIGHT="100"></a>
      </td>
      <td>
        <h1>UPB Pingu</h1>
      </td>

      <%
        if(username != null)
        {
      %>
      <td>
      <div class="auth">
        <p>Hello, <%= username %>! </p>
        <form action="/signout" method="post">
          <div><input type="submit" value="Sign Out"/></div>
        </form>
      </div>
      </td>
      <%
        }
      %>
    </tr>
  </table>
</header>


  <h1>This is the main wiki</h1><br>

  <% if (username == null) {%>
  <h2>Hello, please <a href="home.jsp">login</a> to get access.</h2>
  <%} else {%>
  <h2>Howdy, <%= username %>!</h2>
    <p>
      <b>Already existing wikis: </b><br>
      <ul>
<%
  ArrayList<WikiPage> pages = WikiPage.getAllPages();

  for(WikiPage e: pages) {
    String address = e.getAddress();
    String title = e.getTitle();
%>
  <li><a href="/wiki/<%= address %>"><%=title%></a><br></li>
<%
  }
%>
  </ul>
  </p>
<%
  }
%>

</body>
</html>
