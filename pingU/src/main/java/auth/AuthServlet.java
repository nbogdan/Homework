package auth;

import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import com.google.appengine.api.datastore.Query;
import com.google.appengine.api.datastore.FetchOptions;
import db_schema.Account;

import java.util.List;

import java.io.IOException;

import javax.servlet.http.HttpSession;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class AuthServlet extends HttpServlet {
    @Override
    public void doPost(HttpServletRequest req, HttpServletResponse resp)
        throws IOException {
        HttpSession session = req.getSession();
        String username = (String)req.getParameter("username");
        String password = (String)req.getParameter("password");

        Account account = Account.fetchAccount(username);

        if (account == null) {
            resp.sendRedirect("/home.jsp");
        }
        else if(account.getPassword().equals(password)) {
            session.setAttribute("authenticatedUserName", username);
            session.setAttribute("isAuth", "yes");
            resp.sendRedirect("/home.jsp");
        }
        else {
            resp.sendRedirect("/home.jsp");
        }
    }
}