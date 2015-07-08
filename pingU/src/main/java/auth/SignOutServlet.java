package auth;

import java.io.IOException;

import javax.servlet.http.HttpSession;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class SignOutServlet extends HttpServlet {
  @Override
  public void doPost(HttpServletRequest req, HttpServletResponse resp)
      throws IOException {

    HttpSession session = req.getSession();

    session.setAttribute("authenticatedUserName", null);
    session.setAttribute("isAuth", "no");

    resp.sendRedirect("/home.jsp");
    
  }
}