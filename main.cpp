#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QCoreApplication>
#include <QDateTime>

#define NUM_OF_FILES 4

int num_of_input(const QVector<QString> &vector) {
    for(int i = 0; i < vector.size(); ++i) {
        if (vector[i].contains("DEPENDPATH")) {
            return i;
        }
    }
    return 0;
}

void cut_vector(int begin, int end, QVector<QString> &vector) {
    vector.remove(begin, end);
}

void full_vector(QTextStream &in, QVector<QString> &vector) {
    while (!(in.atEnd())) {
        vector.push_back(in.readLine());
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
       qDebug() << "Please use:";
       qDebug() << "\t./magxheader [*.pro file] [appname]";
       return (-1);
    }

    QDateTime dt = QDateTime::currentDateTime();
    QVector<QString> head_vector;
    QVector<QString> mid_vector;
    QVector<QString> end_vector;
    QString blank = "######################################################################";
    QString appname = QString("APPNAME = %1").arg(QString(argv[2]));
    QString dtstring = "# Generated by magxheader " + dt.toString("dd.MM.yyyy HH:mm:ss");

    QString pro_file = QString(argv[1]);
    if (!pro_file.contains(".pro")) {
            qDebug() << "This is not *.pro file!";
            return (-2);
    }
    QStringList dest = pro_file.split(".");
    pro_file = dest[0] + "_magx." + dest[1];

    QFile file[NUM_OF_FILES];
    file[0].setFileName(QString(argv[1]));
    file[1].setFileName("://header_p1");
    file[2].setFileName("://header_p2");
    file[3].setFileName(pro_file);

    for (int i = 0; i < NUM_OF_FILES; ++i) {
        if (i < 3) {
            if (!file[i].open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "Error openning" << file[i].fileName() << "for reading!";
                return (-3);
            }
        } else {
            if (!file[i].open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "Error openning" << file[i].fileName() << "for writing!";
                return (-4);
            }
        }
    }

    QTextStream textStream[NUM_OF_FILES];

    for (int i = 0; i < NUM_OF_FILES; ++i) {
        textStream[i].setCodec("UTF-8");
        textStream[i].setDevice(&file[i]);
    }

    full_vector(textStream[0], end_vector);
    cut_vector(0, num_of_input(end_vector), end_vector);
    full_vector(textStream[1], head_vector);
    full_vector(textStream[2], mid_vector);

    textStream[3] << blank << '\n';
    textStream[3] << dtstring << '\n';

    for (int i = 0; i < head_vector.size(); ++i) {
        textStream[3] << head_vector[i] << '\n';
    }

    textStream[3] << appname << '\n';

    for (int i = 0; i < mid_vector.size(); ++i) {
        textStream[3] << mid_vector[i] << '\n';
    }

    textStream[3] << '\n';

    for (int i = 0; i < end_vector.size(); ++i) {
        textStream[3] << end_vector[i] << '\n';
    }
    for (int i = 0; i < NUM_OF_FILES; ++i) {
        file[i].close();
    }
    qDebug() << "Writing " << pro_file << "...\nAll done!";

    return 0;
}