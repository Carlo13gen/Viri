//-----------------------------
//
//   GdtJavaWrapper / Java
//   (c)2005 by INTEGRA Sistemi                               
//   Luigi Del Negro          
//                               
//-----------------------------
 
package GdtJavaWrapper;

import java.lang.*;


	public abstract 
	class
Drawer
	{
	protected int                 graphId;
	protected WrapperImplementation wrapper;
		
		public
	Drawer
		(Graph g)
		{
		graphId = g.getId();
		wrapper = WrapperImplementation.getInstance();
		}
	
	Drawer
		(OrthogonalGraph g)
		{
		graphId=g.getId();
		wrapper = WrapperImplementation.getInstance();
		}

		protected
		void
	finalize()
		{
		//System.out.println("Metodo finalize del Drawer");
		//wrapper.deleteDrawnGraph(graphId);}	
		}


		public
		void
	rotate(int angle)
		{
		wrapper.rotate(graphId, angle);
		}

	

		public
		void
	flipHorizontally()
		{
		wrapper.flipHorizontally(graphId);
		}

	
		public
		void
	flipVertically()
		{
		wrapper.flipVertically(graphId);
		}


	}
