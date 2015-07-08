<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<%@ page import="java.util.List" %>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ page import="com.google.appengine.api.datastore.DatastoreService" %>
<%@ page import="com.google.appengine.api.datastore.DatastoreServiceFactory" %>
<%@ page import="com.google.appengine.api.datastore.Entity" %>
<%@ page import="com.google.appengine.api.datastore.FetchOptions" %>
<%@ page import="com.google.appengine.api.datastore.Key" %>
<%@ page import="com.google.appengine.api.datastore.KeyFactory" %>
<%@ page import="com.google.appengine.api.datastore.Query" %>
<%@ page import="javax.servlet.http.HttpSession" %>


<html>
<head>
    <link type="text/css" rel="stylesheet" href="/stylesheets/main.css"/>
    <title>Home</title>
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
            <td>
                <div class="auth">

<%
    String username;

    if (session.getAttribute("isAuth") != null && ((String)session.getAttribute("isAuth")).equals("yes")) {
        username = (String)session.getAttribute("authenticatedUserName");
%>

<p>Hello, <%= username %>! </p>
    <form action="/signout" method="post">
        <div><input type="submit" value="Sign Out"/></div>
    </form>

<%
} else {
%>

<form action="/auth" method="post">
    <table>
        <tr>
            <div><td>Username: </td><td><input type="text" name="username" /></td></div>
        </tr>
        <tr>
            <div><td>Password: </td><td><input type="password" name="password" /></td></div>
        </tr>
    </table>
            <div><input type="submit" value="Login"/></div>
</form>

If you do not have an account, register <a href="register.jsp">here</a>.

<%
    }
%>
            </div>
            </td>
        </tr>
    </table>
</header>
<br>
<br>
<h1>Link to <a href="wiki.jsp">wiki</a></h1>