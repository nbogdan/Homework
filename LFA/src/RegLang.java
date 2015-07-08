/**
 * Interfata pe care o vor extinde clasele RE, NFA, DFA (exprima limbaje regulate)
 * 
 * @author Bogdan
 *
 */
public interface RegLang {

	/* Metoda care verifica daca RegLang-ul accepta un sir. */
	public boolean acceptString(String input);
	
	/* Metoda care va intoarce RE-ul echivalent pentru RegLang-ul dat. */
	public RE toRE();
	
	/* Metoda care va intoarce NFA-ul echivalent pentru RegLang-ul dat. */
	public NFA toNFA();
	
	/* Metoda care va intoarce DFA-ul echivalent pentru RegLang-ul dat. */
	public DFA toDFA();
}
