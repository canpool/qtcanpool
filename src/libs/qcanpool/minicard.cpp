/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#include "minicard.h"
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "qcanpool.h"

class MiniCardPrivate : public QObject
{
    Q_OBJECT
public:
    MiniCardPrivate(QObject *parent = 0);
    void init();
    void createWindow();

public:
    MiniCard *q;

    QLabel *m_pTitleLabel;
    QToolButton *m_pInfoButton;
    QLabel *m_pValueLabel;
    QLabel *m_pDescLabel;
    QToolButton *m_pNoteButton;
    QWidget *m_pSpacer;

    QColor m_infoColor;
    QColor m_infoBkcolor;
    QColor m_spacerColor;
    QColor m_noteColor;
    QColor m_valueColor;
    QColor m_titleColor;
    QColor m_descColor;
    QColor m_backgroundColor;

private slots:
    void slotNoteClicked();
    void slotInfoClicked();

};

MiniCardPrivate::MiniCardPrivate(QObject *parent)
    : QObject(parent)
{
    m_infoColor = QColor(255,255,255);
    m_infoBkcolor = QColor(28,132,198);
    m_spacerColor = QColor(230,230,230);
    m_noteColor = QColor(28,132,198);
    m_valueColor = QColor(50,50,50);
    m_titleColor = QColor(50,50,50);
    m_descColor = QColor(50,50,50);
    m_backgroundColor = QColor(255,255,255);
}

void MiniCardPrivate::init()
{
    createWindow();

    q->setInfoColor(m_infoColor);
    q->setInfoBackgroundColor(m_infoBkcolor);
    q->setTitleColor(m_titleColor);
    q->setSpacerColor(m_spacerColor);
    q->setNoteColor(m_noteColor);
    q->setValueColor(m_valueColor);
    q->setDescColor(m_descColor);
    q->setBackgroundColor(m_backgroundColor);

    q->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void MiniCardPrivate::createWindow()
{
    QFont font("Segoe UI",10);;

    m_pTitleLabel = new QLabel(tr("Title"));
    font.setBold(true);
    font.setPointSize(12);
    m_pTitleLabel->setFont(font);

    m_pInfoButton = new QToolButton();
    connect(m_pInfoButton, SIGNAL(clicked(bool)), this, SLOT(slotInfoClicked()));
    m_pInfoButton->setText(tr("  Information  "));
    font.setPointSize(11);
    m_pInfoButton->setFont(font);

    m_pValueLabel = new QLabel(tr("Value"));
    font.setBold(false);
    font.setPointSize(24);
    m_pValueLabel->setFont(font);

    m_pNoteButton = new QToolButton();
    connect(m_pNoteButton, SIGNAL(clicked(bool)), this, SLOT(slotNoteClicked()));
    m_pNoteButton->setText(tr("Note"));
    font.setBold(true);
    font.setPointSize(10);
    m_pNoteButton->setFont(font);

    m_pDescLabel = new QLabel(tr("Description"));
    font.setBold(false);
    font.setPointSize(10);
    m_pDescLabel->setFont(font);


    QHBoxLayout *pTitleLayout = new QHBoxLayout();
    pTitleLayout->setSpacing(10);
    pTitleLayout->setContentsMargins(12,15,12,10);
    pTitleLayout->addWidget(m_pTitleLabel);
    pTitleLayout->addStretch();
    pTitleLayout->addWidget(m_pInfoButton);

    QHBoxLayout *pValueLayout = new QHBoxLayout();
    pValueLayout->setSpacing(5);
    pValueLayout->setContentsMargins(12,5,0,0);
    pValueLayout->addWidget(m_pValueLabel);
    pValueLayout->addStretch();

    QHBoxLayout *pDescLayout = new QHBoxLayout();
    pDescLayout->setSpacing(5);
    pDescLayout->setContentsMargins(12, 0, 12,15);
    pDescLayout->addWidget(m_pDescLabel);
    pDescLayout->addStretch();
    pDescLayout->addWidget(m_pNoteButton);

    m_pSpacer = new QWidget();
    m_pSpacer->setFixedHeight(1);

    QVBoxLayout *pMainLayout = new QVBoxLayout();
    pMainLayout->setSpacing(0);
    pMainLayout->setContentsMargins(0,0,0,0);
    pMainLayout->addLayout(pTitleLayout);
    pMainLayout->addWidget(m_pSpacer);
    pMainLayout->addLayout(pValueLayout);
    pMainLayout->addLayout(pDescLayout);

    q->setLayout(pMainLayout);
}

void MiniCardPrivate::slotNoteClicked()
{
    QString text = m_pNoteButton->text();
    text = text.trimmed();
    emit q->sigNoteClicked(text);
}

void MiniCardPrivate::slotInfoClicked()
{
    QString text = m_pInfoButton->text();
    text = text.trimmed();
    emit q->sigInfoClicked(text);
}

MiniCard::MiniCard(QWidget *parent)
    : QWidget(parent),d(new MiniCardPrivate(this))
{
    d->q = this;
    d->init();
}

void MiniCard::setTitle(const QString &text)
{
    d->m_pTitleLabel->setText(text);
}

void MiniCard::setInfo(const QString &text)
{
    d->m_pInfoButton->setText(QString("  %1  ").arg(text));
}

void MiniCard::setValue(const QString &text)
{
    d->m_pValueLabel->setText(text);
}

void MiniCard::setDesc(const QString &text)
{
    d->m_pDescLabel->setText(text);
}

void MiniCard::setNote(const QString &text)
{
    d->m_pNoteButton->setText(text);
}

void MiniCard::setToolTip(const QString &tips)
{
    d->m_pInfoButton->setToolTip(tips);
}

void MiniCard::setInfoColor(const QColor &color)
{
    d->m_infoColor = color;
    d->m_pInfoButton->setStyleSheet(QString("background-color:#%1; color:#%2;")
                                .arg(QCanpool::colorToArgb(d->m_infoBkcolor))
                                .arg(QCanpool::colorToArgb(d->m_infoColor))
                                );
}

void MiniCard::setInfoBackgroundColor(const QColor &color)
{
    d->m_infoBkcolor = color;
    d->m_pInfoButton->setStyleSheet(QString("border:0px; background-color:#%1; color:#%2;")
                                    .arg(QCanpool::colorToArgb(d->m_infoBkcolor))
                                    .arg(QCanpool::colorToArgb(d->m_infoColor))
                                    );
}

void MiniCard::setSpacerColor(const QColor &color)
{
    d->m_spacerColor = color;
    d->m_pSpacer->setStyleSheet(QString("background-color:#%1;")
                                .arg(QCanpool::colorToArgb(color))
                                );
}

void MiniCard::setNoteColor(const QColor &color)
{
    d->m_noteColor = color;
    d->m_pNoteButton->setStyleSheet(QString("background-color:transparent;color:#%1;")
                                    .arg(QCanpool::colorToArgb(color))
                                        );
}

void MiniCard::setTitleColor(const QColor &color)
{
    d->m_titleColor = color;
    d->m_pTitleLabel->setStyleSheet(QString("background-color:transparent;color:#%1;")
                                    .arg(QCanpool::colorToArgb(color))
                                    );
}

void MiniCard::setValueColor(const QColor &color)
{
    d->m_valueColor = color;
    d->m_pValueLabel->setStyleSheet(QString("background-color:transparent;color:#%1;")
                                    .arg(QCanpool::colorToArgb(color))
                                    );
}

void MiniCard::setDescColor(const QColor &color)
{
    d->m_descColor = color;
    d->m_pDescLabel->setStyleSheet(QString("background-color:transparent;color:#%1;")
                                .arg(QCanpool::colorToArgb(color))
                                );
}

void MiniCard::setBackgroundColor(const QColor &color)
{
    d->m_backgroundColor = color;
    QPalette palette;
    palette.setColor(QPalette::Background, color);
    this->setPalette(palette);
    this->setAutoFillBackground(true);
}

#include "minicard.moc"
