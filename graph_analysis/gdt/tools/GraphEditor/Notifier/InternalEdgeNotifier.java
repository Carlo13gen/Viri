package Notifier;

import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class InternalEdgeNotifier extends AbstractNotifier {
	
	public InternalEdgeNotifier() {
		super();
	}

	public void showMessage() {
		option = new JOptionPane("cannot create edge, it's internal to node!",JOptionPane.ERROR_MESSAGE,JOptionPane.CLOSED_OPTION);
		dialog = option.createDialog(new JFrame(),"WARNING");
		dialog.pack();
		dialog.setVisible(true);
	}
}
