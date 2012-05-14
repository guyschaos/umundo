#!/bin/bash
#
# Deploy umundocore.jar into maven repository
#
mvn deploy:deploy-file -DgroupId=org.umundo -DartifactId=umundocore -Dversion=0.0.3 \
  -DgeneratePom=true \
  -Dpackaging=jar \
  -Dfile=umundocore.jar \
  -DrepositoryId=tu.darmstadt.umundo \
  -Durl=file:/var/www/html/maven2
