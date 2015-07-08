import java.util.ArrayList;
import java.util.Scanner;


/**
 * Clasa care reprezinta un punct de pe tabla de joc.
 * 
 * @author Bogdan Nicula
 *
 */
class Point {
	int xCoord;
	int	yCoord;
	
	public Point(int x, int y){
		xCoord = x;
		yCoord = y;
	}
	
	public String toString(){
		return "(" + xCoord + "," + yCoord + ") "; 
	}
	
	public boolean equals(Object o)
	{
		return (xCoord == ((Point)o).xCoord) && (yCoord == ((Point)o).yCoord);
	}
}

/**
 * Clasa care reprezinta un arc(o latura parcursa a unui patrat) in cadrul tablei de joc.
 * 
 * @author Bogdan Nicula
 *
 */

class Arc {
	Point startPoint;
	Point stopPoint;
	
	public Arc(Point a, Point b)
	{
		startPoint = a;
		stopPoint = b;
	}
	
	/**
	 * Metoda care verifica daca arcul curent contine punctul elem.
	 * @param elem
	 * @return
	 * true daca il contine, false altfel.
	 */
	public boolean containsPoint(Point elem)
	{
		return startPoint.equals(elem) || stopPoint.equals(elem);
	}
	
	public boolean equals(Object elem)
	{
		Arc o = (Arc) elem;
		if(startPoint.equals(o.startPoint) && stopPoint.equals(o.stopPoint))
			return true;
		return false;
	}
	
	public String toString(){
		return "[" + startPoint + "-" + stopPoint + "] "; 
	}
}

/**
 * Clasa care reprezinta botul.
 * 
 * In cadrul reprezentarii botului avem si reprezentarea tablei de joc definita printr-o
 * lista de puncte(punctele prin care s-a trecut pana la un moment)
 * si o lista de laturi(laturile strabatute pana la un moment).
 * 
 * @author Bogdan Nicula
 *
 */
public class Bot {

	/*
	 * Lista arcelor(laturilor strabatute).
	 */
	ArrayList<Arc> laturi;
	/*
	 * Lista punctelor plasate deja pe tabla de joc.
	 */
	ArrayList<Point> puncte;
	/*
	 * Variabila in care se retine centrul portii.
	 */
	final Point goalCenter = new Point(4, 11);
	
	/*
	 * Constructor default.
	 */
 	public Bot(){
		laturi = new ArrayList<Arc>();
		puncte = new ArrayList<Point>();
	}
	
 	/**
 	 * Metoda care calculeaza distanta euclidiana intre 2 puncte date.
 	 * @param a
 	 * @param b
 	 * @return
 	 * distanta dintre cele doua puncte
 	 */
	private double calcDist(Point a, Point b)
	{
		return Math.sqrt(Math.pow(b.xCoord - a.xCoord, 2) + Math.pow(b.yCoord - a.yCoord, 2));
	}
	
	/**
	 * Metoda care verifica daca un punct a fost marcat pe tabla.
	 * @param e
	 * @return
	 *	true daca punctul a fost deja marcat, false altfel
	 */
	private boolean pointExists(Point e)
	{
		return puncte.contains(e);
	}

	/**
	 * Metoda care verifica daca un arc(o latura) exista deja pe tabla.
	 * Se considera ca arcul (a, b) este echivalent cu (b, a).
	 * @param e
	 * @return
	 * true daca arcul exista, false in caz contrar
	 */
	private boolean arcExists(Arc e)
	{
		return laturi.contains(e) || laturi.contains(new Arc(e.stopPoint, e.startPoint));
	}
	
	/**
	 * Metoda care determina toate punctele in care se poate ajunge pornind din punctul curent,
	 * si realizand o singura mutare(orizontal, vertical sau pe diagonala).
	 * Nu face nicio modificare definitiva asupra structurii tablei de joc
	 * @param currentPoint
	 * Punct pornire.
	 * @return
	 * Lista de puncte la care se poate ajunge.
	 */
	private ArrayList<Point> possibleMoves(Point currentPoint)
	{
		ArrayList<Point> mutari = new ArrayList<Point>();
		
		/* Caz special 1: Punctul se afla pe peretele vestic. */
		if(currentPoint.xCoord == 0)
		{
			for(int i = -1; i <= 1; i++)/* incercam toate cele 3 posibilitati 
			 							   daca arcul nu exista deja, mutarea este posibila */
				if(!arcExists(new Arc(currentPoint, new Point(1, currentPoint.yCoord + i))))
					mutari.add(new Point(1, currentPoint.yCoord + i));
			return mutari;
		}
		/* Caz special 2: Punctul se afla pe peretele estic. */
		if(currentPoint.xCoord == 8)
		{
			for(int i = -1; i <= 1; i++)
				if(!arcExists(new Arc(currentPoint, new Point(7, currentPoint.yCoord + i))))
					mutari.add(new Point(7, currentPoint.yCoord + i));
			return mutari;
		}
		
		/* Caz special 3: Punctul se afla pe peretele sudic.  */
		if(currentPoint.yCoord == 0)
		{
			for(int i = -1; i <= 1; i++)
				if(!arcExists(new Arc(currentPoint, new Point(currentPoint.xCoord + i, 1))))
					mutari.add(new Point(currentPoint.xCoord + i, 1));
			return mutari;
		}
		
		/* Caz special 4: Punctul se afla pe peretele nordic. */
		if(currentPoint.yCoord == 10)
		{
			/* Cazul in care ne aflam in vecinatatea portii(Victorie). */
			if(currentPoint.xCoord <= 5 && currentPoint.xCoord >= 3)
			{
				mutari.add(new Point(4, 11));
				return mutari;
			}
			
			for(int i = -1; i <= 1; i++)
				if(!arcExists(new Arc(currentPoint, new Point(currentPoint.xCoord + i, 9))))
					mutari.add(new Point(currentPoint.xCoord + i, 9));
			return mutari;
		}
		
		/* Punct aflat in interiorul tablei. */
		for(int i = -1; i <= 1; i++)
			for(int j = -1; j <= 1; j++)
			{
				if(i == 0 && j == 0) /* nu ne putem muta in punctul din care plecam */
					continue;
				
				Arc aux = new Arc(currentPoint, 
									new Point(currentPoint.xCoord + i, currentPoint.yCoord + j));
				if(!arcExists(aux)) /* daca arcul nu exista deja, mutarea este posibila */
					mutari.add(new Point(currentPoint.xCoord + i, currentPoint.yCoord + j));
			}
		return mutari;
	}
	
	/**
	 * Metoda care pornind de la o mutare si un punct curent si intoarce punctul destinatie.
	 * @param move
	 * Mutare codificata 0..7 ca in enunt.
	 * @param startPoint
	 * Punct de pornire.
	 * @return
	 * Punct destinatie.
	 */
	private Point returnDestination(int move, Point startPoint)
	{
		Point destinatie = null;
		
		switch(move){
		
		case 0: destinatie = new Point(startPoint.xCoord, startPoint.yCoord + 1);
				break;
		case 1: destinatie = new Point(startPoint.xCoord + 1, startPoint.yCoord + 1);
				break;
		case 2: destinatie = new Point(startPoint.xCoord + 1, startPoint.yCoord);
				break;
		case 3: destinatie = new Point(startPoint.xCoord + 1, startPoint.yCoord - 1);
				break;
		case 4: destinatie = new Point(startPoint.xCoord, startPoint.yCoord - 1);
				break;
		case 5: destinatie = new Point(startPoint.xCoord - 1, startPoint.yCoord - 1);
				break;
		case 6: destinatie = new Point(startPoint.xCoord - 1, startPoint.yCoord);
				break;
		case 7: destinatie = new Point(startPoint.xCoord -1 , startPoint.yCoord + 1);
				break;
		}
		
		return destinatie;
	}
	
	/**
	 * Metoda care efectueaza o mutare, pornind de la un punct dat si o mutare specificata.
	 * Foloseste metoda auxiliara returnDestination.
	 * @param move
	 * Mutare codificata 0..7 ca in enunt.
	 * @param startPoint
	 * Punct pornire.
	 * @return
	 * Punct destinatie.
	 */
	private Point move(int move, Point startPoint)
	{
		Point destinatie = returnDestination(move, startPoint);
		laturi.add(new Arc(startPoint, destinatie));
		puncte.add(destinatie);
		
		return destinatie;
	}
	
	
	/**
	 * Metoda care actualizeaza tabla de joc cu mutarile adversarului.
	 * 
	 * @param moves
	 * Lista continand mutarile adversarului.
	 * @param currentPoint
	 * Punctul curent din care se porneste.
	 * @return
	 * Punctul curent la care s-a ajuns.
	 * 
	 */
	private Point adversaryMove(ArrayList<Integer> moves, Point currentPoint)
	{
		for(int i = 0; i < moves.size(); i++)
		{
			currentPoint = move(moves.get(i), currentPoint);
		}
		
		return currentPoint;
	}
	
	/**
	 * Metoda care citeste input-ul primit de la server prin System.in.
	 * 
	 * @param s
	 * Variabila de tip Scanner cu care se face citirea.
	 * @return
	 * <pre>
	 * Lista de mutari a adversarului daca jocul este in curs de desfasurare.
	 * Null daca jocul incepe sau se incheie.
	 * </pre>
	 */
	private ArrayList<Integer> readInput(Scanner s)
	{
		ArrayList<Integer> moves = new ArrayList<Integer>();
		String identifier = s.next();
		
		/* Daca botul nostru incepe, efectuam o mutare spre nord, 
		 * aceasta fiind cea mai eficienta. */
		if(identifier.contains("S"))
		{
			System.out.print("M 1 0\n");
			
			puncte.add(new Point(4, 6));
			laturi.add(new Arc(new Point(4, 5), new Point(4, 6)));
			return null;
		}
		
		else if(identifier.contains("F"))
		{
			return null;
		}
		
		int nrElem = s.nextInt();
		while(moves.size() < nrElem)
		{
			moves.add(s.nextInt());
		}
	
		return moves;
	}

	/**
	 * Metoda care verifica daca un punct este pozitionat pe zid
	 * @param a
	 * Punctul analizat.
	 * @return
	 * true daca punctul NU se afla pe zid, fals in caz contrar
	 */
	private boolean notWall(Point a)
	{
		/* Pentru usurinta punctele aflate in colturile tablei 
		 * au fost considerate ca nefiind parte din zid */
		if((a.xCoord == 0 && (a.yCoord == 0 || a.yCoord == 10)) || 
				(a.xCoord == 8 && (a.yCoord == 0 || a.yCoord == 10)))
			return true;
		if(a.xCoord == 0 || a.xCoord == 8)
			return false;
		
		if(a.yCoord == 0 && a.xCoord != 4)
			return false;
		if(a.yCoord == 10 && a.xCoord != 4)
			return false;
		return true;
	}
	
	/**
	 * Metoda care determina toate punctele la care se poate ajunge, pornind din punctul curent
	 * si respectand toate regulile jocului
	 * 
	 * @param currentPoint
	 * Punctul din care se porneste.
	 * @return
	 * Lista tuturor punctelor in care se poate ajunge.
	 */
	private ArrayList<Point> getAllPoints(Point currentPoint)
	{
		ArrayList<Point> result = new ArrayList<Point>();
		/* lista mutari posibile, la pasul curent */
		ArrayList<Point> aux = possibleMoves(currentPoint);

		for(int i = 0; i < aux.size(); i++)
		{
			/* pentru mutarea analizata curent, construim arcul aferent */
			laturi.add(new Arc(currentPoint, aux.get(i)));
			
			/* daca s-a ajuns intr-un punct nou si neconectat la perete, 
			 * acesta este punct final */
			if((!pointExists(aux.get(i))) && notWall(aux.get(i)) )
			{
				result.add(aux.get(i));
			}
			
			/* altfel continuam in recursivitate pana ce gasim toate punctele finale */
			else result.addAll(getAllPoints(aux.get(i)));
			
			/* resetam tabla de joc */
			laturi.remove(new Arc(currentPoint, aux.get(i))); 
		}
		
		/* eliminam duplicatele din rezultat */
		result = elimDuplicates(result);
		return result;
	}
	
	/**
	 * Determina calea de la punctul a la punctul b, modificand tabla.
	 * Dupa ce metoda a fost aplicata, se considera ca exista calea a->b pe tabla
	 * @param a
	 * Punct sursa.
	 * @param b
	 * Punct destinatie.
	 * @return
	 * Lista de puncte traversate pentru a ajunge de la a la b.
	 */
	public ArrayList<Point> getPath(Point a, Point b)
	{
		ArrayList<Point> result = new ArrayList<Point>();
		/* lista mutari posibile la pasul curent */
		ArrayList<Point> moves = possibleMoves(a);
		
		/* inseram punctul curent in rezultat */
		result.add(a);
		
		for(int i = 0; i < moves.size(); i++)
		{
			/* pentru mutarea analizata curent, construim arcul aferent */
			laturi.add(new Arc(a, moves.get(i)));
			/* daca s-a ajuns intr-un punct nou si neconectat la perete, 
			 * acesta este punct final */
			if(!pointExists(moves.get(i)) && notWall(moves.get(i)))
			{
				/* daca este punctul cautat ne oprim */
				if(b.equals(moves.get(i)))
					{
					result.add(moves.get(i));
					break;
					}
				/* altfel resetam tabla */
				else
					laturi.remove(new Arc(a, moves.get(i)));
			}
			
			/* altfel cautam punctul recursiv */
			else {
				/* size(result) = 1 + size(getPath...) */
				result.addAll(getPath(moves.get(i), b));
				
				/* daca solutia gasita contine doar un element, 
				 * atunci acela este elementul curent a; prin urmare solutia nu este valida */
				if(result.size() == 1)
					laturi.remove(new Arc(a, moves.get(i)));
				/* in caz contrar, avem o solutie valida */
				else break;
			}			
		}	
		
		/* daca rezultatul contine doar elementul curent a, calea nu este valida, 
		 * deci intoarcem o lista vida */
		if(result.size() == 1 && !result.get(0).equals(b))
			return new ArrayList<Point>();
		return result;
	}
	
	/**
	 * <pre>
	 * Metoda care calculeaza ce mutare s-a efectuat pentru a ajunge de la punctul a la b, 
	 * acestea fiind adiacente(intr-un sens mai larg) 
	 * 
	 * ex:		- - b
	 * 			- a -		(b poate ocupa orice pozitie marcata "-")
	 * 			- - -		(aici mutarea este 1 - NE)
	 * </pre> 
	 * @param a
	 * Punct pornire.
	 * @param b
	 * Punct sosire.
	 * @return
	 * Mutarea efectuata.
	 */
	private int getDirection(Point a, Point b)
	{
		int x = b.xCoord - a.xCoord;
		int y = b.yCoord - a.yCoord;
		
		if(x == -1) /* mutari spre vest */
			return 6 + y;
		if(x == 1) /* mutari spre est */
			return 2 - y;
		
		/* mutari strict nord sau sud */
		return 2*(1 - y);
	}
	
	/**
	 * Metoda care converteste o lista de n puncte adiacente in o lista de n-1 mutari
	 * pentru a ajunge de la primul punct la ultimul 
	 * 
	 * @param list
	 * Lista puncte adiacente.
	 * @return
	 * Lista mutari.
	 */
	private ArrayList<Integer> convertPath(ArrayList<Point> list)
	{
		ArrayList<Integer> result = new ArrayList<Integer>();
		
		for(int i = 0; i + 1 < list.size(); i++)
		{
			result.add(getDirection(list.get(i), list.get(i + 1)));
		}
		
		return result;
	}
	
	/**
	 * Metoda care elimina duplicatele dintr-o lista de puncte
	 * 
	 * @param list
	 * Lista initiala.
	 * @return
	 * Lista finala.
	 */
	private ArrayList<Point> elimDuplicates(ArrayList<Point> list)
	{
		ArrayList<Point> result = new ArrayList<Point>();
		for(int i = 0; i < list.size(); i++)
			if(!result.contains(list.get(i)))
					result.add(list.get(i));
		
		return result;
	}
	
	/**
	 * Metoda care "rezolva" un jocde fotbal pe hartie.
	 * 
	 * @param scanner
	 * Variabila de tip Scanner prin care se face citirea de la System.in
	 */
	public void solve(Scanner scanner)
	{
		/* punct pornire joc */
		Point currentPoint = new Point(4, 5);
		puncte.add(currentPoint);
		ArrayList<Integer> adversaryMoves = null;
		
		/* citesc mutari adversar */
		adversaryMoves = readInput(scanner);
		
		/* daca nu s-au citit mutari, inseamna ca noi am inceput, si prin urmare am
		 * facut mutarea cea mai eficienta la acest pas (Nord de la (4,5) la (4,6))*/
		if(adversaryMoves == null)
			{
			adversaryMoves = readInput(scanner);
			currentPoint = new Point(4, 6);
			}
		
		/* cat timp citim mutari de la adversar */
		while(adversaryMoves != null)
		{
			/* actualizam tabla de joc cu mutarea adversarului */
			currentPoint = adversaryMove(adversaryMoves, currentPoint);
			
			/* determinam toate mutarile noastre posibile */
			ArrayList<Point> possibleMoves = getAllPoints(currentPoint);
			double min = Double.MAX_VALUE;
		
			/* alegem mutarea care ne duce cel mai aproape de centrul portii */
			Point minPoint = null;
			for(int i = 0; i < possibleMoves.size(); i++)
			{
				if(calcDist(goalCenter, possibleMoves.get(i)) < min)
				{
					min = calcDist(goalCenter, possibleMoves.get(i));
					minPoint = possibleMoves.get(i);
				}
			}

			/* generam si actualizam pe tabla calea care ne duce la punctul cel mai apropiat 
			 * de centrul portii care poate fi atins in acest moment */
			ArrayList<Integer> path = convertPath(getPath(currentPoint, minPoint));
			puncte.add(minPoint);
			/* punctul gasit devine punctul curent */
			currentPoint = minPoint;
			
			/* afisam comanda transmisa catre server */
			System.out.print("M " + path.size());
			for(int i = 0; i < path.size(); i++)
				System.out.print(" " + path.get(i));
			System.out.print("\n");
			
			/* citim urmatoarea mutare a adversarului */
			adversaryMoves = readInput(scanner);
		}
	}
	
	public static void main(String[] args)
	{
		/* initializam bot si scanner*/
		Scanner scanner = new Scanner(System.in);
		Bot myBot = new Bot();
		
		/* folosim metoda solve, definita in cadrul botului */
		myBot.solve(scanner);
		
		scanner.close();
	}
}
