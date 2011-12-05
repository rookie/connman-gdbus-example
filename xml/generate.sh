#!/bin/sh

gdbus-codegen \
	--c-namespace ConnmanProxy			\
	--generate-c-code src/connman-generated		\
	--interface-prefix net.connman.			\
	xml/connman.xml 

