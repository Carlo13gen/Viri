package Notifier;


import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class OrthoDrawerNotifier extends AbstractNotifier{
	
	public OrthoDrawerNotifier() {
		super();
	}

	public void showMessage() {
		option = new JOptionPane("the graph is not connected or there is a self-loop!",JOptionPane.ERROR_MESSAGE,JOptionPane.CLOSED_OPTION);
		dialog = option.createDialog(new JFrame(),"WARNING");
		dialog.pack();
		dialog.setVisible(true);
	}
}
