pingU
=====

Small project for the Human-Computer Interaction course


1. Pentru a rula local:<br>
	a.) instalati maven pe masina locala: http://maven.apache.org/download.cgi<br>
	b.) navigati in directorul /pingU<br>
	c.) rulati: mvn clean install<br>
	d.) rulati: mvn appengine:devserver<br>
	e.) accesati: localhost:8080/home.jsp<br><br>


1. Pentru a uploada pe server:<br>
	a-c)<br>
	d.) rulati: mvn appengine:update (sunt necesare credentiale de autentificare la inceput)<br>
	e.) accesati: upbpingu.appspot.com/home.jsp<br>
