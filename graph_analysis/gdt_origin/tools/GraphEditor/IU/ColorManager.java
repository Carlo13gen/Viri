 package IU;

import java.awt.Color;
import java.util.List;

public class ColorManager {
	Color jNodeBackground=JNode.getSharedbackground();
	Color jNodeBound=JNode.getSharedbound();
	Color jNodeLabel=JNode.getSharedlabelColor();
	Color jEdge=JEdge.getSharedcolor();
	Color jEdgeLabel=JEdge.getSharedlabelColor();
	Color jIncompleteEdge=JIncompleteEdge.getColor();
	List<JNode> jnodes;
	List<JEdge> jedges;
	JNode jn;
	JEdge je;
	JGraph jg;
	
	
	public ColorManager(JGraph jg){
		this.jg = jg;
		this.jnodes=jg.getJNodes();
		this.jedges=jg.getJEdges();
	}
	
	public ColorManager(JGraph jg,JNode jn) {
		this.jg = jg;
		this.jn = jn;
	}

	public ColorManager(JGraph jg,JEdge je) {
		this.jg = jg;
		this.je = je;
	}
	/*
	 * change jedges color
	 */
	public void changeJEdges(Color c){
		this.setJEdge(c);
		for(JEdge je:jedges)
			je.setColor(c);
		jg.repaint();
	}
	/*
	 * change color of single jedge
	 */
	public void changeJEdge(Color c){
		je.setColor(c);
		jg.repaint();
	}
	/*
	 * change jedges label color
	 */
	public void changeJEdgesLabel(Color c){
		this.setJEdgeLabel(c);
		for(JEdge je:jedges)
			je.setLabelColor(c);
		jg.repaint();
	}
	/*
	 * change label color of single jedge
	 */
	public void changeJEdgeLabel(Color c){
		je.setLabelColor(c);
		jg.repaint();
	}
	/*
	 * change jnodes label color 
	 */
	public void changeJNodesLabel(Color c){
		this.setJNodeLabel(c);
		for(JNode jn:jnodes)
			jn.setLabelColor(c);
		jg.repaint();
	}
	/*
	 * change label color of single jnode
	 */
	public void changeJNodeLabel(Color c) {
		jn.setLabelColor(c);
		jg.repaint();
	}
	/*
	 * change color jnodes
	 */
	public void changeJNodesBackground(Color c){
		this.setJNodeBackground(c);
		for(JNode jn:jnodes)
			jn.setBackground(c);
		jg.repaint();
	}
	/*
	 * change color of single jnode
	 */
	public void changeJNodeBackground(Color c) {
		jn.setBackground(c);
		jg.repaint();
	}
	/*
	 * change jnodes color bound
	 */
	public void changeJNodesBound(Color c){
		this.setJNodeBound(c);
		for(JNode jn:jnodes)
			jn.setBound(c);
		jg.repaint();
	}
	/*
	 * change color bound of single jnode
	 */
	public void changeJNodeBound(Color c) {
		jn.setBound(c);
		jg.repaint();
	}
	/*
	 * getter and setter methods
	 */
	public Color getJEdge() {
		return jEdge;
	}

	public void setJEdge(Color edge) {
		jEdge = edge;
	}

	public Color getJEdgeLabel() {
		return jEdgeLabel;
	}

	public void setJEdgeLabel(Color edgeLabel) {
		jEdgeLabel = edgeLabel;
	}

	public Color getJNodeLabel() {
		return jNodeLabel;
	}

	public void setJNodeLabel(Color nodeLabel) {
		jNodeLabel = nodeLabel;
	}

	public Color getJIncompleteEdge() {
		return jIncompleteEdge;
	}

	public void setJIncompleteEdge(Color incompleteEdge) {
		jIncompleteEdge = incompleteEdge;
	}

	public Color getJNodeBackground() {
		return jNodeBackground;
	}

	public void setJNodeBackground(Color nodeBackground) {
		jNodeBackground = nodeBackground;
	}

	public Color getJNodeBound() {
		return jNodeBound;
	}

	public void setJNodeBound(Color nodeBound) {
		jNodeBound = nodeBound;
	}
}
