import java.io.IOException;
import java.io.StringReader;
import java.util.Stack;


/**
 * Clasa care reprezinta un nod din arborele de parsare.
 * 
 * @author Bogdan
 *
 */
class Node{
	enum type {operand, operator}
	
	char value;
	int ord;
	int start_id, stop_id;
	
	type my_type;
	Node left;
	Node right;
	
	public Node(Character value, Node left, Node right) {
		super();
		this.value = value;
		this.left = left;
		this.right = right;
	}
	
	public static int numnodes(Node e)
	{
		if(e == null)
			return 0;
		
		return 1 + numnodes(e.left) + numnodes(e.right);
	}

}

/**
 * Clasa care reprezinta o expresie regulata.
 * Implementeaza interfata RegLang(Intrucat exprima un limbaj regulat).
 * 
 * @author Bogdan
 *
 */
public class RE implements RegLang{
	
	@Override
	public String toString() {
		return "{" + alphabet + "}:" + expression.replaceAll("_", "");
	}

	String expression;
	String alphabet;
	Node rETree;
	Stack<Character> stack1;
	Stack<Node> stack2;
	

	/**
	 * Constructor pentru tipul RE
	 */
	public RE()
	{
		this.rETree = null;
		stack1 = new Stack<Character>();
		stack2 = new Stack<Node>();		
	}
	
	/**
	 *	Constructor pentru tipul RE 
	 */
	public RE(String body, String alphabet) {
		super();
		this.expression = body;
		this.rETree = null;
		this.alphabet = alphabet;
		stack1 = new Stack<Character>();
		stack2 = new Stack<Node>();
	}
	
	/**
	 * Metoda care "construieste" o expresie regulata pe baza unei reprezentari.
	 * Pe langa crearea unui obiect de tip RE se genereaza si arborele de parsare al RE.
	 *  
	 */
	void buildRegEx(String input)
	{
		RELexer lex = new RELexer(new StringReader(input));
		try {
			this.alphabet = (String)lex.next_token().value;
			this.expression = (String)lex.next_token().value;
			regExTreeBuild();
			normalize();
		} catch (IOException e) {
			
			e.printStackTrace();
		}
	}
	
	/**
	 * Metoda folosita pentru a determina gradul de "intaietate" al unui operator.
	 * 
	 * @param c
	 * Operatorul analizat
	 */
	int opOrder(Character c)
	{
		if(c == ')' || c == '(')
			return 0;
		if(c == '|')
			return 1;
		if(c == '_')
			return 2;
		
		return 3;
	}
	
	/**
	 * Metoda care construieste arborele expresiei.
	 * 
	 * Mai multe detalii in README.
	 * 
	 */
	void regExTreeBuild()
	{
		String operator="*?|+_";
		
		// expresia este analizata caracter cu caracter 
		for(int i = 0; i < expression.length(); i++)
		{
			char current_char = expression.charAt(i);
			
			if(current_char == '(')
			{
				stack1.push(current_char);
			}
			
			else if(operator.contains(String.valueOf(current_char)))
			{
				while(!stack1.isEmpty() && opOrder(current_char) <= opOrder(stack1.peek()))
				{
					buildTree();
				}
				stack1.push(current_char);
			}
			
			else if(alphabet.contains(String.valueOf(current_char)) || current_char == 'O' || current_char == 'e')
			{
				stack2.push(new Node(current_char,null, null));
			}
			
			else if(current_char == ')')
			{
				buildTree();
				while(stack1.peek() != '(')
				{
					buildTree();
				}
				stack1.pop();
			}
		}
		
		while(!stack1.isEmpty())
			buildTree();
		
		rETree = new Node('#',stack2.pop(),null);
	}

	/**
	 * Metoda auxiliara folosita in constructia arborelui expresiei.
	 * 
	 */
	void buildTree()
	{
		
		char op = stack1.pop();
		Node tD = stack2.pop();
		Node t, tS;
		if(op == '*' || op == '?' || op == '+')
		{
			t = new Node(op, tD, null);
			stack2.push(t);
		}
	
		else if(op == '|' || op == '_')
		{
			tS = stack2.pop();
			t = new Node(op, tS, tD);
			stack2.push(t);
		}
	}
	
	
	/**
	 * Metoda de eliminare a ramurilor inutile din arbore.
	 * 
	 */
	void reductTree()
	{
		reduction(rETree);
	}
	
	/**
	 * Metoda recursiva pentru eliminarea tuturor ramurilor "inutile" ale unui subarbore.
	 * 
	 * Se vizeaza: concatenare cu e sau O, SAU cu O, O*
	 * 
	 * @param n
	 */
	void reduction(Node n)
	{
		int i = 0;
		Node current;
		
		if(n == null)
			return;
		if(n.left == null && n.right == null)
			return;
		
		//arborele este parcurs in postordine
		reduction(n.left);
		reduction(n.right);
		
		
		while(i < 2)
		{
			//sar peste nodurile copii nule
			if((i == 0 && n.left == null)||(i==1&&n.right==null))
			{
				i++;
				continue;
			}
				
			//nodul copil analizat curent
			current = (i % 2 == 1) ? n.right : n.left;
			
			//pentru concatenare
			if(current.value == '_')
			{
				if(current.left != null && current.left.value == 'e')
				{
					if(i % 2 == 0)
						n.left = current.right;
					else
						n.right = current.right;
				}
				
				else if(current.right != null && current.right.value == 'e')
				{
					if(i % 2 == 0)
						n.left = current.left;
					else
						n.right = current.left;
				}
				
				
				if((current.left != null && current.left.value == 'O') || (current.right != null && current.right.value == 'O'))
				{
					current.value = 'O';
					current.right = null;
					current.left = null;
				}
			}
			
			//pentru SAU
			if(current.value == '|')
			{
				if(current.left != null && current.left.value == 'O')
				{
					if(i % 2 == 0)
						n.left = current.right;
					else
						n.right = current.right;
				}
				
				else if(current.right != null && current.right.value == 'O')
				{
					if(i % 2 == 0)
						n.left = current.left;
					else
						n.right = current.left;
				}
			}
			
			//pentru *
			if(current.value == '*')
			{
				if((current.left != null && current.left.value == 'O') 
						|| (current.right != null && current.right.value == 'O'))
				{
					current.value = 'e';
					current.right = null;
					current.left = null;
				}
				
			}
			
			i++;
		}
	}
	
	/**
	 * Metoda foosita pentru expandarea subarborilor care au ca radacina operatori + sau ?.
	 * 
	 */
	void expandTree()
	{
		expansion(rETree);
	}
	
	/**
	 * Metoda recursiva pentru "expandarea" celor 2 operatori speciali: +, ?
	 * 
	 * Operatia este realizata in preordine.
	 * 
	 * @param n
	 */
	void expansion(Node n)
	{
		if(n == null)
			return;
		if(n.left == null && n.right == null)
			return;
		
		if(n.left != null && n.left.value=='+')
		{
			n.left.value = '*';
			Node aux2 = new Node('_',n.left.left, n.left);
			n.left = aux2;
		}
		
		if(n.right != null && n.right.value=='+')
		{
			n.right.value = '*';
			Node aux2 = new Node('_',n.right.left, n.right);
			n.right = aux2;
		}
		
		if(n.left != null && n.left.value=='?')
		{
			Node aux = new Node('|',n.left.left, new Node('e',null,null));
			n.left = aux;
		}
		if(n.right != null && n.right.value=='?')
		{
			Node aux = new Node('|',n.right.left, new Node('e',null,null));
			n.right = aux;
		}
		
		expansion(n.left);
		expansion(n.right);
	}
	
	/**
	 * Metoda de normalizare a arborelui.
	 * 
	 */
	void normalize()
	{
		expandTree();
		reductTree();
	}

	/* Metode implementate de toate clasele care implementeaza RegLang */
	
	/**
	 * Metoda care verifica daca RE-ul accepta String-ul input.
	 * RE-ul este transformat in DFA si pe acesta se va verifica acceptarea.
	 * 
	 */
	public boolean acceptString(String x)
	{
		REtoNFA tr1 = new REtoNFA(this, null);
		NFA afn = tr1.getAFN();
		
		return afn.acceptString(x);		
	}
	
	@Override
	public RE toRE() {
		return this;
	}

	@Override
	public NFA toNFA() {
		REtoNFA tr1 = new REtoNFA(this, null);
		return tr1.getAFN();
	}

	@Override
	public DFA toDFA() {
		return toNFA().toDFA();
	}
	
	
}