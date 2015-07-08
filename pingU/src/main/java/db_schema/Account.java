package db_schema;

import com.google.appengine.api.datastore.*;

import java.util.List;

/**
 * Created by Bogdan on 11/17/2014.
 */
public class Account {
    String username, password, email;
    Key key;
    public static final String TYPE = "Account";

    public Account(String username, String password, String email) {
        this.username = username;
        this.password = password;
        this.email = email;
        this.key = null;
    }

    private Account(String username, String password, String email, Key key) {
        this.username = username;
        this.password = password;
        this.email = email;
        this.key = key;
    }

    public Entity toEntity() {
        key = KeyFactory.createKey(TYPE, username);
        Entity result = new Entity("Account", key);

        result.setProperty("username", username);
        result.setProperty("password", password);
        result.setProperty("email", email);

        return result;
    }

    public void writeInDb() {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();

        Account oldAccount = Account.fetchAccount(username);
        if(oldAccount != null) {
            oldAccount.deleteFromDb();
        }

        datastore.put(toEntity());
    }

    public void deleteFromDb() {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();

        if(key != null) {
            datastore.delete(key);
        }
    }

    public static Account fetchAccount(String username) {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        Key userKey = KeyFactory.createKey(TYPE, username);

        Query query = new Query(TYPE, userKey);
        List<Entity> results = datastore.prepare(query).asList(FetchOptions.Builder.withLimit(5));

        if (results.isEmpty()) {
            return null;
        }

        return new Account((String)results.get(0).getProperty("username"),
                (String)results.get(0).getProperty("password"),
                (String)results.get(0).getProperty("email"),
                results.get(0).getKey()
                );
    }

    public String getPassword() {
        return  this.password;
    }
}
