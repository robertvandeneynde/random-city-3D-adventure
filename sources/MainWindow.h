#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QHash>
#include <QStandardItemModel>
#include <QPushButton>
#include <QVector2D>
#include <vector>

#include "Chargement.h"
#include "Parametres.h"

class VilleInterface;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void afficherLogo();
    void fermerLogo();
    void creerVille();
    void verifierContraintes();
    void afficherSelection();
    void creerMap();
    void on_tabWidget_currentChanged(int);
    void fenetreFermee();

    void on_inputOptimisationVoiture_valueChanged(double);
    void on_inputFarPlane_valueChanged(double);
    void on_commandesAzerty_toggled(bool);
    void on_commandesQwerty_toggled(bool);
    void on_optionsOk_pressed();

    void mettreAJourCouleur(QColor);
private:
    void valeurErronee(QWidget *);
    void valeurCorrecte(QWidget *);
    void testerValeur(bool condition, QWidget *);
    void testerValeur(bool condition, QVector<QWidget*>);
    void afficherVille(VilleInterface * ville);
    void afficherVille(QString nom);
private:
    Ui::MainWindow *ui;
    bool m_correct;
    QHash<QString, VilleInterface*> m_villes;
    QStandardItemModel * m_modeleVilles;
    Chargement * m_fenetreChargement;
    OptionsDeJeu m_optionsDeJeu;
    std::vector< std::vector<QColor> > m_couleursBatiments;
    QHash<QObject*, std::vector<QColor>::iterator> m_boutonsCouleurs;
};

#endif // MAINWINDOW_H
