#!/bin/bash
#
# Deploy umundocore.jar into maven repository
#
mvn2 deploy:deploy-file -DgroupId=org.umundo -DartifactId=umundocore -Dversion=0.0.5 \
  -DgeneratePom=true \
  -Dpackaging=jar \
  -Dfile=umundocore.jar \
  -DrepositoryId=tu.darmstadt.umundo \
  -Durl=scp://umundo.tk.informatik.tu-darmstadt.de/var/www/html/maven2
