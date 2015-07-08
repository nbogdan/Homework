package db_schema;

import com.google.appengine.api.datastore.*;
import com.google.appengine.repackaged.com.google.common.base.Flag;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Bogdan on 11/17/2014.
 */
public class WikiPage {
    String address, title;
    Text body;
    Key key;
    public static final String TYPE = "WikiPage";

    public WikiPage(String title, String address, Text body) {
        this.title = title;
        this.address = address;
        this.body = body;
        this.key = null;
    }

    private WikiPage(String title, String address, Text body, Key key) {
        this.title = title;
        this.address = address;
        this.body = body;
        this.key = key;
    }

    public Entity toEntity() {
        key = KeyFactory.createKey("WikiPage", address);
        Entity result = new Entity(TYPE, key);

        result.setProperty("title", title);
        result.setProperty("address", address);
        result.setProperty("body", body);

        return result;
    }

    public void writeInDb() {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();

        WikiPage oldWiki = WikiPage.fetchPage(address);
        if(oldWiki != null) {
            oldWiki.deleteFromDb();
        }

        datastore.put(toEntity());
    }

    public void deleteFromDb() {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();

        if(key != null) {
            datastore.delete(key);
        }
    }

    public static WikiPage fetchPage(String address) {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        Key addressKey = KeyFactory.createKey(TYPE, address);

        Query query = new Query(TYPE, addressKey);
        List<Entity> results = datastore.prepare(query).asList(FetchOptions.Builder.withLimit(5));

        if (results.isEmpty()) {
            return null;
        }

        return new WikiPage((String)results.get(0).getProperty("title"),
                (String)results.get(0).getProperty("address"),
                (Text)results.get(0).getProperty("body"),
                results.get(0).getKey()
        );
    }

    public static ArrayList<WikiPage> getAllPages() {
        ArrayList<WikiPage> pages = new ArrayList<WikiPage>();
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();

        Query q = new Query(TYPE);

        PreparedQuery pq = datastore.prepare(q);

        for (Entity result : pq.asIterable()) {

            pages.add(new WikiPage((String)result.getProperty("title"),
                                    (String)result.getProperty("address"),
                                    (Text)result.getProperty("body")));
        }

        return pages;
    }

    public String getTitle() {
        return title;
    }

    public Text getBody() {
        return body;
    }

    public String getAddress() {
        return address;
    }

}
