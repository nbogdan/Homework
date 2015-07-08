import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashMap;

import java_cup.runtime.Symbol;

/**
 * Clasa care abstractizeaza un NFA. Foloseste elemente definite in modulul DFA.java.
 * Implementeaza interfata RegLang(Intrucat exprima un limbaj regulat).
 * 
 * @author Bogdan
 *
 */
public class NFA implements RegLang{

	State start_state;
	ArrayList<State> states;
	ArrayList<State> final_states;
	HashMap<State, HashMap<String, ArrayList<State>>> relations;
	String alphabet;
	
	@Override
	public String toString() {
		return "({" + states.toString().substring(1, states.toString().length() - 1).replaceAll(" ","") 
				+ "},{" + alphabet + "}," + printRelations() + "," + start_state + ",{" 
				+ final_states.toString().substring(1, final_states.toString().length() - 1).replaceAll(" ","") + "})";
	}
	
	/**
	 * Metoda folosita pentru afisarea tranzitiilor in formatul cerut de enuntul temei.
	 * 
	 * @return
	 */
	public String printRelations()
	{
		String result = "";
		
		for(State e : relations.keySet())
		{
			for(String f : relations.get(e).keySet())
			{
				for(State g : relations.get(e).get(f))
					
				result += "(" + e.name + "," + f + "," + g + "),";
			}
		}
		
		return "{" + result.substring(0, result.length() - 1) + "}";
	}
	
	

	/**
	 * Constructor pentru tipul NFA. 
	 */
	public NFA(State start_state, ArrayList<State> states,
			ArrayList<State> stop_states,
			HashMap<State, HashMap<String, ArrayList<State>>> relations, String alphabet) {
		super();
		this.start_state = start_state;
		this.states = states;
		this.final_states = stop_states;
		this.relations = relations;
		this.alphabet = alphabet;
	}
	
	/**
	 * Metoda prin care se genereaza un obiect de tip AFN, pornind de la o reprezentare
	 * conforma cu specificatiile din enunt.
	 * 
	 * @param input
	 * Reprezentarea data.
	 */
	public static NFA NFABuilder(String input){
		ArrayList<State> states = new ArrayList<State>();
		ArrayList<State> stop_states = new ArrayList<State>();
		HashMap<State, HashMap<String, ArrayList<State>>> transitions = new HashMap<State, HashMap<String, ArrayList<State>>>();
		String alphabet = null;
		State start_state = null;
		
		NFALexer my_lexer = new NFALexer(new StringReader(input));
		
		try {
			Symbol parseResult = my_lexer.next_token();
			
			//citesc starile
			while(parseResult.sym == NFALexer.STATES)
			{
				states.add(new State((String)parseResult.value));
				parseResult = my_lexer.next_token();
			}
			
			//citesc alfabetul
			while(parseResult.sym == NFALexer.ALFAB)
			{
				alphabet = (String)parseResult.value;
				parseResult = my_lexer.next_token();
			}
			
			//citesc relatiile
			while(parseResult.sym == NFALexer.RELS)
			{
				String startState = (String)parseResult.value;
				parseResult = my_lexer.next_token();
				String symbol = (String)parseResult.value;
				parseResult = my_lexer.next_token();
				String stopState = (String)parseResult.value;
				parseResult = my_lexer.next_token();
				
				State start = new State(startState);
				State stop = new State(stopState);
								
				if(!transitions.containsKey(start))
				{
					transitions.put(start, new HashMap<String, ArrayList<State>>());
				}
				
				if(!transitions.get(start).containsKey(symbol))
				{
					transitions.get(start).put(symbol, new ArrayList<State>());
				}
				transitions.get(start).get(symbol).add(stop);
			}
			
			//citesc starea de inceput
			while(parseResult.sym == NFALexer.STARTS)
			{
				start_state = new State((String)parseResult.value);
				parseResult = my_lexer.next_token();
			}
			
			//citesc starile finale
			while(parseResult.sym == NFALexer.STATES)
			{
				if(((String)parseResult.value).equals("NaS"))
					break;
				stop_states.add(new State((String)parseResult.value));
				parseResult = my_lexer.next_token();
			}
			
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		
		return new NFA(start_state, states, stop_states, transitions, alphabet);
	}

	
	/* Metode implementate de toate clasele care implementeaza RegLang */
	
	/**
	 * Metoda care verifica daca NFA-ul accepta String-ul input.
	 * NFA-ul este transformat in DFA si pe acesta se va verifica acceptarea.
	 * 
	 */
	public boolean acceptString(String input)
	{
		/* Pentru sirul vid nu are rost sa transformam in DFA */
		if(input.equals("e") && final_states.contains(start_state))
			return true;
		
		NFAtoDFA tr1 = new NFAtoDFA();
		DFA dfa = tr1.buildDFA(this);
		
		return dfa.acceptString(input);
	}
	
	
	
	@Override
	public RE toRE() {
		return toDFA().toRE();
	}

	@Override
	public NFA toNFA() {
		return this;
	}

	@Override
	public DFA toDFA() {
		NFAtoDFA tr1 = new NFAtoDFA();
		return tr1.buildDFA(this);
	}
	
	
}
