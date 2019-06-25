package Notifier;

import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class CreditsNotifier extends AbstractNotifier {
	
	public CreditsNotifier() {
		super();
	}

	public void showMessage() {	
		option = new JOptionPane("GDToolkit Graph Editor \n\nDeveloped by:\n" +
								"Enrico Colasante & Valerio Cosentino",JOptionPane.INFORMATION_MESSAGE,JOptionPane.CLOSED_OPTION);
		dialog = option.createDialog(new JFrame(),"CREDITS");
		dialog.pack();
		dialog.setVisible(true);
	}

}
