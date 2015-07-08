import java.io.IOException;
import java.util.Scanner;


public class Main {
	
	public static void main(String[] args) throws IOException
	{
		RegLang ana;
		Scanner x = new Scanner(System.in);
		if(args[0].equals("--from-RE"))
		{
			ana = new RE();
			((RE)ana).buildRegEx(x.next());
		}
		
		else if(args[0].equals("--from-NFA"))
			ana = NFA.NFABuilder(x.next());
		else if(args[0].equals("--from-DFA"))
			ana = DFA.DFABuilder(x.next());
		
		else{
			System.out.println("Not implemented.");
			x.close();
			return;
		}
		
		if(args[1].equals("--to-RE"))
			System.out.println(ana.toRE());
		else if(args[1].equals("--to-NFA"))
			System.out.println(ana.toNFA());
		else if(args[1].equals("--to-DFA"))
			System.out.println(ana.toDFA());
		else if(args[1].equals("--contains"))
		{
			int i = 2;
			
			while(i < args.length)
				System.out.println((ana.acceptString(args[i++]) ? "True" : "False"));
		}
		
		x.close();
	}
}
