import java.util.ArrayList;
import java.util.HashMap;
import java.util.TreeSet;

/**
 * Clasa care reprezinta o tranzitie avand ca stari obiecte de tip NewState.
 * 
 * @author Bogdan
 *
 */
class NewTransition{
	NewState start, stop;
	String symbol;
	
	public NewTransition(NewState start, NewState stop, String symbol) {
		super();
		this.start = start;
		this.stop = stop;
		this.symbol = symbol;
	}
	
	public String transitionText()
	{
		return "d(" + start.newStateName() + "," + symbol + ")=" + stop.newStateName();
	}
	
}

/**
 * NewState este o stare a DFA-ului construit alcatuita din mai multe stari
 * ale NFA-ului folosit.
 * 
 * Pentru State-urile A, B, C putem avea NewState-ul {A, B, C}
 * 
 * @author Bogdan
 *
 */
class NewState{
	ArrayList<State> states;
	
	/* Multimea tuturor NewState-urilor create */
	static HashMap<TreeSet<State>, Integer> states_map;
	static int count = 1;
	
	/**
	 *	Constructor pentru NewState. 
	 */
	public NewState(ArrayList<State> states) {
		super();
		if(states_map == null)
			states_map = new HashMap<TreeSet<State>, Integer>();
		this.states = states;
	}

	/**
	 * Metoda care intoarce numele unui NewState.
	 * 
	 * Se folosesc TreeSet-uri pentru retinerea State-urilor componente 
	 * deoarece acestea le sorteaza.
	 * Astfel: {A, B, C} si {C, B, A} vor avea aceeasi reprezentare.
	 */
	public String newStateName()
	{
		TreeSet<State> aux = new TreeSet<State>();
		for(State e : states)
		{
			aux.add(e);
		}
		
		if(!states_map.containsKey(aux))
			states_map.put(aux, count++);
		
		int id = states_map.get(aux);

		return "Ns" + id;
	}
	
	@Override
	public boolean equals(Object arg0) {
		
		if(states.size() != ((NewState)arg0).states.size())
			return false;
		
		if(states.size() == 0)
			return true;
		
		for(State e : states)
			if(!((NewState)arg0).states.contains(e))
				return false;
		
		for(State e : ((NewState)arg0).states)
			if(!states.contains(e))
				return false;
		
		return true;
	}

	@Override
	public int hashCode() {
		TreeSet<State> aux = new TreeSet<State>();
		for(State e : states)
			aux.add(e);

		return aux.hashCode();
	}

}

/**
 * Clasa continand metodele necesare unei transformari din NFA in DFA
 * 
 *  Algoritmul de transformare se gaseste in README
 *  
 * @author Bogdan
 *
 */
public class NFAtoDFA {

	NFA my_NFA;
	
	/**
	 * Metoda care calculeaza inchiderea pe sirul vid. Aceasta este totalitatea starilor
	 * in care se poate ajunge dintr-o multime de stari curente(init_set) facand oricate
	 * tranzitii pe sirul vid.
	 * 
	 */
	ArrayList<State> eps_clojure(ArrayList<State> init_set)
	{
		ArrayList<State> result = new ArrayList<State>();
		ArrayList<State> aux = new ArrayList<State>();
		ArrayList<State> intermed = new ArrayList<State>(init_set);
		int length = 0;
		
		result.addAll(init_set);
		
		/* bucla este repetata cat timp de la pasul t la pasul t+1 s-au mai gasit noi stari */
		do
		{
			length = result.size();
			for(State e : intermed)
			{
				if(my_NFA.relations.containsKey(e) && my_NFA.relations.get(e).containsKey("e"))
					aux.addAll(my_NFA.relations.get(e).get("e"));
			}
			
			for(State e : aux)
				if(!result.contains(e))
					result.add(e);
			intermed = new ArrayList<State>(result);
		}while(length != result.size());
		
		return result;
	}

	/**
	 * Aceasta metoda calculeaza totalitatea starilor in care se poate ajunge,
	 * plecand de la un set de stari(init) si facand o singura tranzitie pe 
	 * un anumit simbol(symbol). 
	 * 
	 */
	ArrayList<State> clojureOnSymbol(ArrayList<State> init, String symbol)
	{
		ArrayList<State> result = new ArrayList<State>();
		ArrayList<State> aux = new ArrayList<State>();
		
		for(State e : init)
		{
			if(my_NFA.relations.containsKey(e) && my_NFA.relations.get(e).containsKey(symbol))
				aux.addAll(my_NFA.relations.get(e).get(symbol));
		}
		
		for(State e : aux)
			if(!result.contains(e))
				result.add(e);
		
		return result;
	}
	
	/**
	 * Metoda de normalizare a NFA-ului. Se elimina tranzitiile pe siruri de lungime mai mare de 1.
	 * In acest scop se vor introduce si niste stari intermediare.
	 * 
	 */
	public void normalizeNFA()
	{
		HashMap<State,ArrayList<String>> partial_relation = new HashMap<>();

		/*NOTA: stare_de_plecare = stare din care se pleaca pe o tranzitie */
		/* salvam toate asocierile stare_de_plecare, sir tranzitie intr-o structura auxiliara */
		for(State e : my_NFA.relations.keySet())
		{
			partial_relation.put(e,  new ArrayList<String>());
			for(String elem : my_NFA.relations.get(e).keySet())
			{
				partial_relation.get(e).add(elem);
			}
		}
		
		/* pentru fiecare stare_de_plecare */
		for(State e : partial_relation.keySet())
		{
			ArrayList<String> toRemove = new ArrayList<String>();
			
			/* pentru fiecare sir pe care se face tranzitie */
			for(String elem : partial_relation.get(e))
			{
				/* eliminam tranzitiile pe siruri de lungime > 1 */
				if(elem.length() > 1)
				{
					ArrayList<State> aux_state_list = my_NFA.relations.get(e).get(elem);
					
					toRemove.add(elem);
					
					/* inlocuim o tranzitie pe un sir de lungime n cu n tranzitii pe un sir de lungime 1*/
					/* in acest proces introducem si n-1 noi stari intermediare */
					for(int i = 1; i < elem.length(); i++)
					{
						my_NFA.states.add(new State(e.name + "_" + elem + "_" + i));
					}
					
					ArrayList<State> aux = new ArrayList<State>();
					aux.add(new State(e.name + "_" + elem + "_" + (1)));
					if(!my_NFA.relations.get(e).containsKey(String.valueOf(elem.charAt(0))))
						my_NFA.relations.get(e).put(String.valueOf(elem.charAt(0)), aux);
					else
						my_NFA.relations.get(e).get(String.valueOf(elem.charAt(0))).addAll(aux);
					
					for(int i = 1; i < elem.length() - 1; i++)
					{
						aux = new ArrayList<State>();
						aux.add(new State(e.name + "_" + elem + "_" + (i + 1)));
						my_NFA.relations.put(new State(e.name + "_" + elem + "_" + i), new HashMap<String, ArrayList<State>>());
						my_NFA.relations.get(new State(e.name + "_" + elem + "_" + i)).put(String.valueOf(elem.charAt(i)), aux);
					}

					my_NFA.relations.put(new State(e.name + "_" + elem + "_" + (elem.length() - 1)), new HashMap<String, ArrayList<State>>());
					my_NFA.relations.get(new State(e.name + "_" + elem + "_" + (elem.length() - 1))).put(String.valueOf(elem.charAt(elem.length()-1)), aux_state_list);
				}
			}
			
			/* se elimina acele tranzitii pe siruri de lungimi >1 */
			for(String string : toRemove)
				my_NFA.relations.get(e).remove(string);
		}
	}
	
	/** 
	 * Metoda care transforma un NFA intr-un DFA. 
	 * 
	 * Pentru detalii vedeti in README.
	 * */
	public DFA buildDFA(NFA afn)
	{
		my_NFA = afn;
		ArrayList<NewState> marked = new ArrayList<NewState>();
		ArrayList<NewState> unmarked = new ArrayList<NewState>();
		ArrayList<NewState> stop_states = new ArrayList<NewState>();
		ArrayList<State> aux = new ArrayList<State>();
		ArrayList<NewTransition> transitions = new ArrayList<NewTransition>();
		NewState start_state;

		normalizeNFA();
		
		aux.add(my_NFA.start_state);
		start_state = new NewState(eps_clojure(aux));
		unmarked.add(start_state);

		/* Cat timp avem stari neanalizate. */
		while(!unmarked.isEmpty())
		{
			NewState currentState = unmarked.get(0);
			unmarked.remove(0);
			marked.add(currentState);
			
			for(String symbol : my_NFA.alphabet.split(","))
			{
				ArrayList<State> S = new ArrayList<State>();
				S = eps_clojure(clojureOnSymbol(currentState.states, symbol));
				
				if(!marked.contains(new NewState(S)) && !unmarked.contains(new NewState(S)))
					unmarked.add(new NewState(S));

				transitions.add(new NewTransition(currentState, new NewState(S), symbol));
			}
			
		}
		
		/* determinam starile finale ale noului automat */
		for(NewState e : marked)
		{
			for(State elem : e.states)
			{
				if(my_NFA.final_states.contains(elem))
				{
					stop_states.add(e);
					break;
				}
			}
		}
		
		/* facem trecerea de la multimi de stari ale NFA (NewState) la stari ale DFA (State) */
		ArrayList<State> afd_states = new ArrayList<State>();
		ArrayList<State> afd_stop_states = new ArrayList<State>();
		State afd_start_state = new State(start_state.newStateName());
		HashMap<State, HashMap<String, State>> afd_transitions = new HashMap<State, HashMap<String, State>>();
		
		/* construim tranzitiile DFA */
		for(NewTransition e : transitions)
		{
			String aux_string = e.transitionText();
			String startState = aux_string.substring(2).split(",")[0];
			String symbol = aux_string.substring(3 + startState.length(), 4 + startState.length());
			String stopState = aux_string.substring(5 + startState.length() + symbol.length());
			
			if(!afd_transitions.containsKey(new State(startState)))
			{
				afd_transitions.put(new State(startState), new HashMap<String, State>());
			}

			afd_transitions.get(new State(startState)).put(symbol, new State(stopState));
		}
		
		/* construim multimea starilor DFA */
		for(NewState e : marked)
		{
			afd_states.add(new State(e.newStateName()));
		}
		
		/* construim multimea starilor finale DFA */
		for(NewState e : stop_states)
		{
			afd_stop_states.add(new State(e.newStateName()));
		}
		
		
		return new DFA(afd_start_state, afd_states, afd_stop_states, afd_transitions, my_NFA.alphabet);
	}
	
}
