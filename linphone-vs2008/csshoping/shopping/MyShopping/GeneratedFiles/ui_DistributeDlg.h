/********************************************************************************
** Form generated from reading UI file 'distributedlg.ui'
**
** Created: Fri Oct 23 16:11:09 2015
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISTRIBUTEDLG_H
#define UI_DISTRIBUTEDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QTableView>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DistributeDlg
{
public:
    QLabel *label;
    QTableView *tableView;
    QLabel *label_2;
    QTextEdit *textEdit;
    QLabel *label_3;
    QPushButton *pushButton;
    QListView *listView;

    void setupUi(QWidget *DistributeDlg)
    {
        if (DistributeDlg->objectName().isEmpty())
            DistributeDlg->setObjectName(QString::fromUtf8("DistributeDlg"));
        DistributeDlg->resize(506, 520);
        label = new QLabel(DistributeDlg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 171, 21));
        tableView = new QTableView(DistributeDlg);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setGeometry(QRect(10, 30, 481, 171));
        label_2 = new QLabel(DistributeDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 210, 171, 21));
        textEdit = new QTextEdit(DistributeDlg);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(10, 230, 481, 31));
        label_3 = new QLabel(DistributeDlg);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 270, 171, 21));
        pushButton = new QPushButton(DistributeDlg);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(270, 480, 221, 31));
        listView = new QListView(DistributeDlg);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setGeometry(QRect(10, 290, 481, 171));

        retranslateUi(DistributeDlg);

        QMetaObject::connectSlotsByName(DistributeDlg);
    } // setupUi

    void retranslateUi(QWidget *DistributeDlg)
    {
        DistributeDlg->setWindowTitle(QApplication::translate("DistributeDlg", "\351\205\215\351\200\201\347\256\241\347\220\206", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DistributeDlg", "\350\257\267\351\200\211\346\213\251\351\205\215\351\200\201\345\221\230", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DistributeDlg", "\345\244\207\346\263\250\357\274\210\351\200\211\345\241\253\357\274\211", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DistributeDlg", "\350\257\267\347\241\256\350\256\244\346\202\250\347\232\204\351\205\215\351\200\201\344\277\241\346\201\257", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("DistributeDlg", "\347\241\256\350\256\244\351\205\215\351\200\201", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DistributeDlg: public Ui_DistributeDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISTRIBUTEDLG_H
