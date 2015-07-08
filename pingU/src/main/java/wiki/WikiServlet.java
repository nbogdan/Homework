package wiki;

import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import com.google.appengine.api.datastore.Query;
import com.google.appengine.api.datastore.FetchOptions;
import com.google.appengine.api.datastore.Text;
import db_schema.WikiPage;

import java.io.PrintWriter;
import java.util.List;

import java.io.IOException;

import javax.servlet.http.HttpSession;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class WikiServlet extends HttpServlet {
    @Override
    public void doGet(HttpServletRequest req, HttpServletResponse resp)
            throws IOException {
        HttpSession session = req.getSession();

        if(req.getRequestURI().equals("/wiki") || req.getRequestURI().equals("/wiki/")) {
            resp.sendRedirect("/wiki.jsp");//just for the moment
            return;
        }

        String address = req.getRequestURI().substring(6);
        WikiPage wikiPage = WikiPage.fetchPage(address);

        if(wikiPage == null) {
            resp.sendRedirect("/create.jsp");
            return;
        }

        PrintWriter pw = resp.getWriter();

        String writePage = String.format("<!DOCTYPE html>\n" +
                "<html>\n" +
                "<head>\n" +
                    "<title>%s</title>\n" +
                    "<link type=\"text/css\" rel=\"stylesheet\" href=\"/stylesheets/main.css\"/>\n" +
                "</head>\n" +
                "<body>\n" +
                "<header>\n" +
                        "    <table>\n" +
                        "        <tr>\n" +
                        "            <td>\n" +
                        "                <a href=\"home.jsp\"><img src=\"/images/pingU.jpg\" HEIGHT=\"100\"></a>\n" +
                        "            </td>\n" +
                        "            <td>\n" +
                        "                <h1>UPB Pingu</h1>\n" +
                        "            </td>\n" +
                        "           <td>\n" +
                        "               <div class=\"auth\">\n" +
                        "               <p>Hello, " + session.getAttribute("authenticatedUserName") + " </p>\n" +
                        "               <form action=\"/signout\" method=\"post\">\n" +
                        "                   <div><input type=\"submit\" value=\"Sign Out\"/></div>\n" +
                        "               </form>\n" +
                        "               </div>\n" +
                        "           </td>" +
                        "        </tr>\n" +
                        "    </table>\n" +
                "</header><br>" +
                "%s \n" +
                "<br>\n" +
                "<br>\n" +
                "<footer>Link to <a href=\"/wiki\">Main wiki</a></footer>\n" +
                "</body>\n" +
                "</html>\n", wikiPage.getTitle(), wikiPage.getBody().getValue());


        pw.println(writePage);
        //resp.sendRedirect("/wiki.jsp" + req.getRequestURI() + " - " + req.getContextPath());
    }

    @Override
    public void doPost(HttpServletRequest req, HttpServletResponse resp)
            throws IOException {

        String address = (String)req.getParameter("address");
        String title = (String)req.getParameter("title");
        Text body = new Text((String)req.getParameter("body"));
        WikiPage wikiPage = new WikiPage(title, address, body);

        wikiPage.writeInDb();

        resp.sendRedirect("/wiki/" + address);
    }
}