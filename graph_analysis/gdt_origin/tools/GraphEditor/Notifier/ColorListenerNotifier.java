package Notifier;

import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class ColorListenerNotifier extends AbstractNotifier {

	public ColorListenerNotifier() {
		super();
	}

	public void showMessage() {
		option = new JOptionPane("Error choosing color",JOptionPane.ERROR_MESSAGE,JOptionPane.CLOSED_OPTION);
		dialog = option.createDialog(new JFrame(),"HELP");
		dialog.pack();
		dialog.setVisible(true);
		
	}
	
	
}
