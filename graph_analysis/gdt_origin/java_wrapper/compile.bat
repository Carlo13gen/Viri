
javac -d bin\ src\Node.java
javac -d bin\ src\Edge.java
javac -d bin\ src\Wrapper.java
javac -classpath bin\ -d bin\ src\Graph.java
javac -classpath bin\ -d bin\ src\Drawer.java
javac -classpath bin\ -d bin\ src\OrthogonalGraph.java
javac -classpath bin\ -d bin\ src\OrthogonalDrawer.java


cd bin
jar cvf GdtJavaWrapper.jar GdtJavaWrapper/*.*
cd  ..\cpp\include
javah -jni -classpath ..\..\bin GdtJavaWrapper.WrapperImplementation
 

cd ..\..
