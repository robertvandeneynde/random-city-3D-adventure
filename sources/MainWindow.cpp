#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QStandardItemModel>
#include <QMessageBox>
#include <QDesktopWidget>


#include "Ville.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->inputCitySizeX, SIGNAL(valueChanged(int)), this, SLOT(verifierContraintes()));
    connect(ui->inputCitySizeY, SIGNAL(valueChanged(int)), this, SLOT(verifierContraintes()));
    connect(ui->inputMinBlockSize, SIGNAL(valueChanged(int)), this, SLOT(verifierContraintes()));
    connect(ui->inputMaxBlockSize, SIGNAL(valueChanged(int)), this, SLOT(verifierContraintes()));
    connect(ui->inputNom, SIGNAL(textChanged(QString)), this, SLOT(verifierContraintes()));

    connect(ui->boutonGenerer, SIGNAL(clicked()), this, SLOT(creerVille()));

    connect(ui->afficherSelection, SIGNAL(clicked()), this, SLOT(afficherSelection()));
    connect(ui->fichierMap, SIGNAL(clicked()), this, SLOT(creerMap()));

    m_modeleVilles = new QStandardItemModel(this);
    ui->vueListeVille->setModel(m_modeleVilles);
    ui->tabRendu->setPause(true);

    m_fenetreChargement = NULL;
    m_optionsDeJeu.performance.farPlane = ui->inputFarPlane->value();
    m_optionsDeJeu.performance.distanceVoiture = ui->inputOptimisationVoiture->value();
    m_optionsDeJeu.commandes.mode = ui->commandesAzerty->isChecked() ? Options::Commandes::AZERTY : Options::Commandes::QWERTY;
    on_optionsOk_pressed();
    //move(20,30);

    //Couleurs des batiments
    static const QColor couleursReeles[4][3] = {
        {0x892d29, 0xf6dec5, 0xffffcc},
        {0x990000, 0x804000, 0xFF9900},
        {0x000099, 0x7777BB, 0x999999},
        {0x333333, 0x77BB77, 0x008800}
    };
    //Voir aussi // "BB7777" "008800" "f5c4c4"
    BoutonCouleur* boutons[4][3] = {
        {ui->couleur00, ui->couleur01, ui->couleur02},
        {ui->couleur10, ui->couleur11, ui->couleur12},
        {ui->couleur20, ui->couleur21, ui->couleur22},
        {ui->couleur30, ui->couleur31, ui->couleur32}
    };

    m_couleursBatiments.assign(4, std::vector<QColor>(3) );
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            QColor coul = m_couleursBatiments[i][j] = couleursReeles[i][j];
            BoutonCouleur* cible = boutons[i][j];
            cible->setCouleur(coul);
            m_boutonsCouleurs[cible] = m_couleursBatiments[i].begin() + j;
            connect(cible, SIGNAL(couleurChangee(QColor)), this, SLOT(mettreAJourCouleur(QColor)));
        }
    }
}

void MainWindow::mettreAJourCouleur(QColor couleur)
{
    * m_boutonsCouleurs[sender()] = couleur;
}

MainWindow::~MainWindow()
{
    delete ui;
    for(QHash<QString, VilleInterface*>::iterator it = m_villes.begin() ; it != m_villes.end() ; ++it)
        delete it.value();
}

void MainWindow::on_inputFarPlane_valueChanged(double val)
{
    m_optionsDeJeu.performance.farPlane = val;
    ui->optionsOk->setDisabled(false);
}

void MainWindow::on_inputOptimisationVoiture_valueChanged(double val)
{
    m_optionsDeJeu.performance.distanceVoiture = val;
    ui->optionsOk->setDisabled(false);
}

void MainWindow::on_commandesAzerty_toggled(bool v)
{
    if(v)
        m_optionsDeJeu.commandes.mode = Options::Commandes::AZERTY;
    ui->optionsOk->setDisabled(false);
}

void MainWindow::on_commandesQwerty_toggled(bool v)
{
    if(v)
        m_optionsDeJeu.commandes.mode = Options::Commandes::QWERTY;
    ui->optionsOk->setDisabled(false);
}
void MainWindow::on_optionsOk_pressed()
{
    ui->tabRendu->setOptionsDeJeu(&m_optionsDeJeu);
    ui->optionsOk->setDisabled(true);
}

void MainWindow::on_tabWidget_currentChanged(int)
{
    ui->tabRendu->setPause(true);
    if(ui->tabWidget->currentWidget() == ui->tabRendu)
        showMaximized();
    else
        showNormal();
}

void MainWindow::testerValeur(bool condition, QWidget * cible)
{
    if(condition)
        valeurErronee(cible);
    else
        valeurCorrecte(cible);
}

void MainWindow::testerValeur(bool condition, QVector<QWidget *> cibles)
{
    if(condition)
        for(int i = 0 ; i < cibles.size() ; i++)
            valeurErronee(cibles[i]);
    else
        for(int i = 0 ; i < cibles.size() ; i++)
            valeurCorrecte(cibles[i]);
}

void MainWindow::verifierContraintes()
{
    bool mauvaisX = ui->inputCitySizeX->value() < ui->inputMinBlockSize->value();
    bool mauvaisY = ui->inputCitySizeY->value() < ui->inputMinBlockSize->value();
    bool mauvaisLimites = ui->inputMaxBlockSize->value() < 2 * ui->inputMinBlockSize->value() + 1;
    bool mauvaisNom = m_villes.contains(ui->inputNom->text());

    QVector<QWidget*> ciblesMauvaisX;
    QVector<QWidget*> ciblesMauvaisY;
    QVector<QWidget*> ciblesMauvaisLimites;
    QVector<QWidget*> ciblesMauvaisNom;

    ciblesMauvaisX << ui->inputCitySizeX << ui->labelCitySizeX;
    ciblesMauvaisY << ui->inputCitySizeY << ui->labelCitySizeY;
    ciblesMauvaisLimites
            << ui->inputMinBlockSize << ui->inputMaxBlockSize
            << ui->labelMinBlockSize << ui->labelMaxBlockSize;
    ciblesMauvaisNom << ui->inputNom << ui->labelNom;

    testerValeur(mauvaisX, ciblesMauvaisX);
    testerValeur(mauvaisY, ciblesMauvaisY);
    testerValeur(mauvaisLimites, ciblesMauvaisLimites);
    testerValeur(mauvaisNom, ciblesMauvaisNom);

    m_correct = !(mauvaisX || mauvaisY || mauvaisLimites || mauvaisNom);
    ui->boutonGenerer->setDisabled(!m_correct);
}
void MainWindow::creerVille()
{
    Parametres params;
    params.citySizeX = ui->inputCitySizeX->value();
    params.citySizeY = ui->inputCitySizeY->value();
    params.minBlockSize = ui->inputMinBlockSize->value();
    params.maxBlockSize = ui->inputMaxBlockSize->value();
    params.moyenneH = ui->inputMoyenneH->value();
    params.ecartTypeH = ui->inputEcartTypeH->value();
    params.activerJardins = ui->inputJardins->isChecked();
    params.tailleRouteMaximale = ui->inputTailleRoute->value();
    params.couleursBatiments = m_couleursBatiments;
    params.nombreDeVoituresPNJ = ui->inputNombreVoitures->value();

    QString nom = ui->inputNom->text();
    if(nom.isEmpty())
    {
        int n = 1;
        do {
            nom = "Ville " + QString::number(n++);
        } while(m_villes.contains(nom));
    }
    ui->inputNom->setText("");

    params.nom = nom;

    afficherLogo();

    VilleInterface * ville = new Ville(params, m_fenetreChargement);
    m_villes.insert(nom, ville);

    m_modeleVilles->appendRow( new QStandardItem(nom) );

    connect(ville, SIGNAL(generated()), this, SLOT(fermerLogo()));
}
void MainWindow::afficherLogo()
{
    delete m_fenetreChargement;
    m_fenetreChargement = new Chargement();
    m_fenetreChargement->show();
}
void MainWindow::fenetreFermee()
{
    m_fenetreChargement = NULL;
}
void MainWindow::fermerLogo()
{
    if(m_fenetreChargement)
        m_fenetreChargement->finDeChargement();

    if(ui->afficherRendu->isChecked())
        afficherVille( static_cast<VilleInterface*>(sender()) );
    if(ui->fermerEcranChargement->isChecked())
        if(m_fenetreChargement)
            m_fenetreChargement->close();
}

void MainWindow::afficherVille(VilleInterface * ville)
{
    ui->tabRendu->setVille(ville);
    ui->tabWidget->setTabText(1, ville->params().nom);
    ui->tabWidget->setCurrentIndex(1);
    ville->afficherArretes(ui->avecArretes->isChecked());
}
void MainWindow::afficherVille(QString nom)
{
    afficherVille(m_villes[nom]);
}

void MainWindow::valeurErronee(QWidget * cible)
{
    QFont font = cible->font();
    font.setUnderline(true);
    font.setBold(true);
    cible->setFont(font);
}
void MainWindow::valeurCorrecte(QWidget * cible)
{
    QFont font = cible->font();
    font.setUnderline(false);
    font.setBold(false);
    cible->setFont(font);
}
void MainWindow::afficherSelection()
{
    QItemSelectionModel *selection = ui->vueListeVille->selectionModel();
    if(selection->hasSelection())
    {
        QModelIndex indexElementSelectionne = selection->currentIndex();
        QVariant elementSelectionne = m_modeleVilles->data(indexElementSelectionne, Qt::DisplayRole);
        afficherVille( elementSelectionne.toString() );
    }
}

void MainWindow::creerMap()
{
    QItemSelectionModel *selection = ui->vueListeVille->selectionModel();
    if(selection->hasSelection())
    {
        QModelIndex indexElementSelectionne = selection->currentIndex();
        QVariant elementSelectionne = m_modeleVilles->data(indexElementSelectionne, Qt::DisplayRole);
        QString nom = elementSelectionne.toString();
        m_villes[nom]->afficherDansFichierTexte(nom + ".txt");
    }
}

