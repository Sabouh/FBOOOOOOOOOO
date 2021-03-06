#include <qapplication.h>
#include <QString>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "viewer.h"

using namespace std;

char *getFilename(int argc,char **argv) {
  if(argc<2) {
    cout << "Usage: " << argv[0] << " offFile" << endl;
    exit(0);
  }

  return argv[1];
}

int main(int argc,char** argv) {
  QApplication application(argc,argv);

  Viewer viewer("../TP07/models/sphere-low-res.off");

  viewer.setWindowTitle("Exercice 07 - Deferred Shading");
  viewer.show();
  
  return application.exec();
}
