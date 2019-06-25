package Notifier;

import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class JWorkbenchNotifier extends AbstractNotifier{
	
	public JWorkbenchNotifier() {
		super();
	}
	
	public void showMessage() {
		option = new JOptionPane("- Left click to create a new node\n" +
				"- Left click on a node to create a new edge\n" +
				"- Left click on edge to orient it\n" +
				"- Right click on an edge or node to change his property or deleting it\n" +
				"- Hold down right button on node to move it\n" +
				"- Hold down right button on bend to move it\n" +
				"- Press ctrl+right click to delete node or edge\n" +
				"- Press canc for delete bend while a edge is incomplete\n ",JOptionPane.INFORMATION_MESSAGE,JOptionPane.CLOSED_OPTION);
		dialog = option.createDialog(new JFrame(),"HELP");
		dialog.pack();
		dialog.setVisible(true);
	}

}
