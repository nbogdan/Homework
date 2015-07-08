import java.util.ArrayList;
import java.util.HashMap;

/**
 * Clasa continand metodele necesare unei transformari din RE in NFA
 * Se porneste de la premiza ca arborele expresiei regulate a fost deja construit.
 * 
 *  Algoritmul de transformare se gseste in README
 *  
 * @author Bogdan
 *
 */
public class REtoNFA {

	RE my_RE;
	NFA my_AFN;
	int nr_states;
	
	HashMap<Integer,Integer> option1, option2;
	HashMap<Integer,Character> symbol;
	
	
	public REtoNFA(RE my_regEx, NFA my_afn) {
		this.my_RE = my_regEx;
		this.my_AFN = my_afn;
		this.option1 = new HashMap<Integer, Integer>();
		this.option2 = new HashMap<Integer, Integer>();
		this.symbol = new HashMap<Integer, Character>();
		nr_states = 1;
	}


	/**
	 * Metoda folosita la numerotarea nodurilor in preordine.
	 */
	void preorderTag()
	{
		tagNode(my_RE.rETree.left);
	}
	
	void tagNode(Node n)
	{
		if(n.value != '_')
			n.ord = nr_states++;

		if(n.left != null)
			tagNode(n.left);
		if(n.right != null)
			tagNode(n.right);
		
	}
	
	/**
	 * Metoda folosita la numerotarea nodurilor in postordine.
	 */
	void postorderTag()
	{
		tagPost(my_RE.rETree.left);
	}
	
	void tagPost(Node n)
	{
		if(n.left != null)
			tagPost(n.left);
		if(n.right != null)
			tagPost(n.right);
		
		if(n.value != '_')
		{
			n.start_id = 2 * n.ord - 1;
			n.stop_id = 2 * n.ord;
		}
		else
		{
			n.start_id = n.left.start_id;
			n.stop_id = n.right.stop_id;
		}
		
	}
	
	
	/**
	 * Metoda care parcurge un arbore in postordine si extrage informatiile necesare
	 * 
	 *  vezi README
	 * 
	 */
	void analyzer(Node n)
	{
		if(n.left != null)
			analyzer(n.left);
		if(n.right != null)
			analyzer(n.right);
		if(n.left == null && n.right == null)
		{
			symbol.put(n.start_id, n.value);
			option1.put(n.start_id, n.stop_id);
		}
		
		if(n.value == '|')
		{
			option1.put(n.start_id, n.left.start_id);
			option1.put(n.left.stop_id, n.stop_id);
			option1.put(n.right.stop_id, n.stop_id);
			
			option2.put(n.start_id, n.right.start_id);
		}
		
		if(n.value == '_')
		{
			option1.put(n.left.stop_id,n.right.start_id);
		}
		
		if(n.value == '*')
		{
			option1.put(n.start_id, n.left.start_id);
			option1.put(n.left.stop_id, n.left.start_id);
			
			option2.put(n.start_id, n.stop_id);
			option2.put(n.left.stop_id, n.stop_id);
		}

	}
	
	/**
	 * Metoda care obtine un NFA pornind de la un RE.
	 * 
	 * @return
	 */
	public NFA getAFN()
	{
		preorderTag();
		postorderTag();
		analyzer(my_RE.rETree.left);
		
		int num_states = 2 * (nr_states - 1);
		State start_state = null;
		ArrayList<State> states = new ArrayList<State>();
		ArrayList<State> stop_states = new ArrayList<State>();
		HashMap<State, HashMap<String, ArrayList<State>>> relations = new HashMap<State, HashMap<String, ArrayList<State>>>();
		String alphabet = my_RE.alphabet;
		
		start_state = new State("_" + 1);
		for(int i = 1; i <= num_states; i++)
		{
			states.add(new State("_" + i));
			
			if(!option1.containsKey(i) && !option2.containsKey(i))
				stop_states.add(new State("_" + i));
			
			if(option1.containsKey(i))
			{
				char c;
				if(symbol.containsKey(i))
				{
					c = symbol.get(i);
				}
				else
					c = 'e';
				
				if(!relations.containsKey(new State("_" + i)))
					relations.put(new State("_" + i), new HashMap<String, ArrayList<State>>());
				if(!relations.get(new State("_" + i)).containsKey(String.valueOf(c)))
					relations.get(new State("_" + i)).put(String.valueOf(c), new ArrayList<State>());
				relations.get(new State("_" + i)).get(String.valueOf(c)).add(new State("_" + option1.get(i)));
				
			}
			
			if(option2.containsKey(i))
			{
				char c;
				if(symbol.containsKey(i))
				{
					c = symbol.get(i);
				}
				else
					c = 'e';
				
				if(!relations.containsKey(new State("_" + i)))
					relations.put(new State("_" + i), new HashMap<String, ArrayList<State>>());
				if(!relations.get(new State("_" + i)).containsKey(String.valueOf(c)))
					relations.get(new State("_" + i)).put(String.valueOf(c), new ArrayList<State>());
				relations.get(new State("_" + i)).get(String.valueOf(c)).add(new State("_" + option2.get(i)));
			}
		}

		return new NFA(start_state, states, stop_states, relations, alphabet);
	}
	
}
