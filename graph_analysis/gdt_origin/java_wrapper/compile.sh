#!/bin/sh


echo javac -d bin/ src/Node.java
javac -d bin/ src/Node.java
echo javac -d bin/ src/Edge.java
javac -d bin/ src/Edge.java
echo javac -d bin/ src/Wrapper.java
javac -d bin/ src/Wrapper.java
echo javac -classpath bin/ -d bin/ src/Graph.java
javac -classpath bin/ -d bin/ src/Graph.java
echo javac -classpath bin/ -d bin/ src/OrthogonalGraph.java
javac -classpath bin/ -d bin/ src/OrthogonalGraph.java
echo javac -classpath bin/ -d bin/ src/Drawer.java
javac -classpath bin/ -d bin/ src/Drawer.java
echo javac -classpath bin/ -d bin/ src/OrthogonalDrawer.java
javac -classpath bin/ -d bin/ src/OrthogonalDrawer.java

echo cd bin
cd bin
echo jar cvf GdtJavaWrapper.jar GdtJavaWrapper/*.*
jar cvf GdtJavaWrapper.jar GdtJavaWrapper/*.*

echo cd  ../cpp/include
cd  ../cpp/include

echo javah -jni -classpath ../../bin GdtJavaWrapper.WrapperImplementation 
javah -jni -classpath ../../bin GdtJavaWrapper.WrapperImplementation 

echo cd ../..
cd ../..


