package Notifier;

import javax.swing.JDialog;
import javax.swing.JOptionPane;

public abstract class AbstractNotifier {

	protected JOptionPane option;
	protected JDialog dialog;
	
	public abstract void showMessage();
	
}
