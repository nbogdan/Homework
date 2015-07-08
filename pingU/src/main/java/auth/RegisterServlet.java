package auth;

import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import db_schema.Account;


import java.io.IOException;

import javax.servlet.http.HttpSession;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import java.util.Properties;
import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.AddressException;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;

public class RegisterServlet extends HttpServlet {
    @Override
    public void doPost(HttpServletRequest req, HttpServletResponse resp)
        throws IOException {

        HttpSession session = req.getSession();
        String username = (String)req.getParameter("username");
        String password = (String)req.getParameter("password");
        String email = (String)req.getParameter("email");
        Account newAccount = new Account(username, password, email);

        newAccount.writeInDb();

        Properties props = new Properties();
        Session s = Session.getDefaultInstance(props, null);

        String msgBody = "Your pingU App account has been activated.\n" +
                            "Access http:://upbpingu.appspot.com/home.jsp. ";

        try {
            Message msg = new MimeMessage(s);
            msg.setFrom(new InternetAddress("thepinguapp@gmail.com", "pingU App"));
            msg.addRecipient(Message.RecipientType.TO,
             new InternetAddress(email, username));
            msg.setSubject("Activation");
            msg.setText(msgBody);
            Transport.send(msg);

        } catch (AddressException e) {

        } catch (MessagingException e) {

        }

        resp.sendRedirect("/home.jsp");
    }
}