<%--
  Created by IntelliJ IDEA.
  User: Bogdan
  Date: 11/17/2014
  Time: 4:37 PM
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<html>
<head>
    <link type="text/css" rel="stylesheet" href="/stylesheets/main.css"/>
    <title>Create Wiki</title>
</head>

<%
    String username = null;

    if (session.getAttribute("isAuth") != null && ((String)session.getAttribute("isAuth")).equals("yes"))
        username = (String)session.getAttribute("authenticatedUserName");
%>



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



<p>Hello! Please insert the HTML code for the new wiki page. </p>


<form action="/wiki" method="post">
  <table>
    <tr>
      <div><td>Page Domain: </td><td><input type="text" name="address" /></td></div>
    </tr>
    <tr>
      <div><td>Title: </td><td><input type="text" name="title" /></td></div>
    </tr>
    <tr>
      <div><td>Body: </td><td><textarea name="body" cols="50" rows="5">
                                  Enter some html...
                                </textarea>
                    </td></div>
    </tr>
  </table>
  <div><input type="submit" value="Send"/></div>
</form>

</body>
</html>
