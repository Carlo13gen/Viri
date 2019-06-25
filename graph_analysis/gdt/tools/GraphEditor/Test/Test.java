package Test;

import IU.JWorkbench;

public class Test {

	/**
	 * @param args
	 */
	
	static { System.loadLibrary("GdtJavaWrapper"); }
	
	public static void main(String[] args) {
		
		@SuppressWarnings("unused")
		JWorkbench b;
		if (args.length != 0)
			b = new JWorkbench(args[0]);
		else 
			b = new JWorkbench("");
	}

}
