#include "Chargement.h"
#include "ui_Chargement.h"

#include <QVBoxLayout>
#include <QStaticText>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QDebug>
#include <algorithm>

Chargement::Chargement(QWidget *parent) :
    QWidget(parent),
    derniersMessages(0),
    ui(new Ui::Chargement)
{
    ui->setupUi(this);

    int startAngles[4] = {90,85,76,50};
    int spanAngles[4] = {88,95,114,140};
    m_startAngle.assign(startAngles, startAngles+4);
    m_spanAngle.assign(spanAngles, spanAngles+4);

    m_vitesseRotation = -300;

    m_timer.setInterval(25);
    m_timer.setSingleShot(false);

    connect(&m_timer, SIGNAL(timeout()),this,SLOT(repaint()));
    //connect(ui->sliderVitesse, SIGNAL(sliderMoved(int)),this,SLOT(setVitesse(int)));

    //QPalette palette = ui->sliderVitesse->palette() ;
    //palette.setColor( QPalette::Button , Qt::yellow ) ;
    //ui->sliderVitesse->setPalette( palette ) ;

    m_timer.start();

    ui->log->setLayout(layoutLog = new QVBoxLayout());
    // ui->label->setStyleSheet("QLabel { background-color : black; color : yellow; }");
}

Chargement::~Chargement()
{
    delete ui;
}

void Chargement::paintEvent(QPaintEvent *)
{
    QRectF rectangles[4] = { QRectF(20.0, 20.0, 290.0, 240.0)
                           , QRectF(38.0, 55.0, 180, 160)
                           , QRectF(56.0, 85.0, 100, 80)
                           , QRectF(76.0, 105.0, 40, 40)
                           };

    QStaticText text("<font size = 10><strong>RANDOM ASSEMBLY GAMES</strong></font>");
    text.setTextFormat(Qt::RichText);

    QStaticText names(QString::fromUtf8("<font size = 4>by Robert VANDEN EYNDE & Aurélien MARION</font>"));
    names.setTextFormat(Qt::RichText);
    names.setTextWidth(300);

    QPen pen(Qt::yellow);
    pen.setWidth(10);
    QBrush back(Qt::black);
    QPainter p;
    p.begin(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(QRectF(0, 0, geometry().width(), geometry().height()), back);
    p.setPen(pen);
    for(int i = 0; i < 4; i++)
    {
        p.drawArc(rectangles[i], m_startAngle[i]*16, m_spanAngle[i]*16);
        m_startAngle[i] += m_vitesseRotation*16.0/1000.0;
    }
    pen.setWidth(10);
    p.setPen(pen);
    p.fillRect(QRectF(90,118,170,107), back);
    p.drawStaticText(100,125,text);
    p.fillRect(QRectF(10,247,238,18), back);
    p.drawStaticText(20,260,names);

    /*for(unsigned int i = 0, n = std::min<unsigned int>(m_messages.size(), 10) ; i < n ; i++)
    {
        QStaticText texte(m_messages[m_messages.size() - 1 - i]);
        texte.setTextWidth(size().width() - 350);
        p.drawStaticText(350, 250 - i *25, texte);
    }*/

    for( ; derniersMessages < (int) m_messages.size() ; derniersMessages++)
        layoutLog->addWidget(new QLabel(m_messages[derniersMessages]));

    p.end();
}
void Chargement::finDeChargement()
{
    setVitesse(0);
}

void Chargement::setVitesse(int vitesse)
{
    m_vitesseRotation = -vitesse;
}

void Chargement::afficherTexte(QString str)
{
    m_messages.push_back(str);
}
