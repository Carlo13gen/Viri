package Notifier;

import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class GenericNotifier extends AbstractNotifier {
	
	public GenericNotifier() {
		super();
	}

	public void showMessage(String mess) {	
		option = new JOptionPane(mess,JOptionPane.INFORMATION_MESSAGE,JOptionPane.CLOSED_OPTION);
		dialog = option.createDialog(new JFrame(),"MESSAGE");
		dialog.pack();
		dialog.setVisible(true);
	}

	public void showMessage() {	
		option = new JOptionPane("",JOptionPane.INFORMATION_MESSAGE,JOptionPane.CLOSED_OPTION);
		dialog = option.createDialog(new JFrame(),"CREDITS");
		dialog.pack();
		dialog.setVisible(true);
	}
	
}
