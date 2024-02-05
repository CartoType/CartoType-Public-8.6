#include "mainwindow.h"
#include <QApplication>

#ifdef Q_OS_MAC
static void RaiseFileLimitToMaximum()
    {
    rlimit rl;
    if (getrlimit(RLIMIT_NOFILE,&rl) == 0)
        {
        if (rl.rlim_max > rl.rlim_cur)
            {
            rl.rlim_cur = rl.rlim_max;
            if (setrlimit(RLIMIT_NOFILE,&rl) != 0)
                perror("setrlimit");
            }
        }
    else perror("getrlimit");
    }
#endif

int main(int argc,char* argv[])
    {
#ifdef Q_OS_WIN32
    _setmaxstdio(8192);
#endif

#ifdef Q_OS_MAC
    RaiseFileLimitToMaximum();
#endif

    QApplication::setAttribute(Qt::AA_UseOpenGLES);
    QApplication a(argc,argv);
    QCoreApplication::setOrganizationName("CartoType");
    QCoreApplication::setOrganizationDomain("cartotype.com");
    QCoreApplication::setApplicationName("Maps");
    MainWindow w;
    w.show();

    return a.exec();
    }

