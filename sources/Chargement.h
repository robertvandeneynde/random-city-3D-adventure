#ifndef CHARGEMENT_H
#define CHARGEMENT_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

namespace Ui {
class Chargement;
}

class Chargement : public QWidget
{
        Q_OBJECT
        
    public:
        explicit Chargement(QWidget *parent = 0);
        ~Chargement();
        void finDeChargement();
        void afficherTexte(QString);

    private :
        void setVitesse(int);
        void paintEvent(QPaintEvent *);
    private:
        Ui::Chargement *ui;

        std::vector<double> m_startAngle;
        std::vector<double> m_spanAngle;
        std::vector<QString> m_messages;
        int derniersMessages;
        double m_vitesseRotation;
        QTimer m_timer;
        QLayout* layoutLog;
};

#endif // CHARGEMENT_H
