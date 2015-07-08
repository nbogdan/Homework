<%@ page contentType="text/html;charset=UTF-8" language="java" %>

<html>
<head>
    <link type="text/css" rel="stylesheet" href="/stylesheets/main.css"/>
    <title>Register</title>
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
        </tr>
    </table>
</header>


<p>Hello! Please insert your information. </p>


<form action="/register" method="post">
    <table>
        <tr>
            <div><td>Username: </td><td><input type="text" name="username" /></td></div>
        </tr>
        <tr>
            <div><td>Password: </td><td><input type="password" name="password" /></td></div>
        </tr>
        <tr>
            <div><td>Email: </td><td><input type="text" name="email" /></td></div>
        </tr>
    </table>
    <div><input type="submit" value="Register"/></div>
</form>

