#ifndef BOUTONCOULEUR_H
#define BOUTONCOULEUR_H

#include <QPushButton>
#include <QColorDialog>
#include <QPainter>

class BoutonCouleur : public QPushButton
{
        Q_OBJECT
    public:
        explicit BoutonCouleur(QWidget *parent = 0);
        void setCouleur(QColor);

    protected :
        void paintEvent(QPaintEvent *);
        
    signals:
        void couleurChangee(QColor);

    public slots:
        void onClicked();

    private :
        QColor maCouleur;
};

#endif // BOUTONCOULEUR_H
