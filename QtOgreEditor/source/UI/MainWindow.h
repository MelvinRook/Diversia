/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_MAINWINDOW_H
#define DIVERSIA_QTOGREEDITOR_MAINWINDOW_H

#include "Platform/Prerequisites.h"

#include "UI/ConnectDialog.h"
#include "UI/NewGameDialog.h"
#include "ui_MainWindow.h"
#include <QProxyStyle>

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget* pParent = 0, Qt::WFlags flags = 0 );
    ~MainWindow();

    /**
    Initializes the main window. 
    **/
    void init();
    /**
    Gets the connect dialog. 
    **/
    inline ConnectDialog& getConnectDialog() { return mConnectDialog; }
    /**
    Updates the level quick load menu.
    **/
    void updateLevels();

    Ui::MainWindowClass mUI;
    QActionGroup* mGizmoActions;
    QActionGroup* mRecentGameGroup;
    QActionGroup* mLevelGroup;

private slots:
    void exit();
    void disconnect();
    void saveGame();
    void saveGameAs();
    void loadGame();
    void loadRecentGame();
    void saveLevel();
    void saveLevelAs();
    void loadLevel();
    void loadListLevel();
    void logSeverityChange( QWidget* pWidget );
    void logSourceChange( QWidget* pWidget );
    void play();
    void pause( bool pause );
    void stop();
    void undo();
    void redo();
    void gizmoModeChange( QAction* pAction );

private:
    friend class QtLogger;

    void _loadGame( const QString& rFile );
    void _saveGame( const QString& rFile );
    void _loadLevel( const QString& rFile );
    void _saveLevel( const QString& rFile );

    void checkLogItem( const QModelIndex& rIndex );
    bool isSeverityChecked( LogLevel logLevel );
    bool isSourceChecked( const String& rSource );
    void updateRecentGames();
    void closeEvent( QCloseEvent* pEvent );

    ConnectDialog       mConnectDialog;
    NewGameDialog       mNewGameDialog;
    
    static const unsigned short cMaxRecentGames = 5;
    static const unsigned short cMaxLevels = 20;
    QAction*    mRecentGameActions[cMaxRecentGames];
    QAction*    mRecentGameSeperator;
    QAction*    mLevelActions[cMaxLevels];
    
};

//------------------------------------------------------------------------------

class IconDockStyle: public QProxyStyle
{
    Q_OBJECT

public:
    IconDockStyle( const QIcon& rIcon, QStyle* pStyle = 0 );

    void drawControl( ControlElement element, const QStyleOption* pOption, QPainter* pPainter, 
        const QWidget* pWidget = 0 ) const;

private:
    QIcon mIcon;
};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_MAINWINDOW_H