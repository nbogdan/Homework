import java.util.ArrayList;

/**
 * Clasa continand metodele necesare unei transformari din DFA in RE
 * 
 *  Algoritmul de transformare se gaseste in README
 *  
 * @author Bogdan
 *
 */
public class DFAtoRE {

	DFA my_DFA;
	RE my_RE;
	
	/**
	 * Metoda care determina expresia regulata care exprima tranzitiile posibile
	 * de la o stare i la o stare j fara a trece prin vreo stare intermediara.
	 * 
	 *  Ex: daca se poate ajunge de la i la j atat pe a cat si pe b rezultatul va fi: a|b
	 *  	daca nu se poate ajunge de la i la j rezultatul va fi e
	 */
	public String getSymb(int i, int j)
	{
		String result = "";
		State a = my_DFA.states.get(i);
		State b = my_DFA.states.get(j);
		for(String e : my_DFA.transitions.get(a).keySet())
		{
			if(my_DFA.transitions.get(a).get(e).equals(b))
				result += e + "|";
		}
		
		if(result.length() == 0)
			return "O";
		
		return result.substring(0, result.length() - 1);
	}
	
	/**
	 *	Metoda in care se construieste o expresie regulata pornind de la un DFA. 
	 * 
	 */
	public RE buildRE(DFA dfa)
	{
		my_DFA = dfa;
		int n = my_DFA.states.size();
		
		/* caz particular
		 * 
		 * Pentru un DFA cu toate starile finale RE-ul echivalent va fi
		 * reuniunea tuturor simbolurilor de tranzitie star.
		 * 
		 * In acest caz nu are rost sa mai folosim algoritmul K-Path.
		 * */
		if(dfa.states.size() == dfa.stop_states.size())
		{
			String result = "";
			ArrayList<String> symbols = new ArrayList<String>();
			
			for(State e: my_DFA.transitions.keySet())
			{
				for(String elem : my_DFA.transitions.get(e).keySet())
					if(!symbols.contains(elem))
						symbols.add(elem);
			}
			
			result = symbols.toString().replaceAll(",", "|").replaceAll(" ", "");
			result = "(" + result.substring(1, result.length() - 1) + ")*";
			
			RE expr = new RE();
			expr.buildRegEx("{" + dfa.alphabet + "}:" + result);
			
			return expr;
		}
		
		
		/* Algoritmul K-Path */
		
		
		/* multimea tuturor tranzitiilor posibile
		 * 
		 * pentru tranzitia de la i la j vom verifica elementul n * i + j (matrice liniarizata)
		 * */
		ArrayList<String> possibilities = new ArrayList<String>(n * n);
		
		ArrayList<String> possibilities_aux = new ArrayList<String>(n * n);
		
		for(int i = 0; i < n; i++)
			for(int j = 0; j < n; j++)
			{
				if(i == j)
					if(getSymb(i, j).equals("O"))
						possibilities.add(i * n + j, "e");
					else
						possibilities.add(i * n + j, "e|" + getSymb(i, j));
				else
					possibilities.add(i * n + j, getSymb(i, j));
				
			}
		
		
		for (int k = 0; k < n; k++) 
		{
			possibilities_aux.clear();
			for (int i = 0; i < n * n; i++) {
				String tok1, tok2, tok3, tok4;

				/* Expresia de la pasul curent va avea forma:
				 * tok1|tok2(tok3)*tok4 */
				tok1 = possibilities.get(i);
				tok2 = possibilities.get(((i / n) * n) + k);
				tok3 = possibilities.get(k * n + k);
				tok4 = possibilities.get(k * n + (i % n));
				String result = "";
				
				/* simplificam expresiile rezultate prin eliminarea concatenarii cu O sau e
				 * sau a expresiilor de forma a|a */
				if(tok2.equals("O") || tok4.equals("O"))
				{
					possibilities_aux.add(i, tok1);
					continue;
				}
				
				if(!tok1.equals("O"))
					if(tok1.length() != 1)
						tok1 = "(" + tok1 + ")";
				
				if(!tok2.equals("e"))
					if(tok2.length() != 1)
						tok2 =  "(" + tok2 + ")";
				
				
				if(!(tok3.equals("e") || tok3.equals("O")))
					if(tok3.length() > 1)
					{
						if(tok3.startsWith("e|"))
							tok3 = tok3.substring(2);
						if(tok3.endsWith("|e"))
							tok3 = tok3.substring(0, tok3.length() - 2);
						
						if(tok3.length() > 1)
							tok3 = "(" + tok3 + ")";
					}
				if(!tok4.equals("e"))
					if(tok4.length() != 1)
						tok4 =  "(" + tok4 + ")";
				
				
				if(!tok1.equals("O"))
					result += tok1 + "|";
				
				if(!tok2.equals("e"))
					result += tok2;
				
				if(!tok3.equals("e")&&!tok3.equals("O"))
					result += tok3 + "*";
				
				if(!tok4.equals("e"))
					result += tok4;
				
				if(tok4.equals("e") && tok2.equals("e") && (tok3.equals("e")||tok3.equals("O")))
						result += "e";
				
				if(!result.contains("("))
					if(result.split("\\|").length == 2 && result.split("\\|")[0].equals(result.split("\\|")[1]))
						result = result.split("\\|")[0];
				
				possibilities_aux.add(i, result);
			}


			possibilities = new ArrayList<>(possibilities_aux);
		}

		/* La final possibilities.get(n*i+j) va contine expresia regulata care genereaza toate 
		 * sirurile acceptate de automatul care are i ca stare de pornire, j ca stare finala
		 * unica si multimea starilor, alfabetul si multimea tranzitiilor egale cu ale DFA-ului
		 * nostru. */
		
		
		
		/* Rezultatul va fi reuniunea tuturor cailor initial -> final */
		
		int indice_init = dfa.states.indexOf(dfa.start_state);
		
		String result = "";
		
		for(State e : dfa.stop_states)
		{
			result += possibilities.get(indice_init * dfa.states.size() + dfa.states.indexOf(e)) + "|";
		}
		
		if(result.equals(""))
			result = "O";
		else
			result = result.substring(0, result.length() - 1);
		
		RE expr = new RE();
		expr.buildRegEx("{" + dfa.alphabet + "}:" + result);
		
		return expr;
	}
}
