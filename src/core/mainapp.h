/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINAPP_H
#define MAINAPP_H

class MainApp : public QObject, public Singleton<MainApp>
{
    Q_OBJECT

protected:
    MainApp();
    virtual ~MainApp();
    friend class Singleton<MainApp>;

};

#endif // MAINAPP_H
