import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashMap;

import java_cup.runtime.Symbol;


/**
 * Clasa pentru reprezentarea unei stari.
 * Folosita atat de DFA cat si de AFN.
 * 
 * @author Bogdan
 *
 */
class State implements Comparable<State>
{
	String name;

	public State(String name) {
		super();
		this.name = name;
	}

	@Override
	public String toString() {
		return name;
	}

	@Override
	public boolean equals(Object arg0) {
		return name.equals(((State)arg0).name);
	}

	@Override
	public int hashCode() {
		return name.hashCode();
	}

	@Override
	public int compareTo(State arg0) {
		return name.compareTo(arg0.name);
	}
	
}


/**
 * Clasa pentru reprezentarea unui DFA.
 * Implementeaza interfata RegLang(Intrucat exprima un limbaj regulat).
 * 
 * @author Bogdan
 *
 */
public class DFA implements RegLang
{
	State start_state;
	ArrayList<State> states;
	ArrayList<State> stop_states;
	HashMap<State, HashMap<String, State>> transitions;
	String alphabet;

	@Override
	public String toString() {
		return "({" + states.toString().substring(1, states.toString().length() - 1).replaceAll(" ","") 
				+ "},{" + alphabet + "}," + printTransition() + "," + start_state + ",{" 
				+ stop_states.toString().substring(1, stop_states.toString().length() - 1).replaceAll(" ","") + "})";
	}
	
	/**
	 * Metoda folosita pentru afisarea tranzitiilor in formatul cerut de enuntul temei.
	 * 
	 */
	public String printTransition()
	{
		String result = "";
		
		for(State e : transitions.keySet())
		{
			for(String f : transitions.get(e).keySet())
			{
				result += "d(" + e.name + "," + f + ")=" + transitions.get(e).get(f).name + ",";
			}
		}
		
		return "{" + result.substring(0, result.length() - 1) + "}";
	}

	/**
	 *	Constructor pentr DFA. 
	 */
	public DFA(State start_state, ArrayList<State> states,
			ArrayList<State> stop_states, HashMap<State, HashMap<String, State>> transitions, String alphabet ) {
		super();
		this.start_state = start_state;
		this.states = states;
		this.stop_states = stop_states;
		this.transitions = transitions;
		this.alphabet = alphabet;
	}
	
	
	/**
	 * Metoda care construieste un obiect de tip DFA pe baza unei reprezentari date.
	 * 
	 * @param input
	 * Reprezentarea unui DFA.
	 */
	public static DFA DFABuilder(String input){
		ArrayList<State> states = new ArrayList<State>();
		ArrayList<State> final_states = new ArrayList<State>();
		HashMap<State, HashMap<String, State>> transitions = new HashMap<State, HashMap<String, State>>();
		String alphabet = null;
		State start_state = null;
		
		DFALexer my_lexer = new DFALexer(new StringReader(input));
		
		try {
			Symbol parseResult = my_lexer.next_token();
			
			//citesc starile
			while(parseResult.sym == DFALexer.STATES)
			{
				states.add(new State((String)parseResult.value));
				parseResult = my_lexer.next_token();
			}
			
			//citesc alfabetul
			while(parseResult.sym == DFALexer.ALFAB)
			{
				alphabet = (String)parseResult.value;
				parseResult = my_lexer.next_token();
			}
			
			//citesc tranzitiile
			while(parseResult.sym == DFALexer.TRANS)
			{
				
				String startState = (String)parseResult.value;
				parseResult = my_lexer.next_token();
				String symbol = (String)parseResult.value;
				parseResult = my_lexer.next_token();
				String stopState = (String)parseResult.value;
				parseResult = my_lexer.next_token();
				
				if(!transitions.containsKey(new State(startState)))
				{
					transitions.put(new State(startState), new HashMap<String, State>());
				}

				transitions.get(new State(startState)).put(symbol, new State(stopState));
				
				
			}
			
			//citesc starea de inceput
			while(parseResult.sym == DFALexer.STARTS)
			{
				start_state = new State((String)parseResult.value);
				parseResult = my_lexer.next_token();
			}
			
			//citesc starile finale
			while(parseResult.sym == DFALexer.STATES)
			{
				final_states.add(new State((String)parseResult.value));
				parseResult = my_lexer.next_token();
			}
			
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return new DFA(start_state, states, final_states, transitions, alphabet);
	}
	

	/* Metode implementate de toate clasele care implementeaza RegLang */
	
	
	/**
	 * Metoda care verifica daca DFA-ul accepta un anumit sir.
	 * 
	 * Functionare: Impartim sirul in caractere. Secvential din starea curenta,
	 * pe caracterul curent avansam in noua stare. Daca ultima stare in care ajungem
	 * este o stare finala sirul este acceptat, altfel nu este.
	 * 
	 * 
	 * @param input
	 * Sirul verificat.
	 */
	public boolean acceptString(String input)
	{
		State currentState = start_state;
		
		if(input.equals("e") && stop_states.contains(start_state))
			return true;
		
		
		for(int i=0; i < input.length(); i++)
		{
			String x = input.substring(i, i+1);
			
			if(!transitions.containsKey(currentState))
				return false;
			
			if(!transitions.get(currentState).containsKey(x))
				return false;
			
			currentState = transitions.get(currentState).get(x);			
		}
		
		if(stop_states.contains(currentState))
			return true;
		
		return false;
	}


	
	@Override
	public RE toRE() {
		DFAtoRE tr1 = new DFAtoRE();
		return tr1.buildRE(this);
	}

	@Override
	public NFA toNFA() {
		return toRE().toNFA();
	}

	@Override
	public DFA toDFA() {
		return this;
	}
}
